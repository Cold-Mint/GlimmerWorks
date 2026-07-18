/*
* Copyright (C) 2025  Cold-Mint <cold_mint@qq.com>
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
 * 版权(C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * 本程序是自由软件：你可以遵照自由软件基金会出版的GNU Affero通用公共许可证条款来重新分发和修改它
 * 该许可证的第3版，或者（由你选择）任何后续版本。
 *
 * 本程序的发布目的是希望它能有用，但没有任何担保；甚至没有适销性或特定用途适用性的默示担保。
 * 有关详细细节，请参阅GNU Affero通用公共许可证。
 *
 * 你应该已经收到一份GNU Affero通用公共许可证的副本。如果没有，请查阅<https://www.gnu.org/licenses/>。
 */
#include "GameFileInterface.h"

#include "core/log/LogCat.h"
#include "core/utils/StringUtils.h"

glimmer::GameFileInterface::GameFileInterface(VirtualFileSystem* virtualFileSystem) : virtualFileSystem_(
    virtualFileSystem)
{
}

Rml::FileHandle glimmer::GameFileInterface::Open(const Rml::String& path)
{
    if (virtualFileSystem_ == nullptr)
    {
        LogCat::w(std::source_location::current(), "virtualFileSystem_ == nullptr");
        return 0;
    }
    std::unique_ptr<std::istream> stream = virtualFileSystem_->ReadFileAsStream(path);
    if (stream == nullptr)
    {
        LogCat::w(std::source_location::current(), "stream == nullptr");
        return 0;
    }
    indexFileHandle_++;
    streamMap_[indexFileHandle_] = std::move(stream);
    return indexFileHandle_;
}

void glimmer::GameFileInterface::Close(Rml::FileHandle file)
{
    streamMap_.erase(file);
}

size_t glimmer::GameFileInterface::Read(void* buffer, size_t size, Rml::FileHandle file)
{
    const auto it = streamMap_.find(file);
    if (it == streamMap_.end())
    {
        LogCat::w(std::source_location::current(), "it == streamMap_.end()");
        return 0;
    }
    std::istream* stream = it->second.get();
    stream->read(static_cast<char*>(buffer), static_cast<std::streamsize>(size));
    return static_cast<size_t>(stream->gcount());
}

bool glimmer::GameFileInterface::Seek(Rml::FileHandle file, long offset, int origin)
{
    const auto it = streamMap_.find(file);
    if (it == streamMap_.end())
    {
        LogCat::w(std::source_location::current(), "it == streamMap_.end()");
        return false;
    }
    std::istream* stream = it->second.get();
    std::ios::seekdir seekDir;
    switch (origin)
    {
    case SEEK_SET:
        seekDir = std::ios::beg;
        break;
    case SEEK_CUR:
        seekDir = std::ios::cur;
        break;
    case SEEK_END:
        seekDir = std::ios::end;
        break;
    default:
        LogCat::w(std::source_location::current(), "invalid origin: ", origin);
        return false;
    }
    stream->seekg(offset, seekDir);
    return !stream->fail();
}

size_t glimmer::GameFileInterface::Tell(Rml::FileHandle file)
{
    const auto it = streamMap_.find(file);
    if (it == streamMap_.end())
    {
        LogCat::w(std::source_location::current(), "index:", file, "it == streamMap_.end()");
        return 0;
    }
    std::istream* stream = it->second.get();
    //Clear the error flag.
    //清空错误标志。
    stream->clear();
    const std::streampos pos = stream->tellg();
    if (pos == std::streampos(-1))
    {
        LogCat::w(std::source_location::current(), "index:", file, " pos == std::streampos(-1)");
        return 0;
    }
    return pos;
}

size_t glimmer::GameFileInterface::Length(Rml::FileHandle file)
{
    const auto it = streamMap_.find(file);
    if (it == streamMap_.end())
    {
        LogCat::w(std::source_location::current(), "it == streamMap_.end() ");
        return 0;
    }
    std::istream* stream = it->second.get();
    const std::streampos currentPos = stream->tellg();
    if (currentPos == std::streampos(-1))
    {
        LogCat::w(std::source_location::current(), "currentPos == std::streampos(-1)");
        return 0;
    }
    stream->seekg(0, std::ios::end);
    const std::streampos endPos = stream->tellg();
    if (endPos == std::streampos(-1))
    {
        LogCat::w(std::source_location::current(), "endPos == std::streampos(-1)");
        stream->seekg(currentPos);
        return 0;
    }
    const size_t length = endPos;
    stream->seekg(currentPos);
    return length;
}

bool glimmer::GameFileInterface::LoadFile(const Rml::String& path, Rml::String& out_data)
{
    if (virtualFileSystem_ == nullptr)
    {
        LogCat::w(std::source_location::current(), "virtualFileSystem_ == nullptr");
        return false;
    }
    if (std::optional<std::string> string = virtualFileSystem_->ReadFileAsString(path); string.has_value())
    {
        out_data = string.value();
        return true;
    }
    LogCat::w(std::source_location::current(), "readFile Error");
    return false;
}

glimmer::GameFileInterface::~GameFileInterface()
{
    streamMap_.clear();
}
