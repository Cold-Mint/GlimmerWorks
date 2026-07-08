/*
 * Copyright (C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * 版权(C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * 本程序是自由软件：你可以遵照自由软件基金会出版的GNU Affero通用公共许可证条款来重新分发和修改它
 * 该许可证的第3版，或者（由你选择）任何后续版本。
 *
 * 本程序的发布目的是希望它能有用，但没有任何担保；甚至没有适销性或特定用途适用性的默示担保。
 * 有关详细细节，请参阅GNU Affero通用公共许可证。
 *
 * 你应该已经收到一份GNU Affero通用公共许可证的副本。如果没有，请查阅<https://www.gnu.org/licenses/>。
 */
#include "DataPack.h"

#include <utility>

#include "blake3.h"
#include "monocypher-ed25519.h"
#include "SpecialFileProcessingParams.h"
#include "core/Constants.h"

#include "StringManager.h"
#include "TileResourceManager.h"
#include "core/log/LogCat.h"
#include "core/utils/TomlUtils.h"
#include "core/mod/PackManifest.h"
#include "core/lootTable/LootTableManager.h"
#include "core/scene/AppContext.h"
#include "core/utils/StringUtils.h"
#include "toml11/parser.hpp"

using enum glimmer::ShapeType;
using enum glimmer::BiomeDecoratorType;
using enum glimmer::PackVerifyState;


std::vector<std::filesystem::path> glimmer::DataPack::GetActuallyTemplateSearchPath(
    const std::filesystem::path& path) const
{
    const std::optional<std::string> currentOptional = virtualFileSystem_->GetParentPath(path);
    if (!currentOptional.has_value())
    {
        return {};
    }
    const std::string& currentDir = currentOptional.value();
    std::vector<std::filesystem::path> result;
    for (std::string searchPath : manifest_.templateSearchPath)
    {
        StringUtils::ReplaceAll(searchPath, TEMPLATE_CURRENT, currentDir);
        StringUtils::ReplaceAll(searchPath, TEMPLATE_ROOT, rootPath_.string());
        result.emplace_back(std::move(searchPath));
    }
    return result;
}

std::optional<std::string> glimmer::DataPack::GetDataType(const std::string& fileName)
{
    const size_t lastDot = fileName.rfind('.');
    if (lastDot == std::string::npos)
        return std::nullopt;

    const size_t secondLastDot = fileName.rfind('.', lastDot - 1);
    if (secondLastDot == std::string::npos)
        return std::nullopt;

    if (const std::string format = fileName.substr(lastDot + 1); format != "toml")
        return std::nullopt;
    return fileName.substr(secondLastDot + 1,
                           lastDot - secondLastDot - 1);
}

int glimmer::DataPack::LoadStringResourceFromFile(const std::filesystem::path& path, StringManager* stringManager) const
{
    const auto contentOptional = virtualFileSystem_->ReadFileAsString(path);
    if (!contentOptional.has_value())
    {
        return 0;
    }
    const std::vector<std::filesystem::path> searchPath = GetActuallyTemplateSearchPath(path);
    if (searchPath.empty())
    {
        return 0;
    }
    const toml::value value = toml::parse_str(
        tomlTemplateExpander_->Expand(searchPath, contentOptional.value(), virtualFileSystem_), tomlVersion_);
    int count = 0;
    auto array = toml::find<std::vector<StringResource>>(value, "string");
    for (auto& stringRes : array)
    {
        stringRes.packId = manifest_.id;
        stringManager->AddResource(
            std::make_unique<StringResource>(std::move(stringRes))
        );
        count++;
    }

    auto tagArray = toml::find<std::vector<StringResource>>(value, "tag_string");
    for (auto& stringRes : tagArray)
    {
        stringRes.packId = manifest_.id;
        stringManager->SetTagTranslate(
            StringUtils::StringToUint64(stringRes.resourceId), stringRes.value
        );
        count++;
    }
    return count;
}


void glimmer::DataPack::LoadLootTableResourceFromFile(const toml::value& value,
                                                      LootTableManager* lootTableManager) const
{
    auto lootResource = std::make_unique<LootResource>(toml::get<LootResource>(value));
    lootResource->packId = manifest_.id;
    for (auto& mandatory : lootResource->mandatory)
    {
        mandatory.item.SetSelfPackageId(manifest_.id);
        mandatory.mandatory = true;
    }
    for (auto& pool : lootResource->pool)
    {
        pool.item.SetSelfPackageId(manifest_.id);
        pool.mandatory = false;
    }
    lootTableManager->AddResource(std::move(lootResource));
}

void glimmer::DataPack::LoadInitialInventoryResourceFromFile(const toml::value& value,
                                                             InitialInventoryManager* lootTableManager) const
{
    auto initialInventoryResource = std::make_unique<InitialInventoryResource>(
        toml::get<InitialInventoryResource>(value));
    initialInventoryResource->packId = manifest_.id;
    for (auto& itemMessage : initialInventoryResource->addItems)
    {
        itemMessage.item.SetSelfPackageId(manifest_.id);
        for (auto& abilityItemRef : itemMessage.abilityItemRef)
        {
            abilityItemRef.item.SetSelfPackageId(manifest_.id);
        }
    }
    lootTableManager->AddResource(std::move(initialInventoryResource));
}

void glimmer::DataPack::LoadStructureResourceFromFile(const toml::value& value, StructureManager* structureManager,
                                                      StructureGeneratorType structureGeneratorType) const
{
    std::unique_ptr<IStructureResource> structureResource;
    switch (structureGeneratorType)
    {
    case StructureGeneratorType::Tree:
        structureResource = std::make_unique<TreeStructureResource>(
            toml::get<TreeStructureResource>(value));
        break;
    case StructureGeneratorType::Static:
        {
            std::unique_ptr<StaticStructureResource> staticStructureResource = std::make_unique<
                StaticStructureResource>(
                toml::get<StaticStructureResource>(value));
            for (auto& tile_info : staticStructureResource->tileInfo)
            {
                tile_info.tile.SetSelfPackageId(manifest_.id);
            }
            structureResource = std::move(staticStructureResource);
        }
        break;
    case StructureGeneratorType::None:
        break;
    }
    structureResource->packId = manifest_.id;
    structureResource->generatorId = std::to_underlying(structureGeneratorType);
    for (auto& ref : structureResource->data)
    {
        ref.SetSelfPackageId(manifest_.id);
    }
    for (auto& condition : structureResource->condition)
    {
        condition.config.UpdateArgs(manifest_.id);
    }
    structureManager->AddResource(std::move(structureResource));
}

void glimmer::DataPack::LoadTileResourceFromFile(const toml::value& value, TileResourceManager* tileManager) const
{
    auto tileResource = std::make_unique<TileResource>(toml::get<TileResource>(value));
    tileResource->packId = manifest_.id;
    tileResource->name.SetSelfPackageId(manifest_.id);
    tileResource->description.SetSelfPackageId(manifest_.id);
    tileResource->texture.SetSelfPackageId(manifest_.id);
    tileResource->blueprintTexture.SetSelfPackageId(manifest_.id);
    tileResource->breakSfx.SetSelfPackageId(manifest_.id);
    tileResource->placeSfx.SetSelfPackageId(manifest_.id);
    tileResource->lightSource.SetSelfPackageId(manifest_.id);
    tileResource->sideLightMask.SetSelfPackageId(manifest_.id);
    tileResource->backLightMask.SetSelfPackageId(manifest_.id);
    for (auto& tag : tileResource->tags)
    {
        tag.MakeCachedTag();
    }
    if (tileResource->customLootTable)
    {
        tileResource->lootTable.SetSelfPackageId(manifest_.id);
    }
    tileManager->AddResource(std::move(tileResource));
}

void glimmer::DataPack::LoadBiomeResourceFromFile(const toml::value& value, BiomesManager* biomesManager) const
{
    auto biomeResource = std::make_unique<BiomeResource>(toml::get<BiomeResource>(value));
    biomeResource->packId = manifest_.id;
    biomeResource->bgm.SetSelfPackageId(manifest_.id);
    for (auto& decorator : biomeResource->decors)
    {
        decorator.SetSelfPackageId(manifest_.id);
    }
    biomesManager->AddResource(std::move(biomeResource));
}

void glimmer::DataPack::LoadComposableItemResourceFromFile(const toml::value& value, ItemManager* itemManager) const
{
    auto itemResource = std::make_unique<ComposableItemResource>(toml::get<ComposableItemResource>(value));
    itemResource->packId = manifest_.id;
    itemResource->name.SetSelfPackageId(manifest_.id);
    itemResource->description.SetSelfPackageId(manifest_.id);
    itemResource->texture.SetSelfPackageId(manifest_.id);
    for (auto& tag : itemResource->tags)
    {
        tag.MakeCachedTag();
    }
    for (auto& defaultAbility : itemResource->defaultAbilityList)
    {
        defaultAbility.item.SetSelfPackageId(manifest_.id);
        for (auto& abilityItemRef : defaultAbility.abilityItemRef)
        {
            abilityItemRef.item.SetSelfPackageId(manifest_.id);
        }
    }
    itemManager->AddComposableResource(std::move(itemResource));
}

void glimmer::DataPack::LoadAbilityItemResourceFromFile(const toml::value& value, ItemManager* itemManager) const
{
    auto itemResource = std::make_unique<AbilityItemResource>(toml::get<AbilityItemResource>(value));
    itemResource->packId = manifest_.id;
    itemResource->name.SetSelfPackageId(manifest_.id);
    itemResource->description.SetSelfPackageId(manifest_.id);
    itemResource->texture.SetSelfPackageId(manifest_.id);
    for (auto& tag : itemResource->tags)
    {
        tag.MakeCachedTag();
    }
    itemManager->AddAbilityItemResource(std::move(itemResource));
}

void glimmer::DataPack::LoadMaterialItemResourceResourceFromFile(const toml::value& value,
                                                                 ItemManager* itemManager) const
{
    auto itemResource = std::make_unique<MaterialItemResource>(toml::get<MaterialItemResource>(value));
    itemResource->packId = manifest_.id;
    itemResource->name.SetSelfPackageId(manifest_.id);
    itemResource->description.SetSelfPackageId(manifest_.id);
    itemResource->texture.SetSelfPackageId(manifest_.id);
    for (auto& tag : itemResource->tags)
    {
        tag.MakeCachedTag();
    }
    itemManager->AddMaterialItemResource(std::move(itemResource));
}

void glimmer::DataPack::LoadContributorResourceFromFile(const toml::value& value,
                                                        ContributorManager* contributorManager) const
{
    auto contributorResource = std::make_unique<Contributor>(toml::get<Contributor>(value));
    contributorResource->displayName.SetSelfPackageId(manifest_.id);
    contributorManager->Register(std::move(contributorResource));
}

void glimmer::DataPack::LoadMobResourceFromFile(const toml::value& value, MobManager* mobManager) const
{
    auto mobResource = std::make_unique<MobResource>(toml::get<MobResource>(value));
    mobResource->packId = manifest_.id;
    mobResource->shape.SetSelfPackageId(manifest_.id);
    mobResource->texture.SetSelfPackageId(manifest_.id);
    ItemMessageResource& emptyHandAutoUseItem = mobResource->emptyHandAutoUseItem;
    emptyHandAutoUseItem.item.SetSelfPackageId(manifest_.id);
    for (auto& abilityItemRef : emptyHandAutoUseItem.abilityItemRef)
    {
        abilityItemRef.item.SetSelfPackageId(manifest_.id);
    }
    mobManager->Register(std::move(mobResource));
}

void glimmer::DataPack::
LoadShapeResourceFromFile(const toml::value& value, ShapeManager* shapeManager, ShapeType type) const
{
    std::unique_ptr<IShapeResource> shapeResource;
    switch (type)
    {
    case CIRCLE:
        {
            shapeResource = std::make_unique<CircularShapeResource>(
                toml::get<CircularShapeResource>(value));
            shapeResource->shapeType = std::to_underlying(CIRCLE);
            break;
        }

    case RECTANGLE:
        {
            shapeResource = std::make_unique<RectangleShapeResource>(
                toml::get<RectangleShapeResource>(value));
            shapeResource->shapeType = std::to_underlying(RECTANGLE);
            break;
        }
    case ROUNDED_RECTANGLE:
        {
            shapeResource = std::make_unique<RoundedRectangleShapeResource>(
                toml::get<RoundedRectangleShapeResource>(value));
            shapeResource->shapeType = std::to_underlying(ROUNDED_RECTANGLE);
            break;
        }
    }
    shapeResource->packId = manifest_.id;
    shapeManager->Register(std::move(shapeResource));
}

void glimmer::DataPack::LoadFixedColorResourceFromFile(const toml::value& value,
                                                       FixedColorManager* fixedColorManager) const
{
    auto fixedColorResource = std::make_unique<FixedColorResource>(toml::get<FixedColorResource>(value));
    fixedColorResource->packId = manifest_.id;
    fixedColorManager->Register(std::move(fixedColorResource));
}

void glimmer::DataPack::LoadLightMaskResourceFromFile(const toml::value& value,
                                                      LightMaskManager* lightMaskManager) const
{
    auto lightMaskResource = std::make_unique<LightMaskResource>(toml::get<LightMaskResource>(value));
    lightMaskResource->packId = manifest_.id;
    lightMaskResource->lightMaskColor.SetSelfPackageId(manifest_.id);
    lightMaskManager->Register(std::move(lightMaskResource));
}

void glimmer::DataPack::LoadLightSourceResourceFromFile(const toml::value& value,
                                                        LightSourceManager* lightSourceManager) const
{
    auto lightSourceResource = std::make_unique<LightSourceResource>(toml::get<LightSourceResource>(value));
    lightSourceResource->packId = manifest_.id;
    lightSourceResource->lightColor.SetSelfPackageId(manifest_.id);
    if (lightSourceResource->lightRadius > CHUNK_SIZE)
    {
        lightSourceResource->lightRadius = CHUNK_SIZE;
    }
    lightSourceManager->Register(std::move(lightSourceResource));
}

void glimmer::DataPack::LoadBiomeDecoratorResourceFromFile(const toml::value& value,
                                                           BiomeDecoratorResourcesManager* biomeDecoratorManager,
                                                           const BiomeDecoratorType type) const
{
    switch (type)
    {
    case FILL:
        {
            auto fillResource = std::make_unique<FillBiomeDecoratorResource>(
                toml::get<FillBiomeDecoratorResource>(value));
            fillResource->packId = manifest_.id;
            fillResource->tile.SetSelfPackageId(manifest_.id);
            fillResource->biomeDecoratorType = std::to_underlying(FILL);
            biomeDecoratorManager->Register(std::move(fillResource));
            break;
        }
    case MINERAL:
        {
            auto mineralBiomeDecoratorResource = std::make_unique<MineralBiomeDecoratorResource>(
                toml::get<MineralBiomeDecoratorResource>(value));
            mineralBiomeDecoratorResource->packId = manifest_.id;
            mineralBiomeDecoratorResource->ore.SetSelfPackageId(manifest_.id);
            mineralBiomeDecoratorResource->biomeDecoratorType = std::to_underlying(MINERAL);
            biomeDecoratorManager->Register(std::move(mineralBiomeDecoratorResource));
            break;
        }
    case SURFACE:
        {
            auto surfaceBiomeDecoratorResource = std::make_unique<SurfaceBiomeDecoratorResource>(
                toml::get<SurfaceBiomeDecoratorResource>(value));
            surfaceBiomeDecoratorResource->packId = manifest_.id;
            surfaceBiomeDecoratorResource->tile.SetSelfPackageId(manifest_.id);
            surfaceBiomeDecoratorResource->biomeDecoratorType = std::to_underlying(SURFACE);
            biomeDecoratorManager->Register(std::move(surfaceBiomeDecoratorResource));
            break;
        }
    }
}

void glimmer::DataPack::LoadRecipeResourceFromFile(const toml::value& value, RecipeManager* recipeManager) const
{
    auto recipeResource = std::make_unique<RecipeResource>(toml::get<RecipeResource>(value));
    recipeResource->packId = manifest_.id;
    ItemMessageResource& output = recipeResource->output;
    output.item.SetSelfPackageId(manifest_.id);
    for (auto& abilityItemRef : output.abilityItemRef)
    {
        abilityItemRef.item.SetSelfPackageId(manifest_.id);
    }
    for (auto& input : recipeResource->input)
    {
        input.MakeCachedTag();
    }
    recipeManager->RegisterRecipe(std::move(recipeResource));
}

std::optional<std::string> glimmer::DataPack::ExtractLanguageFromFileName(const std::string_view fileName)
{
    constexpr std::string_view suffix = ".strings.toml";
    if (!fileName.ends_with(suffix))
    {
        return std::nullopt;
    }
    std::string base(fileName.substr(0, fileName.size() - suffix.size()));
    auto pos = base.rfind('.');
    if (pos == std::string::npos)
    {
        return base;
    }
    return base.substr(pos + 1);
}

bool glimmer::DataPack::ProcessPublicKeyFile(const std::filesystem::path& path,
                                             SpecialFileProcessingParams& params) const
{
    const auto publicKeyStreamUniquePtr = virtualFileSystem_->ReadFileAsStream(path);
    if (publicKeyStreamUniquePtr == nullptr)
    {
        return false;
    }
    const auto publicKeyStream = publicKeyStreamUniquePtr.get();
    if (publicKeyStream == nullptr)
    {
        return false;
    }
    auto& pubStream = *publicKeyStream;
    pubStream.read(reinterpret_cast<char*>(params.publicKey.data()), 32);
    if (pubStream.gcount() == 32)
    {
        params.findPublicKey = true;
    }
    return true;
}

bool glimmer::DataPack::ProcessSignatureFile(const std::filesystem::path& path,
                                             SpecialFileProcessingParams& params) const
{
    const auto signStreamUniquePtr = virtualFileSystem_->ReadFileAsStream(path);
    if (signStreamUniquePtr == nullptr)
    {
        return false;
    }
    const auto signStream = signStreamUniquePtr.get();
    if (signStream == nullptr)
    {
        return false;
    }
    auto& sigStream = *signStream;
    sigStream.read(reinterpret_cast<char*>(params.signature.data()), 64);
    if (sigStream.gcount() == 64)
    {
        params.findSignature = true;
    }
    return true;
}

std::optional<std::vector<char>> glimmer::DataPack::ReadFileContent(std::istream* stream)
{
    if (stream->fail())
    {
        return std::nullopt;
    }

    constexpr std::streamsize BUFFER_SIZE = 8192;
    std::vector<char> fileBuffer;
    std::vector<char> tempBuf(BUFFER_SIZE);

    while (stream->read(tempBuf.data(), BUFFER_SIZE) || stream->gcount() > 0)
    {
        fileBuffer.insert(fileBuffer.end(), tempBuf.data(), tempBuf.data() + stream->gcount());
    }
    return fileBuffer;
}


glimmer::DataPack::DataPack(std::string path, const VirtualFileSystem* virtualFileSystem,
                            const TomlTemplateExpander* tomlTemplateExpander,
                            const toml::spec& tomlVersion) : rootPath_(std::move(path)),
                                                             manifest_(), tomlVersion_(tomlVersion),
                                                             virtualFileSystem_(virtualFileSystem),
                                                             tomlTemplateExpander_(tomlTemplateExpander)
{
}

bool glimmer::DataPack::LoadManifest()
{
    const auto contentOptional = virtualFileSystem_->ReadFileAsString(rootPath_ / MANIFEST_FILE_NAME);
    if (!contentOptional.has_value())
    {
        return false;
    }
    const toml::value value = toml::parse_str(contentOptional.value(), tomlVersion_);
    manifest_ = toml::get<DataPackManifest>(value);
    manifest_.name.SetSelfPackageId(manifest_.id);
    manifest_.description.SetSelfPackageId(manifest_.id);
    return true;
}

glimmer::PackVerifyState glimmer::DataPack::GetPackVerifyState() const
{
    return packVerifyState_;
}

void glimmer::DataPack::ComputeFileHash(const std::vector<char>& fileBuffer, std::vector<uint8_t>& allHashData)
{
    blake3_hasher hasher;
    blake3_hasher_init(&hasher);
    blake3_hasher_update(&hasher, fileBuffer.data(), fileBuffer.size());
    uint8_t singleHash[BLAKE3_OUT_LEN];
    blake3_hasher_finalize(&hasher, singleHash, BLAKE3_OUT_LEN);
    allHashData.insert(allHashData.end(), singleHash, singleHash + BLAKE3_OUT_LEN);
}

int glimmer::DataPack::LoadResourceByType(const std::string& dataType, const std::string& file,
                                          const std::string& content, const AppContext* appContext) const
{
    const std::vector<std::filesystem::path> searchPath = GetActuallyTemplateSearchPath(file);
    if (dataType == DATA_FILE_TYPE_TEMPLATE)
    {
        return 1;
    }
    std::string data = tomlTemplateExpander_->Expand(searchPath, content, virtualFileSystem_);
    const toml::value value = toml::parse_str(
        data, tomlVersion_);
    if (dataType == DATA_FILE_TYPE_TILE)
    {
        LoadTileResourceFromFile(value, appContext->GetTileResourceManager());
        return 1;
    }
    if (dataType == DATA_FILE_TYPE_BIOME)
    {
        LoadBiomeResourceFromFile(value, appContext->GetBiomesManager());
        return 1;
    }
    if (dataType == DATA_FILE_TYPE_COMPOSABLE_ITEM)
    {
        LoadComposableItemResourceFromFile(value, appContext->GetItemManager());
        return 1;
    }
    if (dataType == DATA_FILE_TYPE_ABILITY_ITEM)
    {
        LoadAbilityItemResourceFromFile(value, appContext->GetItemManager());
        return 1;
    }
    if (dataType == DATA_FILE_TYPE_MATERIAL_ITEM)
    {
        LoadMaterialItemResourceResourceFromFile(value, appContext->GetItemManager());
        return 1;
    }
    if (dataType == DATA_FILE_TYPE_LOOT_TABLE)
    {
        LoadLootTableResourceFromFile(value, appContext->GetLootTableManager());
        return 1;
    }
    if (dataType == DATA_FILE_TYPE_TREE_STRUCTURE)
    {
        LoadStructureResourceFromFile(value, appContext->GetStructureManager(), StructureGeneratorType::Tree);
        return 1;
    }
    if (dataType == DATA_FILE_TYPE_STATIC_STRUCTURE)
    {
        LoadStructureResourceFromFile(value, appContext->GetStructureManager(), StructureGeneratorType::Static);
        return 1;
    }
    if (dataType == DATA_FILE_TYPE_INITIAL_INVENTORY)
    {
        LoadInitialInventoryResourceFromFile(value, appContext->GetInitialInventoryManager());
        return 1;
    }
    if (dataType == DATA_FILE_TYPE_CONTRIBUTOR)
    {
        LoadContributorResourceFromFile(value, appContext->GetContributorManager());
        return 1;
    }
    if (dataType == DATA_FILE_TYPE_MOB)
    {
        LoadMobResourceFromFile(value, appContext->GetMobManager());
        return 1;
    }
    if (dataType == DATA_FILE_TYPE_SHAPE_CIRCLE)
    {
        LoadShapeResourceFromFile(value, appContext->GetShapeManager(), ShapeType::CIRCLE);
        return 1;
    }
    if (dataType == DATA_FILE_TYPE_SHAPE_RECTANGLE)
    {
        LoadShapeResourceFromFile(value, appContext->GetShapeManager(), ShapeType::RECTANGLE);
        return 1;
    }
    if (dataType == DATA_FILE_TYPE_SHAPE_ROUNDED_RECTANGLE)
    {
        LoadShapeResourceFromFile(value, appContext->GetShapeManager(), ShapeType::ROUNDED_RECTANGLE);
        return 1;
    }
    if (dataType == DATA_FILE_TYPE_DECORATOR_FILL)
    {
        LoadBiomeDecoratorResourceFromFile(value, appContext->GetBiomeDecoratorResourcesManager(),
                                           BiomeDecoratorType::FILL);
        return 1;
    }
    if (dataType == DATA_FILE_TYPE_DECORATOR_MINERAL)
    {
        LoadBiomeDecoratorResourceFromFile(value, appContext->GetBiomeDecoratorResourcesManager(),
                                           BiomeDecoratorType::MINERAL);
        return 1;
    }
    if (dataType == DATA_FILE_TYPE_DECORATOR_SURFACE)
    {
        LoadBiomeDecoratorResourceFromFile(value, appContext->GetBiomeDecoratorResourcesManager(),
                                           BiomeDecoratorType::SURFACE);
        return 1;
    }
    if (dataType == DATA_FILE_TYPE_FIXED_COLOR)
    {
        LoadFixedColorResourceFromFile(value, appContext->GetFixedColorManager());
        return 1;
    }
    if (dataType == DATA_FILE_TYPE_LIGHT_MASK)
    {
        LoadLightMaskResourceFromFile(value, appContext->GetLightMaskManager());
        return 1;
    }
    if (dataType == DATA_FILE_TYPE_LIGHT_SOURCE)
    {
        LoadLightSourceResourceFromFile(value, appContext->GetLightSourceManager());
        return 1;
    }
    if (dataType == DATA_FILE_TYPE_RECIPE)
    {
        LoadRecipeResourceFromFile(value, appContext->GetRecipeManager());
        return 1;
    }
    return 0;
}

int glimmer::DataPack::LoadLanguageFiles(const std::vector<std::filesystem::path>& defaultLanguageFiles,
                                         const std::vector<std::filesystem::path>& targetLanguageFiles,
                                         const AppContext* appContext) const
{
    int total = 0;
    const auto& filesToLoad = targetLanguageFiles.empty() ? defaultLanguageFiles : targetLanguageFiles;
    for (const auto& file : filesToLoad)
    {
        total += LoadStringResourceFromFile(file, appContext->GetStringManager());
    }
    return total;
}

glimmer::PackVerifyState glimmer::DataPack::VerifySignature(const bool findPublicKey, const bool findSignature,
                                                            const std::vector<uint8_t>& publicKey,
                                                            const std::vector<uint8_t>& signature,
                                                            const std::vector<uint8_t>& allHashData)
{
    if (!findPublicKey || !findSignature)
    {
        return VerifiedFailed;
    }
    if (crypto_ed25519_check(signature.data(), publicKey.data(),
                             allHashData.data(), allHashData.size()) == 0)
    {
        return VerifiedSuccess;
    }
    return VerifiedFailed;
}


bool glimmer::DataPack::ProcessSpecialFiles(const std::filesystem::path& path,
                                            SpecialFileProcessingParams& params) const
{
    if (!params.enableSignVerify)
    {
        return false;
    }
    if (!params.findPublicKey && path == params.publicPath)
    {
        ProcessPublicKeyFile(path, params);
        return true;
    }
    if (!params.findSignature && path == params.signPath)
    {
        ProcessSignatureFile(path, params);
        return true;
    }
    return false;
}

bool glimmer::DataPack::ProcessLanguageFile(const std::filesystem::path& file, std::string_view dataType,
                                            std::string_view fileName,
                                            std::vector<std::filesystem::path>& defaultLanguageFiles,
                                            std::vector<std::filesystem::path>& targetLanguageFiles,
                                            const AppContext* appContext)
{
    if (dataType != DATA_FILE_TYPE_STRINGS)
    {
        return false;
    }
    const auto langOptional = ExtractLanguageFromFileName(fileName);
    if (!langOptional.has_value())
    {
        return true;
    }
    if (const auto& fileLang = langOptional.value(); fileLang == appContext->GetLanguage())
    {
        targetLanguageFiles.push_back(file);
    }
    else if (fileLang == "default")
    {
        defaultLanguageFiles.push_back(file);
    }
    return true;
}

bool glimmer::DataPack::LoadPack(AppContext* appContext)
{
    packVerifyState_ = PackVerifyState::Unsigned;
    if (appContext == nullptr)
    {
        return false;
    }
    Config* config = appContext->GetConfig();
    if (config == nullptr)
    {
        return false;
    }
    int total = 0;
    std::vector<std::filesystem::path> files = virtualFileSystem_->ListFile(rootPath_, true);
    if (files.empty())
    {
        return false;
    }
    std::ranges::sort(files);
    std::vector<std::filesystem::path> defaultLanguageFiles;
    std::vector<std::filesystem::path> targetLanguageFiles;;
    std::vector<uint8_t> allHashData;
    SpecialFileProcessingParams specialFileProcessingParams{
        config->mods.enableSignVerify, rootPath_ / ".public", rootPath_ / ".sign", false, false,
        std::vector<uint8_t>(32), std::vector<uint8_t>(64)
    };
    for (const auto& file : files)
    {
        if (ProcessSpecialFiles(
            file, specialFileProcessingParams))
        {
            continue;
        }
        auto fileNameOptional = virtualFileSystem_->GetFileOrFolderName(file);
        if (!fileNameOptional.has_value())
        {
            continue;
        }
        const auto& fileName = fileNameOptional.value();
        if (!fileName.empty() && fileName[0] == '.')
        {
            continue;
        }

        auto istreamUniquePtr = virtualFileSystem_->ReadFileAsStream(file);
        if (istreamUniquePtr == nullptr)
        {
            continue;
        }
        auto stream = istreamUniquePtr.get();
        const std::optional<std::vector<char>> fileBufferOptional = ReadFileContent(stream);
        if (!fileBufferOptional.has_value())
        {
            continue;
        }
        const std::vector<char>& fileBuffer = fileBufferOptional.value();
        if (specialFileProcessingParams.enableSignVerify)
        {
            ComputeFileHash(fileBuffer, allHashData);
        }

        const auto dataTypeOptional = GetDataType(fileName);
        if (!dataTypeOptional.has_value())
        {
            continue;
        }
        const std::string content(fileBuffer.data(), fileBuffer.size());
        const auto& dataType = dataTypeOptional.value();
        if (ProcessLanguageFile(file, dataType, fileName, defaultLanguageFiles, targetLanguageFiles, appContext))
        {
            continue;
        }
        total += LoadResourceByType(dataType, file, content, appContext);
    }

    total += LoadLanguageFiles(defaultLanguageFiles, targetLanguageFiles, appContext);

    if (specialFileProcessingParams.enableSignVerify)
    {
        packVerifyState_ = VerifySignature(specialFileProcessingParams.findPublicKey,
                                           specialFileProcessingParams.findSignature,
                                           specialFileProcessingParams.publicKey, specialFileProcessingParams.signature,
                                           allHashData);
    }

    if (config->mods.loadOnlyVerified && packVerifyState_ != PackVerifyState::VerifiedSuccess)
    {
        LogCat::e("Signature verification FAILED for data pack:", manifest_.id);
        return false;
    }
    return total != 0;
}


const glimmer::DataPackManifest& glimmer::DataPack::GetManifest() const
{
    return manifest_;
}
