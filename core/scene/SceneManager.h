//
// Created by Cold-Mint on 2025/10/11.
//

#ifndef GLIMMERWORKS_SCENEMANAGER_H
#define GLIMMERWORKS_SCENEMANAGER_H
#include <vector>

#include "Scene.h"

namespace glimmer {
    /**
     * Scene Manager
     * 场景管理器
     */
    class SceneManager {
        Scene *scene = nullptr;
        std::vector<Scene *> overlayScenes;
        Scene *pendingScene = nullptr;
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
        void AddOverlayScene(Scene *overlay);

        /**
         * remove overlay scene
         * 移除叠加层
         * @param overlay 场景 scene
         */
        void RemoveOverlayScene(Scene *overlay);


        /**
         * get overlay scenes
         * 获取叠加层
         * @return overlayScenes 所有叠加层
         */
        [[nodiscard]] const std::vector<Scene *> &GetOverlayScenes() const;

        /**
         * Change the scene.
         * 改变场景。
         * The set scene will enter the pending mount state. It will be automatically mounted in the next frame.
         * 设置的场景会进入待挂载状态。会在下一帧自动挂载。
         * @param sc New scene 新场景
         */
        void ChangeScene(Scene *sc);

        /**
         * Apply the pending scene.
         * 应用待挂场景。
         */
        void ApplyPendingScene();


        /**
         * Get the current scene
         * 获取当前的场景
         * @return Scene 场景
         */
        [[nodiscard]] Scene *getScene() const;
    };
}


#endif //GLIMMERWORKS_SCENEMANAGER_H
