using FontStashSharp;
using FontStashSharp.RichText;
using GlimmerWorks.Engine.Mods.Resources;
using GlimmerWorks.Game.Ui.Theme;
using GlimmerWorks.Game.Widgets;
using Myra.Events;
using Myra.Graphics2D;
using Myra.Graphics2D.Brushes;
using Myra.Graphics2D.UI;
using Label = GlimmerWorks.Game.Widgets.Label;

namespace GlimmerWorks.Game.Scene;

public class CreateWorldScene(GameAppContext appContext) : BaseScene(appContext)
{
    private readonly int _switchDimensionRequestCode = 1;
    private readonly Label _switchDimensionLabel = new(appContext.Theme);
    private DimensionResource? _startingDimension;

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
        var dimensionResource =
            AppContext.DataRepository.FirstOrDefaultResources<DimensionResource>(Filter);
        if (dimensionResource == null)
        {
            _createButton.Enabled = false;
            return;
        }

        _startingDimension = dimensionResource;

        var panel = new Panel
        {
            HorizontalAlignment = HorizontalAlignment.Stretch, VerticalAlignment = VerticalAlignment.Stretch,
        };
        var card = new Panel
        {
            HorizontalAlignment = HorizontalAlignment.Center, VerticalAlignment = VerticalAlignment.Center,
            MinWidth = 520, Background = new RoundedCornersSolidBrush(new FSColor(255, 255, 255)) { Radius = 16 },
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
        switchDimensionButton.Click += SwitchDimensionButtonOnClick;
        _switchDimensionLabel.Text = AppContext.Localizer["Starting dimension:{0}",
            dimensionResource.GetDisplayName(AppContext)].Value;
        horizontalStackPanel.Widgets.Add(_switchDimensionLabel);
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
    /// <para>SwitchDimensionButtonOnClick</para>
    /// <para>当切换维度按钮被点击时</para>
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void SwitchDimensionButtonOnClick(object sender, MyraEventArgs e)
    {
        AppContext.SceneStack.AddTask(
            new SceneStackTask
            {
                Type = StackType.Push, Scene = new SelectResourceScene<DimensionResource>(AppContext),
                RequestCode = _switchDimensionRequestCode
            });
    }

    public override void OnResult(int requestCode, object? result)
    {
        base.OnResult(requestCode, result);
        if (result == null)
        {
            return;
        }

        if (requestCode == _switchDimensionRequestCode && result is DimensionResource dimensionResource)
        {
            _startingDimension = dimensionResource;
            _switchDimensionLabel.Text = AppContext.Localizer["Starting dimension:{0}",
                dimensionResource.GetDisplayName(AppContext)].Value;
        }
    }

    /// <summary>
    /// <para>DimensionResources</para>
    /// <para>维度过滤器</para>
    /// </summary>
    /// <param name="dimensionResource">
    ///<para>dimensionResource</para>
    ///<para>维度资源</para>
    /// </param>
    /// <returns></returns>
    private static bool Filter(DimensionResource dimensionResource)
    {
        return dimensionResource.AllowAsStarting;
    }

    /// <summary>
    /// <para>CreateFormRow</para>
    /// <para>创建一行表单</para>
    /// </summary>
    /// <param name="labelText">
    ///<para>labelText</para>
    ///<para>标签文本</para>
    /// </param>
    /// <param name="input">
    ///<para>input</para>
    ///<para>输入框</para>
    /// </param>
    /// <param name="onClick">
    ///<para>onClick</para>
    ///<para>当点击右侧按钮时</para>
    /// </param>
    /// <returns></returns>
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


    /// <summary>
    /// <para>OnBackClick</para>
    /// <para>当返回按钮被按下时</para>
    /// </summary>
    private void OnBackClick()
    {
        AppContext.SceneStack.AddTask(new SceneStackTask { Type = StackType.Pop });
    }

    /// <summary>
    /// <para>OnCreateWorldClick</para>
    /// <para>点击创建世界</para>
    /// </summary>
    private void OnCreateWorldClick()
    {
        if (_startingDimension == null)
        {
            return;
        }

        AppContext.SceneStack.AddTask(
            new SceneStackTask
            {
                Type = StackType.Replace, Scene = new GameScene(AppContext),
            });
    }

    /// <summary>
    /// <para>PickRandom</para>
    /// <para>随机选择一个字符串</para>
    /// </summary>
    /// <param name="value"></param>
    /// <returns></returns>
    private static string? PickRandom(string? value)
    {
        if (string.IsNullOrWhiteSpace(value))
        {
            return null;
        }

        var items = value.Split(',', StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries);
        if (items.Length == 0)
        {
            return null;
        }

        return items[Random.Shared.Next(items.Length)].Trim();
    }

    /// <summary>
    /// <para>GenerateRandomName</para>
    /// <para>生成随机的名称</para>
    /// </summary>
    /// <returns></returns>
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

    /// <summary>
    /// <para>GenerateRandomSeed</para>
    /// <para>生成随机种子</para>
    /// </summary>
    /// <returns></returns>
    private static string GenerateRandomSeed()
    {
        return Random.Shared.NextInt64().ToString();
    }
}