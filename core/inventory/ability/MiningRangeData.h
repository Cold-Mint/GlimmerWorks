//
// Created by Cold-Mint on 2026/3/2.
//

#pragma once
#include <unordered_set>
#include <vector>

#include "MiningRangeDataPoint.h"
#include "core/ecs/component/TileLayerComponent.h"


/**
 * Mining Range Data
 * 挖掘范围数据。
 */
namespace glimmer {
    class MiningRangeData {
        //The array of coordinates being excavated.
        //正在被挖掘的坐标数组。
        std::vector<MiningRangeDataPoint> points_;
        std::unordered_set<Vector2DIFingerprint> pointsFingerprint_;
        float maxHardness_ = 0;

        void TryPushPoint(const TileLayerComponent *tileLayerComponent, const TileVector2D &position);

    public:
        MiningRangeData();

        [[nodiscard]] size_t GetPointsCount() const;

        [[nodiscard]] const MiningRangeDataPoint *GetPoint(size_t index) const;

        /**
         * Achieve maximum hardness
         * 获取最大硬度
         * @return
         */
        [[nodiscard]] float GetMaxHardness() const;

        void Reset();


        void CalculateMining(const TileLayerComponent *tileLayerComponent, const TileVector2D &startVector);

        void CalculateChainMining(const TileLayerComponent *tileLayerComponent, const TileVector2D &startVector,
                                  uint8_t radius);
    };
}
