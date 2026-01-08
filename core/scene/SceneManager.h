//
// Created by Cold-Mint on 2025/10/11.
//

#ifndef GLIMMERWORKS_SCENEMANAGER_H
#define GLIMMERWORKS_SCENEMANAGER_H
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
        bool hasPending = false;

    public:
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
        [[nodiscard]] const std::vector<Scene *> &GetOverlayScenes() const;

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


#endif //GLIMMERWORKS_SCENEMANAGER_H
