//
// Created by coldmint on 2026/2/13.
//

#ifndef GLIMMERWORKS_STRUCTUREDYNAMICSUGGESTIONS_H
#define GLIMMERWORKS_STRUCTUREDYNAMICSUGGESTIONS_H
#include "DynamicSuggestions.h"
#include "core/mod/dataPack/StructureManager.h"

namespace glimmer {
    class StructureDynamicSuggestions : public DynamicSuggestions {
        StructureManager *structureManager_;

    public:
        explicit StructureDynamicSuggestions(StructureManager *structureManager);

        [[nodiscard]] std::string GetId() const override;

        std::vector<std::string> GetSuggestions(std::string param) override;

        bool Match(std::string keyword, std::string param) override;
    };
}


#endif //GLIMMERWORKS_STRUCTUREDYNAMICSUGGESTIONS_H
