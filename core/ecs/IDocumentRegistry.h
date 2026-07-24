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
#pragma once
#include "RmlUi/Core/DataModelHandle.h"

namespace Rml
{
    class ElementDocument;
}

namespace glimmer
{
    class ResourceRef;
    /**
     * Rml document registry
     * Rml文档注册表
     *
     * Methods for providing registration, querying Rml documents and data models to the outside world
     * 对外提供注册，查询Rml文档和数据模型的方法
     *
     * Used to implement restricted method access within the global environment.
     * 用于在世界场景内实现受限的方法访问。
     */
    class IDocumentRegistry
    {
    public:
        virtual ~IDocumentRegistry() = default;

        /**
         * LoadSingleDocument
         * 加载单个文档
         * @param resourceRef resourceRef
         * @return
         */
        virtual Rml::ElementDocument* LoadSingleDocument(const ResourceRef* resourceRef) = 0;

        /**
         * CreateDataModel
         * 创建数据模型
         * @param name
         * @return
         */
        virtual Rml::DataModelConstructor* CreateDataModel(const Rml::String& name) = 0;
    };
}
