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

namespace glimmer {
    class EventBus;

    /**
   * Subscription
   * 订阅句柄
   * RAII handle returned by Subscribe. When it is destroyed the
   * subscription is automatically removed.
   * Subscribe 返回的 RAII 句柄。析构时自动退订。
   */
    class Subscription {
    public:
        /**
         * Default constructor
         * 默认构造函数
         * Creates an invalid (empty) subscription that owns nothing.
         * 创建一个无效（空）的订阅，不拥有任何东西。
         */
        Subscription() = default;

        /**
         * Constructor
         * 构造函数
         * Creates a subscription that owns the handler identified by id on bus.
         * 创建一个订阅，它拥有 bus 上由 id 标识的处理器。
         * @param bus bus The event bus this subscription belongs to. 此订阅所属的事件总线。
         * @param id id The subscription id. 订阅 id。
         */
        Subscription(EventBus *bus, uint64_t id) : bus_(bus), id_(id) {
        }

        /**
         * Move constructor
         * 移动构造函数
         * Transfers ownership from other, leaving other invalid.
         * 从 other 转移所有权，并使 other 失效。
         * @param other other The subscription to move from. 要移动的订阅。
         */
        Subscription(Subscription &&other) noexcept : bus_(other.bus_), id_(other.id_) {
            other.bus_ = nullptr;
            other.id_ = 0;
        }

        /**
         * Copy constructor (deleted)
         * 拷贝构造函数（已删除）
         * Subscriptions are move-only and cannot be copied.
         * 订阅是仅移动的，不可拷贝。
         */
        Subscription(const Subscription &) = delete;

        /**
         * Copy assignment (deleted)
         * 拷贝赋值运算符（已删除）
         * Subscriptions are move-only and cannot be copied.
         * 订阅是仅移动的，不可拷贝。
         */
        Subscription &operator=(const Subscription &) = delete;

        /**
         * Move assignment
         * 移动赋值运算符
         * Unsubscribes the current handler (if any) and takes ownership of other.
         * 退订当前的处理器（如果有），并接管 other 的所有权。
         * @param other other The subscription to move from. 要移动的订阅。
         * @return A reference to this subscription. 指向本订阅的引用。
         */
        Subscription &operator=(Subscription &&other) noexcept;

        /**
         * Destructor
         * 析构函数
         * Automatically unsubscribes the owned handler.
         * 自动退订所拥有的处理器。
         */
        ~Subscription();

        /**
         * Reset
         * 重置
         * Unsubscribe now and invalidate the handle.
         * 立即退订并使句柄失效。
         */
        void Reset();

        [[nodiscard]] bool IsValid() const;

    private:
        EventBus *bus_ = nullptr;
        uint64_t id_ = 0;
    };
}
