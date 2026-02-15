//
// Created by coldmint on 2026/2/7.
//

#ifndef GLIMMERWORKS_STRUCTUREINFO_H
#define GLIMMERWORKS_STRUCTUREINFO_H

#include "core/ecs/component/TileLayerComponent.h"
#include "core/mod/ResourceRef.h"

namespace glimmer {
    class StructureInfo {
        uint32_t width_ = 0;
        uint32_t height_ = 0;
        int32_t originX_ = 0;
        int32_t originY_ = 0;
        TileVector2D startPosition_;
        std::vector<ResourceRef> tileData_;
        ResourceRef airRef_;

    public:
        explicit StructureInfo(TileVector2D startPosition);

        [[nodiscard]] uint32_t GetWidth() const;

        [[nodiscard]] uint32_t GetHeight() const;

        /**
         * Set resource reference data (static data)
         * 设置资源引用数据（静态数据）
         * @param width
         * @param tileData
         */
        void SetTileData(uint32_t width, const std::vector<ResourceRef> &tileData);

        /**
         * Set resource reference (dynamically set width and height)
         * 设置资源引用（动态设置宽高）
         * @param x
         * @param y
         * @param resourceRef
         */
        void SetTile(int x, int y, const ResourceRef &resourceRef);

        [[nodiscard]] const ResourceRef *GetResourceRef(uint32_t x, uint32_t y) const;

        [[nodiscard]] TileVector2D GetStartPosition() const;
    };
}

#endif //GLIMMERWORKS_STRUCTUREINFO_H
