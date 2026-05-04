//
// Created by coldmint on 2026/4/15.
//

#ifndef GLIMMERWORKS_LIGHTINGBUFFER_H
#define GLIMMERWORKS_LIGHTINGBUFFER_H

#include "LightMask.h"
#include "LightSource.h"
#include "TileLightData.h"
#include "TraverseAction.h"
#include "core/math/Color.h"


namespace glimmer {
    class LightBuffer {
        std::unordered_map<const TileVector2D, std::unique_ptr<TileLightData>, Vector2DIHash> tileLightData_;

        TraverseAction ClearLightStepCallback(const LightSource *lightSourcePtr,
                                              TileVector2D current,
                                              TileVector2D next, bool centerOfCircle, TileLayerType layerType,
                                              int rayIndex);

        TraverseAction SetLightStepCallback(const LightSource *lightSourcePtr, TileVector2D current, TileVector2D next,
                                            bool centerOfCircle, TileLayerType layerType,
                                            int rayIndex);

    public:
        void SetLightMask(TileVector2D position, TileLayerType layerType, std::unique_ptr<LightMask> lightMask);

        void ClearLightMask(const TileVector2D &position, TileLayerType layerType);

        void ClearTileLightData(const TileVector2D &position);

        [[nodiscard]] const TileLightData *GetTileLightData(const TileVector2D &position) const;

        void SetLightSource(TileVector2D position, TileLayerType layerType, std::unique_ptr<LightSource> lightSource);

        void ClearLightSource(TileVector2D position, TileLayerType layerType);

        const Color *GetFinalLightColor(TileVector2D position);
    };
}


#endif //GLIMMERWORKS_LIGHTINGBUFFER_H
