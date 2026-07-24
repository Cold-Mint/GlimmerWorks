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
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_render.h>

#include "core/Config.h"
#include "core/context/RmlContext.h"
#include "core/ecs/IDocumentRegistry.h"
#include "RmlUi/Core/DataModelHandle.h"


namespace glimmer
{
    class AppContext;

    class Scene : public IDocumentRegistry
    {
        bool initSubclassFinish_ = false;
        AppContext* appContext_ = nullptr;
        RmlContext* rmlContext_ = nullptr;
        std::unordered_set<Rml::ElementDocument*> elementDocumentSet_;
        std::unordered_set<Rml::ElementDocument*> visibleElementDocumentsSnapshot_;
        std::vector<Rml::DataModelConstructor> rmlConstructors_;
        std::unordered_set<Rml::String> rmlConstructorNames_;
#if  !defined(NDEBUG)
        float initTimeOut_ = 0.0F;
#endif

        void RemoveAllDataModel();

    protected:
        [[nodiscard]] AppContext* GetAppContext() const;

        /**
       * It is called after the subclass is fully constructed.
       * 在子类完全构造后调用。
       */
        void Init();

        /**
         * Record and hide all the documents.
         * 记录并隐藏所有的文档。
         */
        void HideAllElementDocuments();

        void RestoreHiddenElementDocuments();

        void CloseAllElementDocuments();

    public:
        [[nodiscard]] std::vector<Rml::ElementDocument*> GetAllDocuments() const;

        Rml::ElementDocument* LoadSingleDocument(const ResourceRef* resourceRef) override;

        Rml::DataModelConstructor* CreateDataModel(const Rml::String& name) override;

        Rml::Element* FindElementById(const Rml::String& elementId) const;

        Rml::Element* FindElementByAttribute(const Rml::String& attrName, const Rml::String& attrValue) const;

        Rml::Element* FindElementByText(const Rml::String& text) const;

        /**
         * Processing events (input, window messages, etc.) returns whether to intercept the event. If it is true, it will not be passed down.
         * 处理事件（输入、窗口消息等）返回是否拦截事件，如果为true则不会向下传递。
         * @param event
         * @return
         */
        virtual bool HandleEvent(const SDL_Event& event);

        /**
         * Call this before the Render function. It is recommended to calculate the view data within this function.
         * 在Render函数前调用，建议在此函数内计算视图数据。
         * @param delta
         */
        virtual void Update(float delta);

        /**
         * Render
         * 渲染
         * @param renderer
         */
        virtual void Render(SDL_Renderer* renderer);

        /**
         * Called when the frame begins
         * 当帧开始时调用
         */
        virtual void OnFrameStart();

        /**
         * This method will be called when the scene is superimposed over the current scene.
         * 当场景叠加在当前场景之上时，将调用此方法。
         * The Overlay scene display does not trigger this method.
         * Overlay场景显示不会触发此方法。
         */
        virtual void OnPauseScene();

        virtual void OnCreateDataModels();

        virtual void LoadDocuments();

        /**
         * When a scene above the current scene is popped up, the current scene is displayed.
         * 当在场景之上的场景被弹出后，当前场景显示出来。
         * The overlay scene hiding does not trigger this method.
         * Overlay场景隐藏不会触发此方法。
         */
        virtual void OnResumeScene();

        /**
         * When the configuration changes
         * 当配置发生改变时
         * @param config
         */
        virtual void OnConfigChanged(const Config* config);

        /**
         * This method is called when the back key is pressed. On Android systems, navigate up; on desktop platforms, press ESC.
         * 当返回键被按下时调用这个方法，安卓系统向上导航，桌面平台按下ESC。
         *
         * @return If true is returned, inform the framework scenario that it has handled the returned event by itself. 如果返回true，告诉框架场景自行处理了返回事件。
         */
        virtual bool OnBackPressed();

        virtual void OnWindowClose();

        virtual void OnWindowSizeChanged(const int& width, const int& height);


        ~Scene() override;

        explicit Scene(AppContext* context);
    };
}
