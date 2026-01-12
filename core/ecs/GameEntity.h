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
        bool persistable_ = false;

    public:
        explicit GameEntity(const ID id) : id_(id) {
        }

        [[nodiscard]] ID GetID() const;

        void SetPersistable(bool persistable);

        [[nodiscard]] bool IsPersistable() const;
    };
}

#endif //GLIMMERWORKS_GAMEENTITY_H
