//
// Created by Cold-Mint on 2026/5/18.
//
#if  !defined(NDEBUG)
#include "MemoryUsageCommand.h"
#include "../../scene/AppContext.h"
#include "../../world/WorldContext.h"
#include "core/MemoryUsage.h"
#include "fmt/color.h"

glimmer::MemoryUsageCommand::MemoryUsageCommand(AppContext *appContext) : Command(appContext) {
}

std::string glimmer::MemoryUsageCommand::BytesToReadableUnit(uint64_t byteSize) {
    constexpr double KB = 1024.0;
    constexpr double MB = KB * 1024.0;
    constexpr double GB = MB * 1024.0;

    auto size = static_cast<double>(byteSize);

    if (size >= GB) {
        return fmt::format("{:.2f} GB", size / GB);
    }
    if (size >= MB) {
        return fmt::format("{:.2f} MB", size / MB);
    }
    if (size >= KB) {
        return fmt::format("{:.2f} KB", size / KB);
    }
    return fmt::format("{} Byte", byteSize);
}

void glimmer::MemoryUsageCommand::InitSuggestions(NodeTree<std::string> *suggestionsTree) {
}

std::string glimmer::MemoryUsageCommand::GetName() const {
    return MEMORY_USAGE_COMMAND_NAME;
}

void glimmer::MemoryUsageCommand::
PutCommandStructure(const CommandArgs *commandArgs, std::vector<std::string> *strings) {
}

bool glimmer::MemoryUsageCommand::Execute(const CommandSender *commandSender, const CommandArgs *commandArgs,
                                          const std::function<void(const std::string &text)> *onMessage) {
    if (appContext_ == nullptr || commandArgs == nullptr || onMessage == nullptr) {
        return false;
    }
    const std::function<void(const std::string &text)> &onMessageRef = *onMessage;
    onMessageRef(fmt::format(fmt::runtime(appContext_->GetLangsResources()->memoryUseInfo), MemoryUsage::TotalBytes,
                             BytesToReadableUnit(MemoryUsage::TotalBytes)));
    return true;
}
#endif
