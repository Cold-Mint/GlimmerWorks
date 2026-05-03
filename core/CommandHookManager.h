//
// Created by coldmint on 2026/5/1.
//

#ifndef GLIMMERWORKS_COMMANDHOOKMANAGER_H
#define GLIMMERWORKS_COMMANDHOOKMANAGER_H
#include <memory>
#include <unordered_map>
#include <vector>

#include "CommandHookEntry.h"
#include "Config.h"
#include "SDL3/SDL_scancode.h"


namespace glimmer {
    class CommandHookManager {
        std::vector<std::unique_ptr<CommandHookEntry> > sessionCommandHookVector_;
        std::vector<std::unique_ptr<CommandHookEntry> > configCommandHookVector_;
        std::unordered_map<uint32_t, std::vector<CommandHookEntry *> > sessionCommandHookMap_;
        std::unordered_map<uint32_t, std::vector<CommandHookEntry *> > configCommandHookMap_;
        std::vector<CommandHookEntry *> fullVector_;


        [[nodiscard]] bool Exist(CommandHookScope scope,
                                 uint32_t key, const std::string &command) const;

        [[nodiscard]] std::optional<std::string> RegisterImpl(
            std::unordered_map<uint32_t, std::vector<CommandHookEntry *> > &commandHookMap,
            std::vector<std::unique_ptr<CommandHookEntry> > &commandHookVector, CommandHookScope exclude,
            std::unique_ptr<CommandHookEntry> commandHookEntry) const;

        [[nodiscard]] static bool UnregisterImpl(
            std::unordered_map<uint32_t, std::vector<CommandHookEntry *> > &commandHookMap,
            std::vector<std::unique_ptr<CommandHookEntry> > &commandHookVector, CommandHookScope exclude,
            const std::string &commandHookId
        );

    public:
        CommandHookManager();

        [[nodiscard]] const std::vector<CommandHookEntry *> &GetCommandHookVector(uint32_t key);

        /**
         * CreateCommandHookEntry
         * 创建命令钩子
         * @param scope scope 作用域
         * @param eventType eventType 事件类型
         * @param scancode scancode 扫描码
         * @param command command 命令
         * @param keyRepeat keyRepeat 是否在重复输入时执行。
         * @return If the hook does not exist, then return the created hook. If it exists, return an empty value. 如果钩子不存在，那么返回创建后的钩子。存在返回空。
         */
        [[nodiscard]] std::unique_ptr<CommandHookEntry> CreateCommandHookEntry(
            CommandHookScope scope, SDL_EventType eventType, SDL_Scancode scancode, const std::string &command,
            bool keyRepeat) const;

        void LoadHookFromConfig(const std::vector<CommandHookResource> &commandHooks);

        [[nodiscard]] std::vector<std::string> GetCommandHookIdsWithOutConfig() const;


        [[nodiscard]] std::optional<std::string> Register(std::unique_ptr<CommandHookEntry> commandHookEntry);

        [[nodiscard]] bool Unregister(const std::string &commandHookId);
    };
}

#endif //GLIMMERWORKS_COMMANDHOOKMANAGER_H
