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
#include <memory>
#include <stack>
#include <vector>

#include "Scene.h"

namespace glimmer {
    /**
     * Scene Manager
     * 场景管理器
     */
    class SceneManager {
        std::stack<std::unique_ptr<Scene> > sceneStack_;
        std::vector<std::unique_ptr<Scene> > overlayScenes_;
        std::vector<Scene *> overlayScenesPtr_;
        /**
         * Are there any scenarios that need to be mounted
         * 是否有需要挂载的场景
         */
        bool hasPending_ = false;

    public:
        void ClearScenes();

        /**
         * add overlay scene
         * 添加叠加层
         * @param overlay 场景 scene
         */
        void AddOverlayScene(std::unique_ptr<Scene> overlay);

        /**
         * remove overlay scene
         * 移除叠加层
         * @param overlay 场景 scene
         */
        void RemoveOverlayScene(const Scene *overlay);


        /**
         * get overlay scenes
         * 获取叠加层
         * @return overlayScenes 所有叠加层
         */
        [[nodiscard]] std::vector<Scene *> GetOverlayScenes() const;

        /**
         * PushScene
         * 压入场景
         * @param scene
         */
        void PushScene(std::unique_ptr<Scene> scene);

        /**
         * Replace the scene
         * 替换场景
         * @param scene
         */
        void ReplaceScene(std::unique_ptr<Scene> scene);

        /**
         * PopScene
         * 弹出场景
         */
        void PopScene();

        /**
         * The scene of obtaining the top of the stack
         * 获取栈顶的场景
         * @return Scene 场景
         */
        [[nodiscard]] Scene *GetTopScene() const;


        /**
         * Get how many scenes there are
         * 获取有多少个场景
         * @return
         */
        [[nodiscard]] size_t GetSceneCount() const;
    };
}
