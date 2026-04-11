//
// Created by coldmint on 2026/4/11.
//

#ifndef GLIMMERWORKS_CLOCKWISEWALKER8_H
#define GLIMMERWORKS_CLOCKWISEWALKER8_H
#include <functional>

#include "TraverseAction.h"
#include "core/ecs/component/TileLayerComponent.h"


namespace glimmer {
    /**
     * LightPropagationTraverser
     * 光照传播遍历器
     *
     * Starting from the specified center tile, perform an 8-direction flood traversal outward with the specified maximum radius.
     * This is used to achieve functions such as light diffusion, brightness attenuation, and calculation of the influence range of the light source.
     * During the traversal, the Euclidean distance will be automatically calculated, and the propagation behavior can be controlled through callbacks.
     * 从指定中心瓦片开始，以指定最大半径向外进行8方向泛洪遍历，
     * 用于实现光照扩散、亮度衰减、光源影响范围计算等功能。
     * 遍历时会自动计算欧几里得距离，并通过回调控制传播行为。
     */
    class LightPropagationTraverser {
        const std::array<TileVector2D, 8> DIRECTION_OFFSETS = {
            TileVector2D{0, -1},
            TileVector2D{1, -1},
            TileVector2D{1, 0},
            TileVector2D{1, 1},
            TileVector2D{0, 1},
            TileVector2D{-1, 1},
            TileVector2D{-1, 0},
            TileVector2D{-1, -1}
        };

        /**
         * The movement radius means that the distance in any direction cannot exceed this radius.
         * 游走半径，每个方向不能超过这个半径。
         */
        int maxRadius_ = 0;

        /**
         * 游走时的回调函数，TileVector2D表示当前游走坐标，返回bool，如果返回true，则终止游走当前方向。
         *  distance The distance from the current point to the center point
         *  distance 当前点距离中心点的距离
         */
        std::function<TraverseAction(TileVector2D current, TileVector2D next,
                                     float distance)> stepCallback_;

        /**
         * center
         * 中点
         */
        TileVector2D center_;

        /**
         * 射线数量
         */
        int rayCount_ = 0;

        /**
         * 射线角度
         */
        float rayAngleStep_ = 0;

    public:
        LightPropagationTraverser(int maxRadius,
                                     const std::function<TraverseAction(
                                         TileVector2D current, TileVector2D next,
                                         float distance)> &stepCallback,
                                     TileVector2D center);


        /**
         * Start
         * 开始遍历
         */
        void Start() const;
    };
}


#endif //GLIMMERWORKS_CLOCKWISEWALKER8_H
