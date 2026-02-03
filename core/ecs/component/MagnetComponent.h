//
// Created by coldmint on 2025/12/22.
//

#ifndef GLIMMERWORKS_MAGNETCOMPONENT_H
#define GLIMMERWORKS_MAGNETCOMPONENT_H
#include <vector>

#include "../GameComponent.h"
#include "../GameEntity.h"
#include "../../Constants.h"

namespace glimmer {
    /**
     * Magnet (the side that generates suction)
     * 吸铁石（产生吸力的一方）
     */
    class MagnetComponent : public GameComponent {
        /**
         * radius(Add it to entities when the radius is less than or equal to this)
         * 吸附半径(当小于等于此半径时将其加入到entities内)
         */
        float adsorptionRadius_ = TILE_SIZE;

        /**
         * Detection radius (the sensing range of the magnet. When adsorbable substances enter the range, they will move towards the magnet)
         * 检测半径（吸铁石的感知范围，可吸附物进入范围时会向吸铁石移动）
         */
        float detectionRadius_ = TILE_SIZE * 3;

        /**
         * Types of magnetic adsorbates that can be adsorbed(Multiple can be set.)
         * magnetic_type_item | magnetic_type_money
         * 可被吸附的磁吸物类型(可以设置多个。)
         * 例如：MAGNETIC_TYPE_ITEM|MAGNETIC_TYPE_MONEY
         */
        uint16_t type_ = 0;

        /**
         *The id of the adsorbed entity
         * 被吸附的实体id
         */
        std::vector<GameEntity::ID> entities_;

    public:
        void SetType(uint16_t type);

        [[nodiscard]] uint16_t GetType() const;

        void SetDetectionRadius(float detectionRadius);

        [[nodiscard]] float GetDetectionRadius() const;

        void SetAdsorptionRadius(float adsorptionRadius);

        [[nodiscard]] float GetAdsorptionRadius() const;

        void RemoveEntity(GameEntity::ID entityId);

        void AddEntity(GameEntity::ID entityId);

        [[nodiscard]] const std::vector<GameEntity::ID> &GetEntities() const;

        [[nodiscard]] bool IsSerializable() override;

        [[nodiscard]] std::string Serialize() override;

        void Deserialize(WorldContext *worldContext, const std::string &data) override;

        [[nodiscard]] u_int32_t GetId() override;
    };
}

#endif //GLIMMERWORKS_MAGNETCOMPONENT_H
