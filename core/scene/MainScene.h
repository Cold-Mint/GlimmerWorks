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
#include <vector>
#include "Scene.h"
#include "core/Hyperlink.h"
#include "core/utils/StringUtils.h"

namespace glimmer
{
    class MainScene : public Scene
    {
        std::vector<Hyperlink> hyperlinks_{
            {StringUtils::MakeRawText("Github"), "https://github.com/Cold-Mint/GlimmerWorks"},
            {StringUtils::MakeRawText("Discord"), "https://discord.com/invite/CfppC9WHw8"},
            {StringUtils::MakeRawText("itch.io"), "https://cold-mint.itch.io/glimmerworks"},
            {StringUtils::MakeRawText("QQ Channel"), "https://pd.qq.com/s/cntb09fr1?b=9"}
        };
        float uiScale_ = 1.0F;
        int windowWidth_ = 0;
        int windowHeight_ = 0;
        std::string copyright_{StringUtils::MakeRawText(GetCopyrightString())};

        static std::string GetCopyrightString();

        void OnStartGameClick(Rml::DataModelHandle handle, Rml::Event& event, const Rml::VariantList& args);

        void OnExitGameClick(Rml::DataModelHandle handle, Rml::Event& event, const Rml::VariantList& args);

        void OnLinkClick(Rml::DataModelHandle handle, Rml::Event& event, const Rml::VariantList& args);

    public:
        explicit MainScene(AppContext* context);

        void OnConfigChanged(const Config* config) override;

        void OnWindowSizeChanged(const int& width, const int& height) override;

        bool OnBackPressed() override;

        ~MainScene() override;
    };
}
