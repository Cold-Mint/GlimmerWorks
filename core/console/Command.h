//
// Created by Cold-Mint on 2025/10/15.
//

#ifndef GLIMMERWORKS_COMMAND_H
#define GLIMMERWORKS_COMMAND_H
#include <functional>
#include <string>

#include "CommandArgs.h"

namespace glimmer
{
    class WorldContext;
    class AppContext;

    class Command
    {
    protected:
        AppContext* appContext_ = nullptr;
        WorldContext* worldContext_ = nullptr;

    public:
        virtual ~Command() = default;

        explicit Command(AppContext* ctx) : appContext_(ctx)
        {
        }

        [[nodiscard]] virtual std::string GetName() const = 0;

        /**
         * Is the world context required to execute this command
         * 是否需要世界上下文，才能执行该命令
         * @return 是否需要世界上下文
         */
        [[nodiscard]] virtual bool RequiresWorldContext() const;
        /**
         * Bind the world context
         * 绑定世界上下文
         * @param worldContext 世界上下文
         */
        void BindWorldContext(WorldContext* worldContext);
        /**
         * UnBind the world context
         * 解绑世界上下文
         */
        void UnBindWorldContext();


        virtual void InitSuggest() = 0;

        virtual bool Execute(CommandArgs commandArgs, std::function<void(const std::string& text)> onOutput) = 0;
    };
}


#endif //GLIMMERWORKS_COMMAND_H
