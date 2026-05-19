//
// Created by Cold-Mint on 2025/10/26.
//

#pragma once
#include "core/mod/ResourceRef.h"

namespace glimmer {
    class GameEntity {
    public:
        using ID = uint32_t;

    private:
        ID id_;
        bool persistable_ = false;
        ResourceRef resourceRef_;

    public:
        explicit GameEntity(ID id);

        [[nodiscard]] const ResourceRef &GetResourceRef() const;

        void SetResourceRef(const ResourceRef &resourceRef);

        [[nodiscard]] ID GetID() const;

        void SetPersistable(bool persistable);

        [[nodiscard]] bool IsPersistable() const;
    };
}
