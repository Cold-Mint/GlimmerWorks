//
// Created by Cold-Mint on 2026/1/6.
//

#ifndef GLIMMERWORKS_SAVESMANAGER_H
#define GLIMMERWORKS_SAVESMANAGER_H
#include <vector>

#include "Saves.h"

namespace glimmer {
    class SavesManager {
        VirtualFileSystem *virtualFileSystem_;
        std::vector<std::unique_ptr<Saves> > saveList_;
        std::vector<std::unique_ptr<MapManifest> > manifestList_;

        void AddSaves(std::unique_ptr<Saves> saves);

    public:
        explicit SavesManager(VirtualFileSystem *virtualFileSystem);

        /**
         * Retrieve the archive at the specified location.
         * 获取指定位置的存档。
         * @param index
         * @return
         */
        [[nodiscard]] Saves *GetSave(size_t index) const;

        [[nodiscard]] MapManifest *GetMapManifest(size_t index) const;

        /**
         * Delete the specified archive
         * 删除指定存档
         * @param index
         * @return
         */
        bool DeleteSave(size_t index);


        /**
        * Create a saves
        * 创建存档
        * @param manifest manifest 清单文件
        */
        Saves *Create(MapManifest &manifest);


        /**
         * Load all the saved files.
         * 加载所有的存档。
         */
        void LoadAllSaves();

        /**
         * Find out how many archives there are.
         * 获取有多少个存档。
         * @return
         */
        [[nodiscard]] size_t GetSavesListSize() const;
    };
}

#endif //GLIMMERWORKS_SAVESMANAGER_H
