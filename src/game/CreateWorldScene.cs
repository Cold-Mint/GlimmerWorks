using FontStashSharp.RichText;
using GlimmerWorks.Engine;
using GlimmerWorks.Engine.Mods.Data;
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
    private DimensionResources? _dimensionResource;

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

    private readonly FilledButton
        _createButton = new(appContext.Theme, appContext.Localizer["Create world"].Value);

    private readonly CheckBox _allowCheatsCheck = new(appContext.Theme);

    public override void OnCreate()
    {
        base.OnCreate();
        _dimensionResource = AppContext.DataRepository.FirstOrDefaultResources<DimensionResources>(Filter);
        if (_dimensionResource == null)
        {
            _createButton.Enabled = false;
            return;
        }

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
            Text = AppContext.Localizer["Create world"].Value
        });
        _worldNameBox.Text = GenerateRandomName();
        content.Widgets.Add(CreateFormRow(
            AppContext.Localizer["World name"].Value,
            _worldNameBox,
            () => _worldNameBox.Text = GenerateRandomName()));
        _seedBox.Text = GenerateRandomSeed();
        content.Widgets.Add(CreateFormRow(
            AppContext.Localizer["Seed"].Value,
            _seedBox,
            () => _seedBox.Text = GenerateRandomSeed()));
        var horizontalStackPanel = new HorizontalStackPanel();
        var switchDimensionButton =
            new OutlinedButton(AppContext.Theme, AppContext.Localizer["Switch"].Value);
        var switchDimensionLabel = new Label(AppContext.Theme)
        {
            Text = AppContext.Localizer["Starting dimension:{0}",
                $"{_dimensionResource.PackId}:{_dimensionResource.ResourceId}"].Value
        };
        horizontalStackPanel.Widgets.Add(switchDimensionLabel);
        horizontalStackPanel.Widgets.Add(switchDimensionButton);
        content.Widgets.Add(horizontalStackPanel);
        var row = new HorizontalStackPanel();
        row.Widgets.Add(_allowCheatsCheck);
        row.Widgets.Add(new Label(AppContext.Theme)
        {
            Text = AppContext.Localizer["Allow Cheating"].Value
        });
        content.Widgets.Add(row);
        var buttonRow = new HorizontalStackPanel
        {
            HorizontalAlignment = HorizontalAlignment.Right, Spacing = 12, Margin = new Thickness(0, 12, 0, 0),
        };
        var backButton = new OutlinedButton(AppContext.Theme, AppContext.Localizer["Back"].Value);
        backButton.Click += (_, _) => OnBackClick();
        _createButton.Click += (_, _) => OnCreateWorldClick();
        buttonRow.Widgets.Add(backButton);
        buttonRow.Widgets.Add(_createButton);
        content.Widgets.Add(buttonRow);
        SetWidget(panel);
    }

    /// <summary>
    /// <para>DimensionResources</para>
    /// <para>维度过滤器</para>
    /// </summary>
    /// <param name="dimensionResource"></param>
    /// <returns></returns>
    private bool Filter(DimensionResources dimensionResource)
    {
        return dimensionResource.AllowAsStarting;
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
        var randomButton = new OutlinedButton(AppContext.Theme, AppContext.Localizer["Random"].Value)
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

        return items[Random.Shared.Next(items.Length)].Trim();
    }

    private string GenerateRandomName()
    {
        var prefix = AppContext.Localizer[
                "Barren, Arid, Desolate, Abandoned, Ruined, Shattered, Collapsed, Scarred, Wounded, Withered, Wilted, Sparse, Hollow, Jagged, Erased, Verdant, Fertile, Lush, Luxuriant, Frozen, Wintry, Frigid, Frostbound, Blazing, Scorched, Sultry, Parched, Damp, Misty, Hazy, Steaming, Dank, Gloomy, Deep, Dusky, Glimmering, Dim, Murky, Radiant, Silvered, Pale, Ancient, Primordial, Final, Ageless, Eternal, Evernight, Dawn, Dusk, Twilight, Fleeting, Dreary, Lonely, Mournful, Serene, Blurred, Dazed, Solitary, Silent, Mad, Cursed, Hidden, Secret, Sacred, Blessed, Nameless, Untamed, Legendary, Enigmatic, Sunken, Exiled, Fallen, Wandering, Weathered, Burning, Drifting, Toppled, Whirling, Surging, Still, Dying, Fading, Feral, Wild, Savage, Vicious, Defiant, Golden, Silver, Iron, Amber, Glazed, Indigo, Rusted, Gilded, Black, Slate, Distant, Remote, Endless, Lost, Forgotten, Forsaken, Eclipsed, Starlit, Stormy, Windswept, Foggy, Charred, Ashen"]
            .Value;
        var suffix = AppContext.Localizer[
                "Canyon, Peaks, Mountains, Valley, Ridge, Cliff, Crag, Highlands, Glen, Snowpeak, Volcano, Bluff, Pinnacles, Mesa, Cave, Grotto, Cavern, Frosthollow, Rift, Bay, Beach, Coast, Shore, Cove, Falls, Fjord, Reef, Lake, Lagoon, Springs, Saltflat, Brook, Underflow, Understream, Archipelago, Island, Isles, Peninsula, Forest, Weald, Vinewood, Deepwood, Bamboo, Plains, Wastes, Wilds, Mire, Briars, Desert, Dunes, Expanse, Crater, Delta, Glacier, Frostland, Snowfield, Tundra, Basin, Vale, Ruins, Relics, Remnants, Watchtower, Lighthouse, Fortress, Watchspire, Waystation, Docks, Camp, Altar, Graveyard, Monoliths, Cloister, Belfry, Colony, Outpost, Landing, Settlement, Haven, Refuge, Tribe, Village, Stonecircle, Oldtown, Gate, Beacon, Mine, Granary, Starfield, Gloaming, Aurora, Mirage, Phantasm, Echovale, Hushwood, Mistvale, Abyss, Skyisle, Stormeye, Cloudsea, Tidebelt, Whispervale"]
            .Value;
        var prefixPick = PickRandom(prefix);
        var suffixPick = PickRandom(suffix);
        return string.Concat(prefixPick, suffixPick);
    }

    private string GenerateRandomSeed()
    {
        return Random.Shared.NextInt64().ToString();
    }
}