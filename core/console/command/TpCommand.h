//
// Created by Cold-Mint on 2025/11/16.
//

#ifndef GLIMMERWORKS_TPCOMMAND_H
#define GLIMMERWORKS_TPCOMMAND_H
#include "../Command.h"

namespace glimmer
{
    class TpCommand final : public Command
    {
    public:
        explicit TpCommand(AppContext* ctx) : Command(ctx)
        {
        }

        ~TpCommand() override = default;
        [[nodiscard]] std::string GetName() const override;
        void InitSuggest() override;
        bool Execute(CommandArgs commandArgs, std::function<void(const std::string& text)> onOutput) override;

        [[nodiscard]] bool RequiresWorldContext() const override;
    };
}


#endif //GLIMMERWORKS_TPCOMMAND_H
