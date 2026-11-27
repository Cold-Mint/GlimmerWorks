using GlimmerWorks.Engine;
using GlimmerWorks.Engine.Log;
using GlimmerWorks.Engine.Mods;
using GlimmerWorks.Engine.Vfs;
using GlimmerWorks.Game.Attributes;
using GlimmerWorks.Game.Graphics;
using GlimmerWorks.Game.Graphics.Pipeline;
using GlimmerWorks.Game.Graphics.Uniform;
using GlimmerWorks.Game.Graphics.Vertex;
using JetBrains.Annotations;

namespace GlimmerWorks.Game.Mods.assetsParser;

/// <summary>
/// <para>PipelineAssetsParser</para>
/// <para>管线资源解析器。</para>
/// </summary>
[UsedImplicitly]
[AssetsPaster("pipeline")]
public class PipelineAssetsParser : BaseAssetsParser<GraphicsPipeline>
{
    public override string GetFolderName()
    {
        return "pipelines";
    }

    public override string GetFileTypeName()
    {
        return "pipeline.toml";
    }

    public override GraphicsPipeline? Parser(GameAppContext appContext, FileEntry fileEntry, string requesterPackId)
    {
        var path = fileEntry.Path;
        if (path == null)
        {
            return null;
        }

        var definition = appContext.VirtualFileSystem.ReadToml<PipelineDefinition>(path, new PackContext
        {
            AppContext = appContext,
            RequesterPackId = requesterPackId
        });
        if (definition == null)
        {
            LogCat.Error($"Failed to read pipeline: {path.Path}");
            return null;
        }

        var vertexShader = ResolveShader(appContext, definition.VertexShader, ResourceType.VertShader,
            requesterPackId, "vertex_shader");
        var fragmentShader = ResolveShader(appContext, definition.FragmentShader, ResourceType.FragShader,
            requesterPackId, "fragment_shader");
        if (vertexShader == null || fragmentShader == null)
        {
            LogCat.Error($"Pipeline '{path.Path}' is missing a shader.");
            return null;
        }

        var pipeline = new GraphicsPipeline
        {
            BlendMode = (BlendMode)definition.BlendMode,
            VertexShader = vertexShader,
            FragmentShader = fragmentShader
        };

        if (definition.VertexLayout is { } layoutReference &&
            !string.IsNullOrWhiteSpace(layoutReference.ResourceId))
        {
            layoutReference.SetPackId(requesterPackId);
            layoutReference.ResourceType = ResourceType.VertexLayout;
            var layoutResult =
                appContext.AssetsPackManager.FindAsset<VertexLayout>(appContext, layoutReference);
            if (!layoutResult.Exist || layoutResult.Result == null)
            {
                LogCat.Error(
                    $"Pipeline '{path.Path}': failed to resolve the vertex layout '{layoutReference.ResourceId}'.");
                return null;
            }

            pipeline.VertexLayout = layoutResult.Result;
        }

        if (!ResolveUniformBlocks(appContext, definition.VertexUniformBlocks, requesterPackId,
                pipeline.VertexUniformBlocks))
        {
            return null;
        }

        if (!ResolveUniformBlocks(appContext, definition.FragmentUniformBlocks, requesterPackId,
                pipeline.FragmentUniformBlocks))
        {
            return null;
        }

        if (vertexShader.NumUniformBuffers != (uint)pipeline.VertexUniformBlocks.Count)
        {
            LogCat.Error(
                $"Pipeline '{path.Path}': the vertex shader declares {vertexShader.NumUniformBuffers} uniform buffer(s), " +
                $"but the pipeline configures {pipeline.VertexUniformBlocks.Count}.");
            return null;
        }

        if (fragmentShader.NumUniformBuffers != (uint)pipeline.FragmentUniformBlocks.Count)
        {
            LogCat.Error(
                $"Pipeline '{path.Path}': the fragment shader declares {fragmentShader.NumUniformBuffers} uniform buffer(s), " +
                $"but the pipeline configures {pipeline.FragmentUniformBlocks.Count}.");
            return null;
        }

        return pipeline;
    }

    private static CompiledShader? ResolveShader(GameAppContext appContext, ResourceRef? reference,
        string resourceType, string requesterPackId, string fieldName)
    {
        if (reference == null || string.IsNullOrWhiteSpace(reference.ResourceId))
        {
            LogCat.Error($"Pipeline field '{fieldName}' is missing.");
            return null;
        }

        reference.SetPackId(requesterPackId);
        // The shader type is implied by the field it is assigned to.
        // 着色器类型由它所在的字段决定。
        reference.ResourceType = resourceType;
        var result = appContext.AssetsPackManager.FindAsset<CompiledShader>(appContext, reference);
        if (!result.Exist || result.Result == null)
        {
            LogCat.Error($"Failed to resolve {resourceType}: {reference.ResourceId}");
            return null;
        }

        return result.Result;
    }

    private static bool ResolveUniformBlocks(GameAppContext appContext, List<ResourceRef> references,
        string requesterPackId, List<UniformBlock> target)
    {
        foreach (var reference in references)
        {
            reference.SetPackId(requesterPackId);
            reference.ResourceType = ResourceType.UniformBlock;
            var result = appContext.AssetsPackManager.FindAsset<UniformBlock>(appContext, reference);
            if (!result.Exist || result.Result == null)
            {
                LogCat.Error($"Failed to resolve uniform block: {reference.ResourceId}");
                return false;
            }

            target.Add(result.Result);
        }

        return true;
    }
}