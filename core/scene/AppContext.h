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
#include <future>
#include <queue>
#include <string>

#include "SceneManager.h"
#include "core/LangsResources.h"
#include "core/mod/dataPack/BiomesManager.h"
#include "core/mod/dataPack/ItemManager.h"
#include "core/mod/dataPack/TileResourceManager.h"
#include "core/mod/resourcePack/ResourcePackManager.h"
#include "core/contributor/ContributorManager.h"
#include "core/CommandHookManager.h"
#include "core/mod/ResourceLocator.h"
#include "core/mod/dataPack/DataPackManager.h"
#include "core/mod/dataPack/StringManager.h"
#include "core/Config.h"
#include "core/GameUIMessage.h"
#include "core/console/CommandHistoryManager.h"
#include "core/console/CommandManager.h"
#include "core/console/ConsoleWorker.h"
#include "core/inventory/InitialInventoryManager.h"
#include "core/lootTable/LootTableManager.h"
#include "core/mod/StructurePlacementConditionsManager.h"
#include "core/mod/TomlTemplateExpander.h"
#include "core/mod/dataPack/BiomeDecoratorResourcesManager.h"
#include "core/mod/dataPack/FixedColorManager.h"
#include "core/mod/dataPack/LightMaskManager.h"
#include "core/mod/dataPack/LightSourceManager.h"
#include "core/mod/dataPack/MobManager.h"
#include "core/mod/dataPack/RecipeManager.h"
#include "core/mod/dataPack/StructureManager.h"
#include "core/mod/resourcePack/AudioManager.h"
#include "core/shape/ShapeManager.h"
#include "core/world/PreloadColors.h"
#include "core/world/generator/BiomeDecoratorManager.h"
#include "core/world/structure/StructureGeneratorManager.h"


namespace glimmer
{
    class SavesManager;
    class ResourceLocator;
    class DynamicSuggestionsManager;
    class StringManager;
    class Config;
    class ResourcePackManager;
    class WorldContext;
    class DragAndDrop;
    /**
     * @class AppContext
     * @brief 应用上下文，管理共享资源和依赖。
     *
     * Application context that holds and provides shared resources
     * and dependencies to different modules.
     *
     */
    class AppContext
    {
        SDL_Window* window_ = nullptr;
        SDL_Renderer* renderer_ = nullptr;
        std::string language_;
        std::unique_ptr<DataPackManager> dataPackManager_;
        std::unique_ptr<Config> config_;
        std::unique_ptr<toml::value> configValue;
        std::unique_ptr<SceneManager> sceneManager_;
        std::unique_ptr<StringManager> stringManager_;
        std::unique_ptr<TileResourceManager> tileResourceManager_;
        std::unique_ptr<BiomesManager> biomesManager_;
        std::unique_ptr<CommandManager> commandManager_;
        std::unique_ptr<SavesManager> savesManager_;
        std::unique_ptr<ConsoleWorker> consoleWorker_;
        std::unique_ptr<ResourcePackManager> resourcePackManager_;
        std::unique_ptr<LangsResources> langsResources_;
        std::unique_ptr<DynamicSuggestionsManager> dynamicSuggestionsManager_;
        std::unique_ptr<VirtualFileSystem> virtualFileSystem_;
        std::unique_ptr<BiomeDecoratorManager> biomeDecoratorManager_;
        std::unique_ptr<BiomeDecoratorResourcesManager> biomeDecoratorResourcesManager_;
        std::unique_ptr<LootTableManager> lootTableManager_;
        std::unique_ptr<StructureGeneratorManager> structureGeneratorManager_;
        std::unique_ptr<StructureManager> structureManager_;
        std::unique_ptr<ResourceLocator> resourceLocator_;
        std::unique_ptr<ItemManager> itemManager_;
        std::unique_ptr<InitialInventoryManager> initialInventoryManager_;
        std::unique_ptr<StructurePlacementConditionsManager> structurePlacementConditionsManager_;
        std::unique_ptr<PreloadColors> preloadColors_;
        std::unique_ptr<ContributorManager> contributorManager_;
        std::unique_ptr<MobManager> mobManager_;
        std::unique_ptr<ShapeManager> shapeManager_;
        std::unique_ptr<AudioManager> audioManager_;
        std::unique_ptr<LightMaskManager> lightMaskManager_;
        std::unique_ptr<LightSourceManager> lightSourceManager_;
        std::unique_ptr<FixedColorManager> fixedColorManager_;
        std::unique_ptr<TomlTemplateExpander> tomlTemplateExpander_;
        std::unique_ptr<CommandHookManager> commandHookManager_;
        std::shared_ptr<AudioResourceResult> mainMenuBGM_;
        std::shared_ptr<CommandHistoryManager> commandHistoryManager_;
        std::unique_ptr<RecipeManager> recipeManager_;
        std::mutex mainThreadMutex_;
        std::queue<std::function<void()>> mainThreadTasks_;
        bool isRunning = true;
        std::thread::id mainThreadId_;
        toml::spec tomlVersion_ = toml::spec::v(1, 1, 0);
        std::vector<GameUIMessage> gameUIMessages_;
        bool initSuccess_ = false;
        int windowWidth_ = 0;
        int windowHeight_ = 0;
        void LoadLanguage(const std::string& data) const;

        static std::string GetTimeFileName(const std::string& prefix = "screenshot", const std::string& ext = ".png");

    public:
        AppContext();

        void LoadMainMenuBGM();

        void PlayMainMenuBGM() const;

        [[nodiscard]] const toml::spec& GetTomlVersion() const;

        ~AppContext();

        [[nodiscard]] bool InitSuccess() const;

        void SetWindowWidth(int width);

        void SetWindowHeight(int height);

        void SetWindowTitle(const char* title) const;

        [[nodiscard]] int GetWindowWidth() const;

        [[nodiscard]] int GetWindowHeight() const;

        void SetWindow(SDL_Window* window);

        void SetRenderer(SDL_Renderer* renderer);

        void CreateScreenshot(const std::function<void(const std::string& text)>* onMessage) const;

        [[nodiscard]] bool Running() const;


        void AddUIMessage(const std::string& string);

        std::vector<GameUIMessage>& GetGameUIMessages();

        void ExitApp();


        [[nodiscard]] PreloadColors* GetPreloadColors() const;

        [[nodiscard]] Config* GetConfig() const;

        [[nodiscard]] InitialInventoryManager* GetInitialInventoryManager() const;

        [[nodiscard]] CommandManager* GetCommandManager() const;

        [[nodiscard]] AudioManager* GetAudioManager() const;

        [[nodiscard]] ConsoleWorker* GetConsoleWorker() const;

        [[nodiscard]] LightMaskManager* GetLightMaskManager() const;

        [[nodiscard]] LightSourceManager* GetLightSourceManager() const;

        [[nodiscard]] FixedColorManager* GetFixedColorManager() const;

        [[nodiscard]] RecipeManager* GetRecipeManager() const;

        [[nodiscard]] StringManager* GetStringManager() const;

        [[nodiscard]] CommandHookManager* GetCommandHookManager() const;

        [[nodiscard]] BiomeDecoratorResourcesManager* GetBiomeDecoratorResourcesManager() const;

        [[nodiscard]] TomlTemplateExpander* GetTomlTemplateExpander() const;

        [[nodiscard]] DynamicSuggestionsManager* GetDynamicSuggestionsManager() const;

        [[nodiscard]] const std::string& GetLanguage();

        [[nodiscard]] CommandHistoryManager* GetCommandHistoryManager() const;

        [[nodiscard]] LangsResources* GetLangsResources() const;

        [[nodiscard]] ResourcePackManager* GetResourcePackManager() const;

        [[nodiscard]] StructurePlacementConditionsManager* GetStructurePlacementConditionsManager() const;

        [[nodiscard]] TileResourceManager* GetTileResourceManager() const;

        [[nodiscard]] DataPackManager* GetDataPackManager() const;

        [[nodiscard]] LootTableManager* GetLootTableManager() const;

        [[nodiscard]] ContributorManager* GetContributorManager() const;

        [[nodiscard]] StructureManager* GetStructureManager() const;

        [[nodiscard]] StructureGeneratorManager* GetStructureGeneratorManager() const;

        [[nodiscard]] BiomeDecoratorManager* GetBiomeDecoratorManager() const;

        [[nodiscard]] BiomesManager* GetBiomesManager() const;

        [[nodiscard]] ShapeManager* GetShapeManager() const;

        [[nodiscard]] ResourceLocator* GetResourceLocator() const;

        [[nodiscard]] VirtualFileSystem* GetVirtualFileSystem() const;

        [[nodiscard]] ItemManager* GetItemManager() const;

        [[nodiscard]] SceneManager* GetSceneManager() const;

        void SetRandomSlogan() const;

        [[nodiscard]] SavesManager* GetSavesManager() const;


        [[nodiscard]] MobManager* GetMobManager() const;

        [[nodiscard]] bool IsMainThread() const;

        void ProcessMainThreadTasks();

        /**
         * Execute a function on the main thread. For example: Loading textures must be done in the main thread.
         * 在主线程上执行一个函数。例如：加载纹理，必须在主线程。
         * @tparam Func
         * @param func
         * @return
         */
        template <typename Func>
        std::future<std::invoke_result_t<Func>> AddMainThreadTaskAwait(Func&& func)
        {
            using Result = std::invoke_result_t<Func>;
            if (IsMainThread())
            {
                std::promise<Result> p;
                try
                {
                    if constexpr (std::is_void_v<Result>)
                    {
                        func();
                        p.set_value();
                    }
                    else
                    {
                        p.set_value(func());
                    }
                }
                catch (...)
                {
                    p.set_exception(std::current_exception());
                }
                return p.get_future();
            }
            auto promise = std::make_shared<std::promise<Result>>();
            auto future = promise->get_future();
            {
                std::lock_guard lock(mainThreadMutex_);
                mainThreadTasks_.push(
                    [func = std::forward<Func>(func), promise]() mutable
                    {
                        try
                        {
                            if constexpr (std::is_void_v<Result>)
                            {
                                func();
                                promise->set_value();
                            }
                            else
                            {
                                promise->set_value(func());
                            }
                        }
                        catch (...)
                        {
                            promise->set_exception(std::current_exception());
                        }
                    }
                );
            }

            return future;
        }

        /**
         * Restore the color of the renderer.
         * 恢复渲染器的颜色。
         */
        static void RestoreColorRenderer(SDL_Renderer* sdlRenderer);

        /**
         * RunOnMainThread
         * 在主线程执行
         * @param task
         */
        void RunOnMainThread(std::function<void()> task);

        /**
         * In the next frame, the main thread will execute.
         * 在下一帧主线程执行
         * @param task
         */
        void PostToNextMainFrame(std::function<void()> task);
    };
}
