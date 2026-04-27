//
// Created by coldmint on 2026/4/11.
//

#ifndef GLIMMERWORKS_CLOCKWISEWALKER8_H
#define GLIMMERWORKS_CLOCKWISEWALKER8_H
#include <functional>
#include <unordered_set>

#include "TraverseAction.h"
#include "core/Constants.h"
#include "core/math/Vector2DI.h"

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

        //Use the "使用GlimmerWorksCli raycirc" command to perform the pre-computation.
        //使用GlimmerWorksCli raycirc命令预先计算。
        const std::array<int, CHUNK_SIZE> RAY_COUNT = {
            4, //r=1
            8, //r=2
            16, //r=3
            20, //r=4
            32, //r=5
            32, //r=6
            40, //r=7
            48, //r=8
            58, //r=9
            80, //r=10
            80, //r=11
            88, //r=12
            88, //r=13
            112, //r=14
            122, //r=15
            128 //r=16
        };

        const std::array<float, CHUNK_SIZE> ANGLE_STEPS = {
            90.0F, //r=1
            45.0F, //r=2
            22.5F, //r=3
            18.0F, //r=4
            11.25F, //r=5
            11.25F, //r=6
            9.0F, //r=7
            7.5F, //r=8
            6.2069F, //r=9
            4.5F, //r=10
            4.5F, //r=11
            4.0909F, //r=12
            4.0909F, //r=13
            3.2143F, //r=14
            2.9508F, //r=15
            2.8125F //r=16
        };

        /**
         * The callback function during movement. TileVector2D represents the current movement coordinates. It returns a boolean value. If the value returned is true, it terminates the current movement direction.
         * 游走时的回调函数，TileVector2D表示当前游走坐标，返回bool，如果返回true，则终止游走当前方向。
         */
        std::function<TraverseAction(TileVector2D current, TileVector2D next, bool centerOfCircle, int reyIndex)>
        stepCallback_;

        /**
         * 射线数量
         */
        int rayCount_ = 0;

        /**
         * 射线角度
         */
        float rayAngleStep_ = 0;

        TileVector2D center_;
        int maxRadius_ = 0;

        /**
         * PropagateSingleRayImpl
         * 发射一条射线内部实现
         * @param rayIndex rayIndex 射线索引
         * @param visited  visited 已访问坐标
         * @param maxRadSq maxRadSq 最大半径的平方
         */
        void PropagateSingleRayImpl(int rayIndex, std::unordered_set<TileVector2D, Vector2DIHash> &visited,
                                    int maxRadSq) const;

        /**
         * Handle the special case where the radius is 0.
         * 处理半径为0的特殊情况。
         */
        void HandleRadiusIsZero() const;

    public:
        LightPropagationTraverser(TileVector2D center, int maxRadius,
                                  const std::function<TraverseAction(TileVector2D current, TileVector2D next,
                                                                     bool centerOfCircle,
                                                                     int reyIndex)> &stepCallback);


        /**
         * PropagateAllRays
         * 向四周发射全部射线
         */
        void PropagateAllRays() const;

        void PropagateSingleRay(int rayIndex) const;
    };
}


#endif //GLIMMERWORKS_CLOCKWISEWALKER8_H
