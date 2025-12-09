//
// Created by Cold-Mint on 2025/12/2.
//

#ifndef GLIMMERWORKS_CONFIGCOMMAND_H
#define GLIMMERWORKS_CONFIGCOMMAND_H
#include <utility>

#include "../Command.h"
#include "nlohmann/json.hpp"
#include "nlohmann/json_fwd.hpp"

namespace glimmer {
    enum ConfigType {
        Number, String, Array, Object, Boolean
    };

    class ConfigCommand final : public Command {
    protected:
        void InitSuggestions(NodeTree<std::string> &suggestionsTree) override;

    public:
        nlohmann::json json_;

        explicit ConfigCommand(AppContext *ctx, nlohmann::json json)
            : Command(ctx), json_(std::move(json)) {
        }

        ~ConfigCommand() override = default;

        [[nodiscard]] std::string GetName() const override;

        void PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) override;

        [[nodiscard]] ConfigType GetParameterType(const std::string &parameterName) const;

        bool Execute(CommandArgs commandArgs, std::function<void(const std::string &text)> onOutput) override;

        [[nodiscard]] NodeTree<std::string> GetSuggestionsTree(const CommandArgs &commandArgs) override;
    };
}

#endif //GLIMMERWORKS_CONFIGCOMMAND_H
