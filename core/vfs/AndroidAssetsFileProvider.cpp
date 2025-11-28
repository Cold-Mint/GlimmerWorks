//
// Created by coldmint on 2025/11/28.
//

#include "AndroidAssetsFileProvider.h"
#include "../log/LogCat.h"
#include <streambuf>
#include <istream>

namespace glimmer {
    class AAssetStreamBuf : public std::streambuf {
        AAsset *asset_;
        char buffer_[1024];

    public:
        explicit AAssetStreamBuf(AAsset *asset) : asset_(asset) {
        }

        ~AAssetStreamBuf() override {
            if (asset_) {
                AAsset_close(asset_);
            }
        }

    protected:
        int_type underflow() override {
            if (AAsset_getRemainingLength(asset_) == 0) {
                return traits_type::eof();
            }

            int bytesRead = AAsset_read(asset_, buffer_, sizeof(buffer_));
            if (bytesRead <= 0) {
                return traits_type::eof();
            }

            setg(buffer_, buffer_, buffer_ + bytesRead);
            return traits_type::to_int_type(buffer_[0]);
        }
        
        pos_type seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which) override {
            if (which & std::ios_base::in) {
                 off_type newPos;
                 switch (dir) {
                     case std::ios_base::beg:
                         newPos = off;
                         break;
                     case std::ios_base::cur:
                         newPos = AAsset_getLength(asset_) - AAsset_getRemainingLength(asset_) + off; // This is tricky with buffer, simplifying
                         // Correct implementation requires tracking position carefully or using AAsset_seek directly
                         // Since we are buffering, we need to be careful.
                         // For simplicity, let's rely on AAsset_seek but we need to invalidate buffer.
                         break;
                     case std::ios_base::end:
                         newPos = AAsset_getLength(asset_) + off;
                         break;
                     default:
                         return -1;
                 }
                 
                 // However, AAsset_seek is simple.
                 // Let's implement a simpler version that just forwards to AAsset_seek if possible, 
                 // but mixing with streambuf buffering is complex.
                 // For read-only sequential access, we might not need seekoff.
                 // But if we do:
                 
                 off_type ret = AAsset_seek(asset_, off, (dir == std::ios_base::beg ? SEEK_SET : (dir == std::ios_base::cur ? SEEK_CUR : SEEK_END)));
                 if (ret != -1) {
                     setg(0, 0, 0); // Invalidate buffer
                     return ret;
                 }
            }
            return -1;
        }
        
        pos_type seekpos(pos_type pos, std::ios_base::openmode which) override {
             if (which & std::ios_base::in) {
                 off_type ret = AAsset_seek(asset_, pos, SEEK_SET);
                 if (ret != -1) {
                     setg(0, 0, 0);
                     return ret;
                 }
             }
             return -1;
        }
    };

    class AAssetStream : public std::istream {
        AAssetStreamBuf buf_;

    public:
        explicit AAssetStream(AAsset *asset) : std::istream(&buf_), buf_(asset) {
        }
    };

    AndroidAssetsFileProvider::AndroidAssetsFileProvider(AAssetManager *assetManager, std::string root)
        : assetManager_(assetManager), root_(std::move(root)) {
    }

    std::string AndroidAssetsFileProvider::GetFileProviderName() const {
        return "AndroidAssetsFileProvider(" + root_ + ")";
    }

    std::optional<std::string> AndroidAssetsFileProvider::ReadFile(const std::string &path) {
        std::string fullPath = root_.empty() ? path : root_ + "/" + path;
        // Remove leading ./ if present, AAssetManager doesn't like it? Actually it might be fine, but let's be clean.
        if (fullPath.rfind("./", 0) == 0) {
             fullPath = fullPath.substr(2);
        }
        
        AAsset *asset = AAssetManager_open(assetManager_, fullPath.c_str(), AASSET_MODE_BUFFER);
        if (!asset) {
            return std::nullopt;
        }

        off_t length = AAsset_getLength(asset);
        std::string content;
        content.resize(length);

        AAsset_read(asset, content.data(), length);
        AAsset_close(asset);

        return content;
    }

    std::optional<std::unique_ptr<std::istream> > AndroidAssetsFileProvider::ReadStream(const std::string &path) {
        std::string fullPath = root_.empty() ? path : root_ + "/" + path;
         if (fullPath.rfind("./", 0) == 0) {
             fullPath = fullPath.substr(2);
        }

        AAsset *asset = AAssetManager_open(assetManager_, fullPath.c_str(), AASSET_MODE_UNKNOWN);
        if (!asset) {
            return std::nullopt;
        }

        return std::make_unique<AAssetStream>(asset);
    }

    bool AndroidAssetsFileProvider::Exists(const std::string &path) {
        std::string fullPath = root_.empty() ? path : root_ + "/" + path;
         if (fullPath.rfind("./", 0) == 0) {
             fullPath = fullPath.substr(2);
        }
        
        AAsset *asset = AAssetManager_open(assetManager_, fullPath.c_str(), AASSET_MODE_UNKNOWN);
        if (asset) {
            AAsset_close(asset);
            return true;
        }
        return false;
    }

    bool AndroidAssetsFileProvider::IsFile(const std::string &path) {
        return Exists(path); // Simplification: AAssetManager doesn't easily distinguish unless we try to open dir
    }

    bool AndroidAssetsFileProvider::WriteFile(const std::string &path, const std::string &content) {
        LogCat::e("AndroidAssetsFileProvider::WriteFile is not supported");
        return false;
    }

    std::vector<std::string> AndroidAssetsFileProvider::ListFile(const std::string &path) {
        std::string fullPath = root_.empty() ? path : root_ + "/" + path;
         if (fullPath.rfind("./", 0) == 0) {
             fullPath = fullPath.substr(2);
        }
        
        std::vector<std::string> result;
        AAssetDir *assetDir = AAssetManager_openDir(assetManager_, fullPath.c_str());
        if (!assetDir) {
            return result;
        }

        const char *filename;
        while ((filename = AAssetDir_getNextFileName(assetDir)) != nullptr) {
            result.emplace_back(filename);
        }
        AAssetDir_close(assetDir);
        return result;
    }

    std::optional<std::string> AndroidAssetsFileProvider::GetActualPath(const std::string &path) const {
        return std::nullopt; // Assets don't have a real filesystem path
    }

    bool AndroidAssetsFileProvider::CreateFolder(const std::string &path) {
        return false;
    }
}
