//
// Created by Cold-Mint on 2025/10/15.
//

#include "HelpCommand.h"

#include <chrono>

#include "../../Constants.h"
#include "../../log/LogCat.h"
#include "../../scene/AppContext.h"
#include "fmt/color.h"


std::string glimmer::HelpCommand::GetName() const
{
    return HELP_COMMAND_NAME;
}

void glimmer::HelpCommand::InitSuggest()
{
}

bool glimmer::HelpCommand::Execute(CommandArgs commandArgs, std::function<void(const std::string& text)> onOutput)
{
    auto& commands = appContext_->commandManager->GetCommands();
    onOutput(fmt::format(
        fmt::runtime(appContext_->langs->commandInfo),
        commands.size()));
    for (const auto& [name, command] : commands)
    {
        onOutput(name + ": RequiresWorldContext=" + (command->RequiresWorldContext() ? "true" : "false"));
    }

    return true;
}
