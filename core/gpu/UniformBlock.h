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
#pragma once
#include <memory>
#include <string>
#include <vector>

#include "Std140LayoutBuilder.h"
#include "UniformInjectContext.h"
#include "UniformInjectorRegistry.h"
#include "core/mod/Resource.h"

namespace glimmer {
    /**
     * CompiledUniformMember
     * 编译后的 Uniform 成员
     */
    struct CompiledUniformMember {
        std::string name;
        UniformScalarType type = UniformScalarType::Float;
        uint32_t offset = 0;
        uint32_t size = 0;
        //Value source: "static" for a fixed value, otherwise an "@builtin.xxx" name.
        //值来源："static" 为固定值，否则为 "@builtin.xxx" 语义名。
        std::string source;
        //Static value, valid only when source == "static".
        //静态值，仅 source == "static" 时有效。
        std::vector<float> staticValue;
    };

    /**
     * CompiledUniformBlock
     * 编译后的 Uniform 块
     *
     * Holds the resolved std140 layout plus a pre-baked buffer whose static
     * members are already written. At frame time the renderer copies this
     * buffer into its staging buffer and only injects the dynamic members.
     * 保存解析后的 std140 布局，以及一个已写入静态成员的预烘焙缓冲区。
     * 运行时渲染器将缓冲区拷入 staging buffer，仅注入动态成员。
     */
    class CompiledUniformBlock {
    public:
        /**
         * Compile
         * 从 UniformBlockResource 编译出布局与静态缓冲区。
         * @param resource resource 描述资源
         * @return 编译产物；类型非法时返回 nullptr。
         */
        static std::unique_ptr<CompiledUniformBlock> Compile(const UniformBlockResource &resource);

        [[nodiscard]] uint32_t GetSet() const;

        [[nodiscard]] uint32_t GetBinding() const;

        [[nodiscard]] size_t GetSize() const;

        [[nodiscard]] const std::vector<CompiledUniformMember> &GetMembers() const;

        [[nodiscard]] const std::vector<uint32_t> &GetDynamicMemberIndices() const;

        [[nodiscard]] const std::vector<uint8_t> &GetStaticBuffer() const;

        /**
         * Fill
         * 填充输出缓冲区：先恢复静态成员，再逐动态成员调用注入器写入值。
         * @param ctx ctx 注入上下文
         * @param out out 输出缓冲区（大小会被重置为本块大小）
         */
        void Fill(const UniformInjectContext &ctx, std::vector<uint8_t> &out) const;

    private:
        uint32_t set_ = 0;
        uint32_t binding_ = 0;
        std::vector<CompiledUniformMember> members_;
        std::vector<uint32_t> dynamicMemberIndices_;
        std::vector<uint8_t> staticBuffer_;
    };
}
