//
// Created by Cold-Mint on 2025/10/15.
//

#ifndef GLIMMERWORKS_COMMANDARGS_H
#define GLIMMERWORKS_COMMANDARGS_H
#include <string>
#include <vector>
#include <optional>

#include "../mod/ResourceRef.h"
#include "core/BoolOrToggle.h"

namespace glimmer {
    class CommandArgs {
        std::vector<std::string> tokens_;
        std::string command_;

    public:
        explicit CommandArgs(const std::string &command);

        /**
         * Obtain the Token position corresponding to the cursor position
         * 获取光标位置对应的Token位置
         * @param cursorPos cursorPos 光标位置
         * @return
         */
        [[nodiscard]] int GetTokenIndexAtCursor(int cursorPos) const;

        /**
         * Get the keyword corresponding to the cursor position
         * 获取光标位置对应的关键字
         * @param cursorPos cursorPos 光标位置
         * @return
         */
        [[nodiscard]] std::string GetKeywordAtCursor(int cursorPos) const;

        [[nodiscard]] int GetSize() const;

        [[nodiscard]] bool AsBool(int index) const;

        /**
         *
         * 转为bool值或者toggle
         * @param index
         * @return
         */
        [[nodiscard]] BoolOrToggle AsBoolOrToggle(int index) const;

        [[nodiscard]] int AsInt(int index) const;

        /**
         * Convert to coordinates
         * 转换为坐标
         * @param index index 位置
         * @param origin origin(Used to replace the tilde symbol ~) 原点坐标(用于替代~号)
         * @return  Return to world coordinates 返回世界坐标
         */
        [[nodiscard]] float AsCoordinate(int index, float origin) const;

        [[nodiscard]] float AsFloat(int index) const;

        [[nodiscard]] double AsDouble(int index) const;

        [[nodiscard]] std::string AsString(int index) const;

        [[nodiscard]] std::optional<ResourceRef> AsResourceRef(int index, int resourceType) const;
    };
}

#endif //GLIMMERWORKS_COMMANDARGS_H
