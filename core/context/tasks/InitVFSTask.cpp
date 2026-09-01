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
#include "InitVFSTask.h"

#include "core/context/ISystemBucket.h"
#include "core/log/LogCat.h"
#include "core/vfs/StdFileProvider.h"
#include "core/vfs/VirtualFileSystem.h"

bool glimmer::InitVFSTask::Run(ISystemBucket *systemBucket) {
    auto virtualFileSystem = std::make_unique<VirtualFileSystem>();
#ifdef __ANDROID__
    auto env = static_cast<JNIEnv *>(SDL_GetAndroidJNIEnv());
    if (env == nullptr) {
        initSuccess_ = false;
        LogCat::e(std::source_location::current(), "env is nullptr");
        return;
    }
    auto activity = static_cast<jobject>(SDL_GetAndroidActivity());
    if (activity == nullptr) {
        initSuccess_ = false;
        LogCat::e(std::source_location::current(), "activity is nullptr");
        return;
    }
    jclass activityClass = env->GetObjectClass(activity);
    if (activityClass == nullptr) {
        LogCat::e(std::source_location::current(), "activityClass is nullptr");
        return;
    }
    jmethodID getAssetsMethod = env->
            GetMethodID(activityClass, "getAssets", "()Landroid/content/res/AssetManager;");
    if (getAssetsMethod == nullptr) {
        LogCat::e(std::source_location::current(), "getAssetsMethod is nullptr");
        return;
    }
    jobject assetManagerJava = env->CallObjectMethod(activity, getAssetsMethod);
    if (assetManagerJava == nullptr) {
        LogCat::e(std::source_location::current(), "assetManagerJava is nullptr");
        return;
    }
    AAssetManager *assetManager = AAssetManager_fromJava(env, assetManagerJava);
    if (assetManager == nullptr) {
        LogCat::e(std::source_location::current(), "assetManager is nullptr");
        return;
    }
    auto assetsProvider = std::make_unique<AndroidAssetsFileProvider>(assetManager);
    std::optional<std::string> indexTomlOptional = assetsProvider->ReadFile("index.toml");
    if (!indexTomlOptional.has_tomlValue()) {
        LogCat::e(std::source_location::current(), "assetManager is nullptr");
        return;
    }
    const toml::tomlValue tomlValue = toml::parse_str(indexTomlOptional.tomlValue(), tomlVersion_);
    auto assetsEntry = toml::get<std::vector<AndroidAssetEntry> >(tomltomlValue);
    assetsProvider->SetAssetEntryData(assetsEntry);
    jmethodID getDataDirMethod = env->GetMethodID(activityClass, "getFilesDir", "()Ljava/io/File;");
    if (getDataDirMethod == nullptr) {
        LogCat::e(std::source_location::current(), "getDataDirMethod is nullptr");
        return;
    }
    jobject dataDirFile = env->CallObjectMethod(activity, getDataDirMethod);
    if (dataDirFile == nullptr) {
        LogCat::e(std::source_location::current(), "dataDirFile is nullptr");
        return;
    }
    jclass fileClass = env->GetObjectClass(dataDirFile);
    if (fileClass == nullptr) {
        LogCat::e(std::source_location::current(), "fileClass is nullptr");
        return;
    }
    jmethodID getAbsolutePathMethod = env->GetMethodID(fileClass, "getAbsolutePath", "()Ljava/lang/String;");
    if (getAbsolutePathMethod == nullptr) {
        LogCat::e(std::source_location::current(), "getAbsolutePathMethod is nullptr");
        return;
    }
    auto absolutePathJStr = static_cast<jstring>(env->CallObjectMethod(dataDirFile, getAbsolutePathMethod));
    if (!absolutePathJStr) {
        LogCat::e(std::source_location::current(), "absolutePathJStr is nullptr");
        return;
    }
    const char *absolutePathCStr = env->GetStringUTFChars(absolutePathJStr, nullptr);
    if (!absolutePathCStr) {
        LogCat::e(std::source_location::current(), "absolutePathCStr is nullptr");
        return;
    }
    std::string dataDirPath(absolutePathCStr);
    env->ReleaseStringUTFChars(absolutePathJStr, absolutePathCStr);
    virtualFileSystem->Mount(std::make_unique<StdFileProvider>(dataDirPath + "/assets"));
    virtualFileSystem->Mount(std::unique_ptr<IFileProvider>(std::move(assetsProvider)));
#else
    virtualFileSystem->Mount(
        std::make_unique<StdFileProvider>(std::filesystem::current_path().string()));
#endif
    systemBucket->SetVirtualFileSystem(std::move(virtualFileSystem));
    return true;
}

void glimmer::InitVFSTask::Rollback(ISystemBucket *systemBucket) {
    systemBucket->SetVirtualFileSystem(nullptr);
}
