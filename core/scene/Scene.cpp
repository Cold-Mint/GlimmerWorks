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
#include "Scene.h"

#include "core/context/AppContext.h"
#include "core/log/LogCat.h"
#include "RmlUi/Core/Context.h"
#include "RmlUi/Core/DataModelHandle.h"

void glimmer::Scene::OnFrameStart()
{
    // Intentionally empty default implementation for base class
}

void glimmer::Scene::OnPauseScene()
{
    HideAllElementDocuments();
}

void glimmer::Scene::OnCreateDataModels()
{
    //Create the RML data model here
}

void glimmer::Scene::LoadDocuments()
{
    //Load the rml document within this method.
}

Rml::ElementDocument* glimmer::Scene::LoadDocumentByName(const Rml::String& name, const ResourceRef* resourceRef)
{
    auto existingDoc = GetDocument(name);
    if (existingDoc != nullptr)
    {
        return existingDoc;
    }
    Rml::ElementDocument* document = LoadSingleDocument(resourceRef);
    if (document != nullptr)
    {
        RegisterDocument(name, document);
    }
    return document;
}

void glimmer::Scene::OnResumeScene()
{
    ShowAllElementDocuments();
}

void glimmer::Scene::OnConfigChanged(const Config* config)
{
    // Intentionally empty default implementation for base class
}

bool glimmer::Scene::OnBackPressed()
{
    return false;
}

void glimmer::Scene::OnWindowClose()
{
    // Intentionally empty default implementation for base class
}

void glimmer::Scene::OnWindowSizeChanged(const int& width, const int& height)
{
    // Intentionally empty default implementation for base class
}

glimmer::Scene::~Scene()
{
    RemoveAllDataModel();
    CloseAllElementDocuments();
}

glimmer::AppContext* glimmer::Scene::GetAppContext() const
{
    return appContext_;
}

void glimmer::Scene::Init()
{
    LogCat::i("Scene init started");
    initSubclassFinish_ = true;
    const AppContext* appContext = GetAppContext();
    if (appContext == nullptr)
    {
        LogCat::w(std::source_location::current(), "Scene init failed: appContext is nullptr");
        return;
    }
    const WindowContext* windowContext = appContext->GetWindowContext();
    if (windowContext == nullptr)
    {
        LogCat::w(std::source_location::current(), "Scene init failed: windowContext is nullptr");
        return;
    }
    OnWindowSizeChanged(windowContext->GetWindowWidth(), windowContext->GetWindowHeight());
    if (const Config* config = appContext->GetConfig(); config != nullptr)
    {
        OnConfigChanged(config);
    }
    rmlContext_ = appContext->GetRmlContext();
    OnCreateDataModels();
    LoadDocuments();
    LogCat::i("Scene init completed");
}

Rml::ElementDocument* glimmer::Scene::LoadSingleDocument(const ResourceRef* resourceRef)
{
    if (appContext_ == nullptr || rmlContext_ == nullptr || resourceRef == nullptr)
    {
        LogCat::w(std::source_location::current(),
                  "The required variables for LoadDocument are missing. It is necessary to check if they are called after the init method.");
        return nullptr;
    }
    Rml::ElementDocument* elementDocument = rmlContext_->LoadDocument(appContext_, resourceRef);
    if (elementDocument == nullptr)
    {
        LogCat::w(std::source_location::current(), "elementDocument_ == nullptr");
        return nullptr;
    }
#if  !defined(NDEBUG)
    if (elementDocumentSet_.contains(elementDocument))
    {
        LogCat::e(std::source_location::current(), "A duplicate loading of the document has been detected.");
        return nullptr;
    }
#endif
    elementDocumentSet_.insert(elementDocument);
    return elementDocument;
}

Rml::DataModelConstructor* glimmer::Scene::CreateDataModel(const Rml::String& name)
{
    if (rmlContext_ == nullptr)
    {
        return nullptr;
    }
    Rml::Context* rmlContextCore = rmlContext_->GetRmlContext();
    if (rmlContextCore == nullptr)
    {
        return nullptr;
    }
    if (rmlConstructorNames_.contains(name))
    {
        LogCat::w(std::source_location::current(), "Recreate the dataModel:", name);
        return nullptr;
    }
    rmlConstructors_.push_back(rmlContextCore->CreateDataModel(name));
    rmlConstructorNames_.insert(name);
    return &rmlConstructors_.back();
}

void glimmer::Scene::RemoveAllDataModel()
{
    if (rmlContext_ == nullptr)
    {
        LogCat::w(std::source_location::current(), "RemoveAllDataModel rmlContext_ == nullptr");
        return;
    }
    Rml::Context* rmlContextCore = rmlContext_->GetRmlContext();
    if (rmlContextCore == nullptr)
    {
        LogCat::w(std::source_location::current(), "RemoveAllDataModel rmlContextCore == nullptr");
        return;
    }
    rmlConstructors_.clear();
    for (auto& rmlConstructorName : rmlConstructorNames_)
    {
        rmlContextCore->RemoveDataModel(rmlConstructorName);
    }
    rmlConstructorNames_.clear();
}

void glimmer::Scene::RegisterDocument(const Rml::String& name, Rml::ElementDocument* document)
{
    if (document == nullptr)
    {
        LogCat::w(std::source_location::current(), "RegisterDocument called with nullptr document");
        return;
    }
    namedDocuments_[name] = document;
}

Rml::ElementDocument* glimmer::Scene::GetDocument(const Rml::String& name) const
{
    auto it = namedDocuments_.find(name);
    return it != namedDocuments_.end() ? it->second : nullptr;
}

void glimmer::Scene::HideDocument(const Rml::String& name) const
{
    Rml::ElementDocument* document = GetDocument(name);
    if (document != nullptr)
    {
        document->Hide();
    }
}

void glimmer::Scene::ShowDocument(const Rml::String& name) const
{
    Rml::ElementDocument* document = GetDocument(name);
    if (document != nullptr)
    {
        document->Show();
    }
}

std::vector<Rml::String> glimmer::Scene::GetDocumentNames() const
{
    std::vector<Rml::String> names;
    for (const auto& pair : namedDocuments_)
    {
        names.push_back(pair.first);
    }
    return names;
}

Rml::ElementDocument* glimmer::Scene::GetDocumentPublic(const Rml::String& name) const
{
    return GetDocument(name);
}

std::vector<Rml::ElementDocument*> glimmer::Scene::GetAllDocuments() const
{
    std::vector<Rml::ElementDocument*> docs;
    for (auto doc : elementDocumentSet_)
    {
        if (doc != nullptr)
        {
            docs.push_back(doc);
        }
    }
    return docs;
}

Rml::Element* glimmer::Scene::FindElementById(const Rml::String& elementId) const
{
    for (Rml::ElementDocument* doc : elementDocumentSet_)
    {
        if (doc != nullptr)
        {
            Rml::Element* element = doc->GetElementById(elementId.c_str());
            if (element != nullptr)
            {
                return element;
            }
        }
    }
    return nullptr;
}

void FindElementRecursively(Rml::Element* parent, const Rml::String& attrName, const Rml::String& attrValue, Rml::Element*& result)
{
    if (parent == nullptr || result != nullptr)
    {
        return;
    }
    Rml::String value = parent->GetAttribute<Rml::String>(attrName.c_str(), "");
    if (value == attrValue)
    {
        result = parent;
        return;
    }
    int childCount = parent->GetNumChildren();
    for (int i = 0; i < childCount; ++i)
    {
        Rml::Element* child = parent->GetChild(i);
        FindElementRecursively(child, attrName, attrValue, result);
        if (result != nullptr)
        {
            return;
        }
    }
}

void FindElementByTextRecursively(Rml::Element* parent, const Rml::String& text, Rml::Element*& result)
{
    if (parent == nullptr || result != nullptr)
    {
        return;
    }
    Rml::String innerRml = parent->GetInnerRML();
    if (innerRml.find(text) != Rml::String::npos)
    {
        result = parent;
        return;
    }
    int childCount = parent->GetNumChildren();
    for (int i = 0; i < childCount; ++i)
    {
        Rml::Element* child = parent->GetChild(i);
        FindElementByTextRecursively(child, text, result);
        if (result != nullptr)
        {
            return;
        }
    }
}

Rml::Element* glimmer::Scene::FindElementByAttribute(const Rml::String& attrName, const Rml::String& attrValue) const
{
    for (Rml::ElementDocument* doc : elementDocumentSet_)
    {
        if (doc != nullptr)
        {
            Rml::Element* result = nullptr;
            FindElementRecursively(doc, attrName, attrValue, result);
            if (result != nullptr)
            {
                return result;
            }
        }
    }
    return nullptr;
}

Rml::Element* glimmer::Scene::FindElementByText(const Rml::String& text) const
{
    for (Rml::ElementDocument* doc : elementDocumentSet_)
    {
        if (doc != nullptr)
        {
            Rml::Element* result = nullptr;
            FindElementByTextRecursively(doc, text, result);
            if (result != nullptr)
            {
                return result;
            }
        }
    }
    return nullptr;
}

void glimmer::Scene::HideAllElementDocuments() const
{
    for (auto elementDocument : elementDocumentSet_)
    {
        if (elementDocument == nullptr)
        {
            continue;
        }
        elementDocument->Hide();
    }
}

void glimmer::Scene::ShowAllElementDocuments() const
{
    for (auto elementDocument : elementDocumentSet_)
    {
        if (elementDocument == nullptr)
        {
            continue;
        }
        elementDocument->Show();
    }
}

void glimmer::Scene::CloseAllElementDocuments()
{
    for (auto elementDocument : elementDocumentSet_)
    {
        if (elementDocument == nullptr || rmlContext_ == nullptr)
        {
            continue;
        }
        rmlContext_->CloseDocument(elementDocument);
    }
    elementDocumentSet_.clear();
    namedDocuments_.clear();
}

bool glimmer::Scene::HandleEvent(const SDL_Event& event)
{
    return false;
}

void glimmer::Scene::Update(float delta)
{
#if  !defined(NDEBUG)
    if (!initSubclassFinish_)
    {
        initTimeOut_ += delta;
        if (initTimeOut_ > 2)
        {
            assert(false);
        }
    }
#endif
}

void glimmer::Scene::Render(SDL_Renderer* renderer)
{
    // Intentionally empty default implementation for base class
}

glimmer::Scene::Scene(AppContext* context) : appContext_(context)
{
}
