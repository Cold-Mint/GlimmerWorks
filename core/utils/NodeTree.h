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

#include <list>
#include <optional>
#include <vector>
#include <functional>

namespace glimmer {
    template<typename T>
    class NodeTree {
    public:
        /**
         * The constructor 'data' can be empty and is represented as' optional '
         * 构造函数，data 支持可空，用 optional 表示
         * @param data Data 数据
         */
        explicit NodeTree(std::optional<T> data = std::nullopt)
            : data_(std::move(data)) {
        }

        /**
         * Obtain data
         * 获取数据
         * @return
         */
        const std::optional<T> &Data() const {
            return data_;
        }

        /**
         * Add child nodes
         * 添加子节点
         * @param value Add child nodes 添加子节点
         * @return
         */
        NodeTree *AddChild(const T &value) {
            children_.emplace_back(value);
            return &children_.back();
        }

        /**
         * Obtain the data of all child nodes (without traversing subtrees)
         * 获取所有子节点的数据（不会遍历子树）
         * @return
         */
        std::vector<T> GetAllChildren() const {
            std::vector<T> result;
            result.reserve(children_.size());

            for (const auto &child: children_) {
                if (child.Data().has_value()) {
                    result.push_back(child.Data().value());
                }
            }
            return result;
        }

        /**
         * Traverse the child nodes (not the subtrees)
         * 遍历子节点（不会遍历子树）
         * @param callback If the callback returns false, the traversal is interrupted  callback返回 false 则中断遍历
         */
        void ForEachChildren(const std::function<bool(NodeTree *)> &callback) {
            for (auto &child: children_) {
                if (!callback(&child)) {
                    return;
                }
            }
        }

        /**
         * Remove child nodes based on value
         * 根据 value 移除子节点
         * @param value value 值
         * @return
         */
        bool RemoveChild(const T &value) {
            for (auto it = children_.begin(); it != children_.end(); ++it) {
                if (it->Data().has_value() && it->Data().value() == value) {
                    children_.erase(it);
                    return true;
                }
            }
            return false;
        }

        /**
         * Obtain the child nodes based on the value
         * 根据 value 获取子节点
         * @param value value 值
         * @return
         */
        [[nodiscard]] NodeTree *GetChildByValue(const T &value) {
            for (auto &child: children_) {
                if (child.Data().has_value() && child.Data().value() == value) {
                    return &child;
                }
            }
            return nullptr;
        }

        [[nodiscard]] const NodeTree *GetChildByValue(const T &value) const {
            for (const auto &child: children_) {
                if (child.Data().has_value() && child.Data().value() == value) {
                    return &child;
                }
            }
            return nullptr;
        }

        /**
         * Obtain the child nodes
         * 获取子节点
         * @param index 下标
         * @return
         */
        [[nodiscard]] NodeTree *GetChild(const int index) {
            if (index < 0) return nullptr;

            int counter = 0;
            for (auto &child: children_) {
                if (counter == index) {
                    return &child;
                }
                counter++;
            }
            return nullptr;
        }

        [[nodiscard]] const NodeTree *GetChild(const int index) const {
            if (index < 0) return nullptr;

            int counter = 0;
            for (const auto &child: children_) {
                if (counter == index) {
                    return &child;
                }
                counter++;
            }
            return nullptr;
        }


        /**
         * Clear the connections of child nodes
         * 清理子节点的连接
         */
        void ClearChildren() {
            children_.clear();
        }

        /**
         * Obtain the length of the child node
         * 获取子节点的长度
         * @return
         */
        [[nodiscard]] int GetSize() const {
            return children_.size();
        }

    private:
        std::optional<T> data_;
        std::list<NodeTree> children_;
    };
}
