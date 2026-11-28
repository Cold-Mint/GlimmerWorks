using FontStashSharp;
using GlimmerWorks.Engine.Mods;

namespace GlimmerWorks.Game.Extensions;

public static class ColorExtensions
{
    public static Color ToColor(this FSColor fsColor)
    {
        return new Color(fsColor.R, fsColor.G, fsColor.B, fsColor.A);
    }

    public static FSColor ToFsColor(this Color color)
    {
        return new FSColor(color.R, color.G, color.B, color.A);
    }
}