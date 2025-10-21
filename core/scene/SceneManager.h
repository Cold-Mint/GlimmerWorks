//
// Created by Cold-Mint on 2025/10/11.
//

#ifndef GLIMMERWORKS_SCENEMANAGER_H
#define GLIMMERWORKS_SCENEMANAGER_H
#include "Scene.h"

namespace Glimmer {
    /**
     * Scene Manager
     * 场景管理器
     */
    class SceneManager {
        Scene *scene = nullptr;
        Scene *consoleScene = nullptr;
        Scene *pendingScene = nullptr;
        /**
         * Are there any scenarios that need to be mounted
         * 是否有需要挂载的场景
         */
        bool hasPending = false;

    public:
        /**
         * Set the console scene
         * 设置控制台场景
         * The console scene provides an input box that allows players to enter commands to debug and modify the game.
         * 控制台场景提供了一个输入框，允许玩家输入命令来调试和修改游戏。
         * This scene is globally available. By default, press the ~ key to open.
         * 这个场景时全局可用的。默认按下～键打开。
         * @param sc
         */
        void setConsoleScene(Scene *sc);

        /**
         * Change the scene.
         * 改变场景。
         * The set scene will enter the pending mount state. It will be automatically mounted in the next frame.
         * 设置的场景会进入待挂载状态。会在下一帧自动挂载。
         * @param sc New scene 新场景
         */
        void changeScene(Scene *sc);

        /**
         * Apply the pending scene.
         * 应用待挂场景。
         */
        void applyPendingScene();

        /**
         * Get the console scene
         * 获取控制台场景
         * @return scene 场景
         */
        [[nodiscard]] Scene *getConsoleScene() const;

        /**
         * Get the current scene
         * 获取当前的场景
         * @return Scene 场景
         */
        [[nodiscard]] Scene *getScene() const;
    };
}


#endif //GLIMMERWORKS_SCENEMANAGER_H
