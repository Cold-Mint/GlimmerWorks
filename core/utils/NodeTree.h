//
// Created by Cold-Mint on 2025/11/27.
//

#ifndef GLIMMERWORKS_NODETREE_H
#define GLIMMERWORKS_NODETREE_H

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
            for (auto it = children_.begin(); it != children_.end(); ++it) { //skipcq: CXX-W1161
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
        NodeTree *GetChildByValue(const T &value) {
            for (auto &child: children_) {
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
        NodeTree *GetChild(const int index) {
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
} // namespace glimmer

#endif //GLIMMERWORKS_NODETREE_H
