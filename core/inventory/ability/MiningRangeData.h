//
// Created by coldmint on 2026/3/2.
//

#ifndef GLIMMERWORKS_MININGRANGEDATA_H
#define GLIMMERWORKS_MININGRANGEDATA_H
#include <vector>

#include "core/ecs/component/TileLayerComponent.h"
#include "core/math/Vector2D.h"


/**
 * Mining Range Data
 * 挖掘范围数据。
 */
namespace glimmer {
    class MiningRangeData {
        //The array of coordinates being excavated.
        //正在被挖掘的坐标数组。
        std::vector<WorldVector2D> points_;
        float maxHardness_ = 0;

    public:
        MiningRangeData();

        /**
         * Obtain all coordinate points.
         * 获取所有坐标点。
         * @return
         */
        [[nodiscard]] const std::vector<WorldVector2D> &GetPoints() const;

        /**
         * Achieve maximum hardness
         * 获取最大硬度
         * @return
         */
        [[nodiscard]] float GetMaxHardness() const;

        void Reset();


        void CalculateMining(WorldVector2D startVector, TileLayerComponent *tileLayerComponent);

        void CalculateChainMining(WorldVector2D startVector, TileLayerComponent *tileLayerComponent, int radius);
    };
}

#endif //GLIMMERWORKS_MININGRANGEDATA_H
