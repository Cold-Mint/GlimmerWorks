//
// Created by coldmint on 2026/4/6.
//

#ifndef GLIMMERWORKS_LOCATECOMMAND_H
#define GLIMMERWORKS_LOCATECOMMAND_H
#include "core/console/Command.h"
#include "core/mod/Resource.h"
#include "core/mod/dataPack/BiomesManager.h"
#include "core/world/generator/ChunkGenerator.h"


namespace glimmer {
    class LocateCommand : public Command {
    public:
        explicit LocateCommand(AppContext *appContext);

        static std::optional<TileVector2D> SearchBiomes(int tileX, const BiomesManager *biomesManager, ChunkGenerator *chunkGenerator,
                                                        const std::string &targetBiomeId);

    protected:
        void InitSuggestions(NodeTree<std::string> &suggestionsTree) override;

    public:
        [[nodiscard]] std::string GetName() const override;

        [[nodiscard]] bool RequiresWorldContext() const override;

        void PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) override;

        bool Execute(const CommandSender *commandSender, CommandArgs commandArgs,
            std::function<void(const std::string &text)> onMessage) override;
    };
}


#endif //GLIMMERWORKS_LOCATECOMMAND_H
