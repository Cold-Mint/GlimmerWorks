//
// Created by coldmint on 2026/3/28.
//

#ifndef GLIMMERWORKS_FLOWINGTEXTCREATOR_H
#define GLIMMERWORKS_FLOWINGTEXTCREATOR_H
#include "IEntityCreator.h"
#include "IPersistenceEntityCreator.h"


namespace glimmer {
    class FlowingTextCreator : public IEntityCreator {
        std::string text_;
        WorldVector2D position_;

    public:
        explicit FlowingTextCreator(WorldContext *worldContext, const std::string &text, WorldVector2D position);

        void LoadTemplateComponents(GameEntity::ID id) override;
    };
}


#endif //GLIMMERWORKS_FLOWINGTEXTCREATOR_H
