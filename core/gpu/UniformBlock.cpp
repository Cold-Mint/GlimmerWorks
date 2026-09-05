/*
 * Copyright (C) 2025-2026  Cold-Mint <cold_mint@qq.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * 版权(C) 2025-2026  Cold-Mint <cold_mint@qq.com>
 *
 * 本程序是自由软件：你可以遵照自由软件基金会出版的GNU Affero通用公共许可证条款来重新分发和修改它
 * 该许可证的第3版，或者（由你选择）任何后续版本。
 *
 * 本程序的发布目的是希望它能有用，但没有任何担保；甚至没有适销性或特定用途适用性的默示担保。
 * 有关详细细节，请参阅GNU Affero通用公共许可证。
 *
 * 你应该已经收到一份GNU Affero通用公共许可证的副本。如果没有，请查阅<https://www.gnu.org/licenses/>。
 */
#include "UniformBlock.h"

#include <cstring>

#include "core/log/LogCat.h"

std::unique_ptr<glimmer::CompiledUniformBlock> glimmer::CompiledUniformBlock::Compile(
    const UniformBlockResource &resource) {
    auto block = std::make_unique<CompiledUniformBlock>();
    block->set_ = resource.set;
    block->binding_ = resource.binding;

    uint32_t cursor = 0;
    block->members_.reserve(resource.members.size());
    for (const UniformMemberResource &member: resource.members) {
        CompiledUniformMember compiled;
        compiled.name = member.name;
        compiled.source = member.source;
        compiled.staticValue = member.value;
        compiled.type = Std140LayoutBuilder::ParseType(member.type);
        if (compiled.type == UniformScalarType::Invalid) {
            LogCat::w(std::source_location::current(), "Invalid uniform member type: ", member.type,
                      " for member ", member.name);
            return nullptr;
        }
        const uint32_t alignment = Std140LayoutBuilder::GetAlignment(compiled.type);
        const uint32_t size = Std140LayoutBuilder::GetSize(compiled.type);
        cursor = (cursor + alignment - 1) & ~(alignment - 1);
        compiled.offset = cursor;
        compiled.size = size;
        cursor += size;
        block->members_.push_back(std::move(compiled));
    }

    //Align the total size to a 16-byte boundary (std140 uniform block granularity).
    //将总大小对齐到 16 字节边界（std140 uniform 块粒度）。
    const uint32_t totalSize = (cursor + 15) & ~15u;
    block->staticBuffer_.assign(totalSize, 0);

    //Bake static members and collect dynamic member indices.
    //烘焙静态成员并收集动态成员索引。
    for (size_t i = 0; i < block->members_.size(); ++i) {
        CompiledUniformMember &member = block->members_[i];
        if (member.source == "static") {
            const uint32_t componentCount = Std140LayoutBuilder::GetComponentCount(member.type);
            auto *dst = reinterpret_cast<float *>(block->staticBuffer_.data() + member.offset);
            for (uint32_t c = 0; c < componentCount; ++c) {
                dst[c] = c < member.staticValue.size() ? member.staticValue[c] : 0.0F;
            }
        } else {
            block->dynamicMemberIndices_.push_back(static_cast<uint32_t>(i));
        }
    }

    return block;
}

uint32_t glimmer::CompiledUniformBlock::GetSet() const {
    return set_;
}

uint32_t glimmer::CompiledUniformBlock::GetBinding() const {
    return binding_;
}

size_t glimmer::CompiledUniformBlock::GetSize() const {
    return staticBuffer_.size();
}

const std::vector<glimmer::CompiledUniformMember> &glimmer::CompiledUniformBlock::GetMembers() const {
    return members_;
}

const std::vector<uint32_t> &glimmer::CompiledUniformBlock::GetDynamicMemberIndices() const {
    return dynamicMemberIndices_;
}

const std::vector<uint8_t> &glimmer::CompiledUniformBlock::GetStaticBuffer() const {
    return staticBuffer_;
}

void glimmer::CompiledUniformBlock::Fill(const UniformInjectContext &ctx, std::vector<uint8_t> &out) const {
    out.assign(staticBuffer_.size(), 0);
    std::memcpy(out.data(), staticBuffer_.data(), staticBuffer_.size());
    for (const uint32_t memberIndex: dynamicMemberIndices_) {
        const CompiledUniformMember &member = members_[memberIndex];
        const UniformInjector injector = UniformInjectorRegistry::Find(member.source);
        if (injector == nullptr) {
            LogCat::w(std::source_location::current(), "Uniform injector not found: ", member.source);
            continue;
        }
        injector(ctx, reinterpret_cast<float *>(out.data() + member.offset));
    }
}
