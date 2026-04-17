//@genCode
//
// Created by Cold-Mint on 2025/10/10.
//
#ifndef DATAPACKMANIFEST_H
#define DATAPACKMANIFEST_H
#include <string>

#include "ResourceRef.h"
#include "dataPack/PackDependence.h"


namespace glimmer {
    //@genNextLine(PackManifest|包清单)
    struct PackManifest {
        //@genNextLine(id|包唯一标识)
        std::string id;
        //@genNextLine(name|包名称资源引用)
        ResourceRef name;
        //@genNextLine(description|包描述资源引用)
        ResourceRef description;
        //@genNextLine(author|包作者)
        std::string author;
        //@genNextLine(versionName|版本名称)
        std::string versionName;
        //@genNextLine(versionNumber|版本号)
        uint32_t versionNumber;
        //@genNextLine(minGameVersion|最低兼容游戏版本号)
        uint32_t minGameVersion;
        //@genNextLine(resPack|是否为资源包)
        bool resPack;
        //@genNextLine(templateSearchPath|模板搜索路径)
        std::vector<std::string> templateSearchPath = {TEMPLATE_CURRENT, TEMPLATE_ROOT};
    };

    //@genNextLine(DataPackManifest|数据包包清单)
    struct DataPackManifest : PackManifest {
        //@genNextLine(DatapackDependencies|包依赖列表)
        std::vector<PackDependence> packDependencies;
    };

    //@genNextLine(ResourcePackManifest|资源包包清单)
    struct ResourcePackManifest : PackManifest {
    };
}


#endif //DATAPACKMANIFEST_H
