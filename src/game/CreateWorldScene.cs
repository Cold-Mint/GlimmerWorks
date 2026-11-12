using FontStashSharp.RichText;
using GlimmerWorks.Engine;
using GlimmerWorks.Engine.Scene;
using GlimmerWorks.Engine.widgets;
using Microsoft.Xna.Framework;
using Myra.Graphics2D;
using Myra.Graphics2D.Brushes;
using Myra.Graphics2D.UI;
using AppContext = GlimmerWorks.Engine.AppContext;
using Label = GlimmerWorks.Engine.widgets.Label;

namespace GlimmerWorks.Game;

public class CreateWorldScene(AppContext appContext) : Scene(appContext)
{
    private readonly InputText _worldNameBox = new(appContext.Theme)
    {
        Margin = new Thickness(16, 0, 16, 0),
        VerticalAlignment = VerticalAlignment.Center
    };

    private readonly InputText _seedBox = new(appContext.Theme)
    {
        Margin = new Thickness(16, 0, 16, 0),
        VerticalAlignment = VerticalAlignment.Center
    };

    private readonly CheckBox _allowCheatsCheck = new(appContext.Theme);

    public override void OnCreate()
    {
        base.OnCreate();
        var panel = new Panel
        {
            HorizontalAlignment = HorizontalAlignment.Stretch, VerticalAlignment = VerticalAlignment.Stretch,
        };
        var card = new Panel
        {
            HorizontalAlignment = HorizontalAlignment.Center, VerticalAlignment = VerticalAlignment.Center,
            MinWidth = 520, Background = new RoundedCornersSolidBrush(new Color(255, 255, 255)) { Radius = 16 },
            Padding = new Thickness(36, 28),
        };
        panel.Widgets.Add(card);
        var content = new VerticalStackPanel { Spacing = 18 };
        card.Widgets.Add(content);
        content.Widgets.Add(new Label(AppContext.Theme)
        {
            Text = AppContext.LocalizationManager.Get("ui.create_world")
        });
        _worldNameBox.Text = GenerateRandomName();
        content.Widgets.Add(CreateFormRow(
            AppContext.LocalizationManager.Get("ui.world_name"),
            _worldNameBox,
            () => _worldNameBox.Text = GenerateRandomName()));
        _seedBox.Text = GenerateRandomSeed();
        content.Widgets.Add(CreateFormRow(
            AppContext.LocalizationManager.Get("ui.seed"),
            _seedBox,
            () => _seedBox.Text = GenerateRandomSeed()));
        var row = new HorizontalStackPanel();
        row.Widgets.Add(_allowCheatsCheck);
        row.Widgets.Add(new Label(AppContext.Theme)
        {
            Text = AppContext.LocalizationManager.Get("ui.allow_cheating")
        });
        content.Widgets.Add(row);
        var buttonRow = new HorizontalStackPanel
        {
            HorizontalAlignment = HorizontalAlignment.Right, Spacing = 12, Margin = new Thickness(0, 12, 0, 0),
        };
        var backButton = new OutlinedButton(AppContext.Theme, AppContext.LocalizationManager.Get("ui.back"));
        backButton.Click += (_, _) => OnBackClick();
        var createButton = new FilledButton(AppContext.Theme, AppContext.LocalizationManager.Get("ui.create"));
        createButton.Click += (_, _) => OnCreateWorldClick();
        buttonRow.Widgets.Add(backButton);
        buttonRow.Widgets.Add(createButton);
        content.Widgets.Add(buttonRow);
        SetWidget(panel);
    }


    private HorizontalStackPanel CreateFormRow(string labelText, Widget input, Action onClick)
    {
        var row = new HorizontalStackPanel();
        row.Widgets.Add(new Label(AppContext.Theme)
        {
            Text = labelText,
            VerticalAlignment = VerticalAlignment.Center,
            MinWidth = 180,
            Font = AppContext.Theme.GetFont(ThemeFontKind.Small),
            TextAlign = TextHorizontalAlignment.Left
        });
        row.Widgets.Add(input);
        var randomButton = new OutlinedButton(AppContext.Theme, AppContext.LocalizationManager.Get("ui.random"))
        {
            VerticalAlignment = VerticalAlignment.Center
        };
        randomButton.Click += (_, _) => onClick();
        row.Widgets.Add(randomButton);
        return row;
    }


    private void OnBackClick()
    {
        AppContext.SceneStack.AddTask(new SceneStackTask { Type = StackType.Pop });
    }

    private void OnCreateWorldClick()
    {
    }

    private string PickRandom(string? value)
    {
        if (string.IsNullOrWhiteSpace(value))
        {
            return string.Empty;
        }

        var items = value.Split(',', StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries);
        if (items.Length == 0)
        {
            return string.Empty;
        }

        return items[Random.Shared.Next(items.Length)];
    }

    private string GenerateRandomName()
    {
        var prefix = AppContext.LocalizationManager.Get("ui.prefix");
        var suffix = AppContext.LocalizationManager.Get("ui.suffix");
        var prefixPick = PickRandom(prefix);
        var suffixPick = PickRandom(suffix);
        return string.Concat(prefixPick, suffixPick);
    }

    private string GenerateRandomSeed()
    {
        return Random.Shared.NextInt64().ToString();
    }
}