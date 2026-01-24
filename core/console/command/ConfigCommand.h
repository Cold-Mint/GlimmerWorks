//
// Created by Cold-Mint on 2025/12/2.
//

#ifndef GLIMMERWORKS_CONFIGCOMMAND_H
#define GLIMMERWORKS_CONFIGCOMMAND_H

#include "../Command.h"
#include "cmake-build-debug/_deps/toml11-src/include/toml.hpp"

namespace glimmer {
    enum class ConfigType {
        STRING, ARRAY, TABLE, FLOAT, INT, BOOLEAN
    };

    class ConfigCommand final : public Command {
        toml::value *configValue_;

    protected:
        void InitSuggestions(NodeTree<std::string> &suggestionsTree) override;

    public:
        explicit ConfigCommand(AppContext *ctx, toml::value *value)
            : Command(ctx), configValue_(value) {
        }

        ~ConfigCommand() override = default;

        [[nodiscard]] std::string GetName() const override;

        void PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) override;

        [[nodiscard]] ConfigType GetParameterType(const std::string &parameterName) const;

        [[nodiscard]] std::string GetValue(const std::string &parameterName) const;

        [[nodiscard]] bool SetValue(const std::string &parameterName, const std::string &value) const;

        bool Execute(CommandArgs commandArgs, std::function<void(const std::string &text)> onMessage) override;

        [[nodiscard]] NodeTree<std::string> GetSuggestionsTree(const CommandArgs &commandArgs) override;
    };
}

#endif //GLIMMERWORKS_CONFIGCOMMAND_H
