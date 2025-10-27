//
// Created by Cold-Mint on 2025/10/26.
//

#ifndef GLIMMERWORKS_GAMEENTITY_H
#define GLIMMERWORKS_GAMEENTITY_H

namespace glimmer {
    class GameEntity {
    public:
        using ID = unsigned int;

    private:
        ID id_;

    public:
        explicit GameEntity(const ID id) : id_(id) {
        }

        [[nodiscard]] ID GetID() const { return id_; }
    };
}

#endif //GLIMMERWORKS_GAMEENTITY_H
