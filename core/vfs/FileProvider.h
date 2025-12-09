//
// Created by Cold-Mint on 2025/11/28.
//

#ifndef GLIMMERWORKS_FILEPROVIDER_H
#define GLIMMERWORKS_FILEPROVIDER_H
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace glimmer {
    /**
     * File provider
     * 文件提供者
     */
    class FileProvider {
    public:
        virtual ~FileProvider() = default;

        /**
         * Get the name of the file provider
         * 获取文件提供者的名称
         * @return
         */
        [[nodiscard]] virtual std::string GetFileProviderName() const = 0;

        /**
         * Read the file
         * 读取文件
         * @param path path 文件路径
         * @return string 文件内容
         */
        [[nodiscard]] virtual std::optional<std::string> ReadFile(const std::string &path) = 0;


        /**
         * Read Stream
         * 读取文件流
         * @param path  path 文件路径
         * @return stream 文件流
         */
        [[nodiscard]] virtual std::optional<std::unique_ptr<std::istream> > ReadStream(const std::string &path) = 0;


        /**
         * Whether the specified route exists
         * 指定的路经是否存在
         * @param path path 文件路径
         * @return 是否存在
         */
        [[nodiscard]] virtual bool Exists(const std::string &path) = 0;


        /**
         * IsFile
         * 指定的路径是否为文件
         * @param path path 文件路径
         * @return 是否为文件
         */
        [[nodiscard]] virtual bool IsFile(const std::string &path) = 0;

        /**
         * Write the file
         * 写出文件
         * @param path path 文件路径
         * @param content content文件内容
         * @return 是否写出成功
         */
        [[nodiscard]] virtual bool WriteFile(const std::string &path, const std::string &content) = 0;

        [[nodiscard]] virtual std::vector<std::string> ListFile(const std::string &path) = 0;

        [[nodiscard]] virtual std::optional<std::string> GetActualPath(const std::string &path) const = 0;

        virtual bool CreateFolder(const std::string &path) = 0;
    };
}

#endif //GLIMMERWORKS_FILEPROVIDER_H
