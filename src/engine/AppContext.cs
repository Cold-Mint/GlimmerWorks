using System.Globalization;
using FontStashSharp;
using GlimmerWorks.Engine.Log;
using GlimmerWorks.Engine.Scene;
using GlimmerWorks.engine.Vfs;
using GlimmerWorks.Game;
using Microsoft.Xna.Framework;
using Myra;
using Myra.Events;
using Myra.Graphics2D;
using Myra.Graphics2D.Brushes;
using Myra.Graphics2D.UI;
using Myra.Graphics2D.UI.Styles;

namespace GlimmerWorks.Engine;

public class AppContext
{
    public LogCat LogCat { get; init; } = new();

    public GraphicsDeviceManager? GraphicsDeviceManager { get; set; }
    public LocalizationManager LocalizationManager { get; init; } = new();
    public GameConfig GameConfig { get; } = new();
    public VirtualFileSystem VirtualFileSystem { get; } = new();

    public DataRepository DataRepository { get; init; } = new();

    public SceneStack SceneStack { get; init; } = new();

    public Desktop? Desktop
    {
        get;
        set
        {
            field = value;
            ChangeStylesheet();
        }
    }

    public FontSystem FontSystem { get; init; } = new();

    public AppContext()
    {
        VirtualFileSystem.AddFileSystemProvider(new LocalFileSystemProvider(System.AppContext.BaseDirectory));
        // Cover the game configuration.
        // 覆盖游戏配置。
        var gameConfig = VirtualFileSystem.ReadToml<GameConfig>(new VirtualPath("assets", "config.toml"));
        if (gameConfig != null)
        {
            GameConfig = gameConfig;
        }

        // Use the configured language if set, otherwise follow the system UI language.
        // 配置了语言则使用配置语言，否则跟随系统 UI 语言。
        ChangeLanguage(string.IsNullOrEmpty(GameConfig.Language)
            ? CultureInfo.CurrentUICulture.Name
            : GameConfig.Language);
        LogCat.Localization = LocalizationManager;
    }

    /// <summary>
    /// <para>ChangeStylesheet</para>
    /// <para>改变样式</para>
    /// </summary>
    private void ChangeStylesheet()
    {
        var buttonStyle = Stylesheet.Current.ButtonStyle;
        var labelStyle = Stylesheet.Current.LabelStyle;

        var radius = 9;
        var borderThickness = new Thickness(1);
        var buttonPadding = new Thickness(16, 5);
        var minHeight = 34;

        labelStyle.TextColor = new Color(0, 0, 0);
        labelStyle.Font = FontSystem.GetFont(32);
        buttonStyle.MinHeight = minHeight;
        buttonStyle.Padding = buttonPadding;
        buttonStyle.BorderThickness = borderThickness;
        buttonStyle.Background = new RoundedCornersSolidBrush(new Color(255, 255, 255))
            { Radius = radius }; // 或 #ebebeb
        buttonStyle.Border = new RoundedCornersSolidBrush(new Color(190, 190, 190)) { Radius = radius };
        buttonStyle.OverBackground = new RoundedCornersSolidBrush(new Color(230, 230, 230)) { Radius = radius };
        buttonStyle.OverBorder = new RoundedCornersSolidBrush(new Color(170, 170, 170)) { Radius = radius };
        buttonStyle.PressedBackground = new RoundedCornersSolidBrush(new Color(205, 205, 205)) { Radius = radius };
        buttonStyle.PressedBorder = new RoundedCornersSolidBrush(new Color(150, 150, 150)) { Radius = radius };
        buttonStyle.FocusedBackground = new RoundedCornersSolidBrush(new Color(255, 255, 255)) { Radius = radius };
        buttonStyle.FocusedBorder = new RoundedCornersSolidBrush(new Color(53, 132, 228))
            { Radius = radius }; // #3584e4
        buttonStyle.DisabledBackground = new RoundedCornersSolidBrush(new Color(240, 240, 240)) { Radius = radius };
        buttonStyle.DisabledBorder = new RoundedCornersSolidBrush(new Color(210, 210, 210)) { Radius = radius };


        var accent = new Color(53, 132, 228); // #3584e4
        var suggested = new ButtonStyle
        {
            MinHeight = minHeight,
            Padding = buttonPadding,
            BorderThickness = borderThickness,
            Background = new RoundedCornersSolidBrush(accent) { Radius = radius },
            Border = new RoundedCornersSolidBrush(new Color(40, 110, 200)) { Radius = radius },
            OverBackground = new RoundedCornersSolidBrush(new Color(70, 145, 235)) { Radius = radius },
            OverBorder = new RoundedCornersSolidBrush(new Color(45, 120, 210)) { Radius = radius },
            PressedBackground = new RoundedCornersSolidBrush(new Color(40, 110, 200)) { Radius = radius },
            PressedBorder = new RoundedCornersSolidBrush(new Color(30, 90, 170)) { Radius = radius },
            FocusedBackground = new RoundedCornersSolidBrush(accent) { Radius = radius },
            FocusedBorder = new RoundedCornersSolidBrush(Color.White) { Radius = radius }
        };


        Stylesheet.Current.ButtonStyles["suggested"] = suggested;

        var destructiveRed = new Color(224, 27, 36);
        var destructive = new ButtonStyle
        {
            MinHeight = minHeight,
            Padding = buttonPadding,
            BorderThickness = borderThickness,
            Background = new RoundedCornersSolidBrush(destructiveRed) { Radius = radius },
            Border = new RoundedCornersSolidBrush(new Color(190, 20, 30)) { Radius = radius },
            OverBackground = new RoundedCornersSolidBrush(new Color(240, 50, 60)) { Radius = radius },
            OverBorder = new RoundedCornersSolidBrush(new Color(200, 25, 35)) { Radius = radius },
            PressedBackground = new RoundedCornersSolidBrush(new Color(190, 20, 30)) { Radius = radius },
            PressedBorder = new RoundedCornersSolidBrush(new Color(160, 15, 25)) { Radius = radius },
            FocusedBackground = new RoundedCornersSolidBrush(destructiveRed) { Radius = radius },
            FocusedBorder = new RoundedCornersSolidBrush(Color.White) { Radius = radius }
        };

        Stylesheet.Current.ButtonStyles["destructive"] = destructive;
    }

    /// <summary>
    /// <para>ChangeLanguage</para>
    /// <para>改变语言</para>
    /// </summary>
    /// <param name="language"></param>
    private void ChangeLanguage(string language)
    {
        FontSystem.Reset();
        LocalizationManager.Clear();
        var candidates = LocalizationManager.GetCandidates(language);
        bool loadFont = false;
        bool loadLangs = false;
        foreach (var candidate in candidates)
        {
            var langsRoot = new VirtualPath("assets", "langs", candidate);
            if (!loadLangs && VirtualFileSystem.DirectoryExists(langsRoot))
            {
                if (LocalizationManager.LoadDirectory(LogCat, VirtualFileSystem, langsRoot))
                {
                    loadLangs = true;
                }
            }

            var ttfVirtualPath = new VirtualPath("assets", "fonts", $"{candidate}.ttf");
            if (!loadFont && VirtualFileSystem.FileExists(ttfVirtualPath))
            {
                using var stream = VirtualFileSystem.OpenRead(ttfVirtualPath);
                if (stream == null)
                {
                    continue;
                }

                FontSystem.AddFont(stream);
                loadFont = true;
            }

            if (loadFont && loadLangs)
            {
                break;
            }
        }
    }
}