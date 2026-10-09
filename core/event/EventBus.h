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

#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "Subscription.h"

namespace glimmer {
    /**
     * EventBus
     * 事件总线
     * A thread-safe, type-keyed publish/subscribe bus. Handlers are invoked
     * synchronously on the thread that calls Publish.
     * 一个线程安全、以类型为键的发布/订阅总线。处理器在调用 Publish 的线程上同步执行。
     */
    class EventBus {
        /**
         * HandlerBase
         * 处理器基类
         * Type-erased base so that all handler types can be stored in a single map.
         * 类型擦除的基类，使所有处理器类型都能存储在同一个 map 中。
         */
        struct HandlerBase {
            virtual ~HandlerBase() = default;
        };

        template<typename E>
        struct Handler : HandlerBase {
            std::function<void(const E &)> callback;

            explicit Handler(std::function<void(const E &)> handler) : callback(std::move(handler)) {
            }
        };

        std::unordered_map<uint64_t, std::shared_ptr<HandlerBase> > entries_;
        std::unordered_map<std::type_index, std::vector<uint64_t> > byType_;
        std::mutex mutex_;
        uint64_t nextId_ = 1;

    public:
        /**
         * Default constructor
         * 默认构造函数
         */
        EventBus() = default;

        /**
         * Copy constructor (deleted)
         * 拷贝构造函数（已删除）
         * The event bus is non-copyable because it owns a mutex and handler table.
         * 事件总线不可拷贝，因为它持有互斥锁与处理器表。
         */
        EventBus(const EventBus &) = delete;

        /**
         * Copy assignment (deleted)
         * 拷贝赋值运算符（已删除）
         * The event bus is non-copyable because it owns a mutex and handler table.
         * 事件总线不可拷贝，因为它持有互斥锁与处理器表。
         */
        EventBus &operator=(const EventBus &) = delete;

        /**
         * Subscribe
         * 订阅
         * @tparam E event type 事件类型
         * @param handler handler 事件处理器
         * @return A RAII handle that auto-unsubscribes on destruction. 析构时自动退订的 RAII 句柄。
         */
        template<typename E>
        Subscription Subscribe(std::function<void(const E &)> handler) {
            std::shared_ptr<HandlerBase> entry = std::make_shared<Handler<E> >(std::move(handler));
            std::lock_guard lock(mutex_);
            const uint64_t id = nextId_++;
            entries_.emplace(id, std::move(entry));
            byType_[std::type_index(typeid(E))].push_back(id);
            return {this, id};
        }

        /**
         * Unsubscribe
         * 退订
         * @param id id The subscription id returned by Subscribe. Subscribe 返回的订阅 id。
         */
        void Unsubscribe(uint64_t id);

        /**
         * Publish
         * 发布
         * Deliver an event to all current subscribers of type E, synchronously.
         * 同步地将事件投递给类型 E 的所有当前订阅者。
         * @tparam E event type 事件类型
         * @param event event 事件
         */
        template<typename E>
        void Publish(const E &event) {
            std::vector<std::shared_ptr<HandlerBase> > snapshot;
            {
                std::lock_guard lock(mutex_);
                const auto it = byType_.find(std::type_index(typeid(E)));
                if (it == byType_.end()) {
                    return;
                }
                for (const uint64_t id: it->second) {
                    if (const auto entryIt = entries_.find(id); entryIt != entries_.end()) {
                        snapshot.push_back(entryIt->second);
                    }
                }
            }
            for (const auto &entry: snapshot) {
                static_cast<Handler<E> *>(entry.get())->callback(event);
            }
        }
    };
}
