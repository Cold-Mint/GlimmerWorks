//
// Created by Cold-Mint on 2025/12/11.
//

#ifndef GLIMMERWORKS_RESOURCELOCATOR_H
#define GLIMMERWORKS_RESOURCELOCATOR_H
#include <optional>

#include "Resource.h"
#include "ResourceRef.h"

namespace glimmer {
    class AppContext;
    /**
     * ResourceLocator，Used to interpret references and return the corresponding resources.
     * 资源定位器，用于解释引用并返回对应的资源。
     */
    class ResourceLocator {
        AppContext *appContext_;

    public:
        ResourceLocator() : appContext_(nullptr) {
        }

        void SetAppContext(AppContext *appContext);

        /**
         * FindString
         * 查找字符串资源
         * @param resourceRef resourceRef 字符串引用
         * @return
         */
        [[nodiscard]] std::optional<StringResource *> FindString(const ResourceRef &resourceRef) const;


        /**
         * FindTile
         * 查找瓦片资源
         * @param resourceRef  resourceRef 瓦片引用
         * @return
         */
        [[nodiscard]] std::optional<TileResource *> FindTile(const ResourceRef &resourceRef) const;
    };
}

#endif //GLIMMERWORKS_RESOURCELOCATOR_H
