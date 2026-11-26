using GlimmerWorks.Engine.Mods;
using GlimmerWorks.Game.Widgets;
using Myra.Events;
using Myra.Graphics2D.UI;
using Label = GlimmerWorks.Game.Widgets.Label;
using ListView = GlimmerWorks.Game.Widgets.ListView;

namespace GlimmerWorks.Game.Scene;

public class SelectResourceScene<T>(GameAppContext appContext)
    : BaseScene(appContext) where T : Resource
{
    private readonly InputText _inputText = new(appContext.Theme);
    private readonly VerticalStackPanel _verticalStackPanel = new();

    private readonly FilledButton _confirmButton =
        new(appContext.Theme, appContext.LocalizationManager["Confirm"].Value)
        {
            Visible = false,
            HorizontalAlignment = HorizontalAlignment.Center
        };

    private T? _selectedResource;
    private T? _result;
    private readonly List<T> _resourcesList = [];

    private readonly Label _cannotFindResourceLabel = new(appContext.Theme)
    {
        Text = appContext.Localizer["No matching resource found"].Value,
    };

    private readonly ListView _listView = new(appContext.Theme)
    {
        HorizontalAlignment = HorizontalAlignment.Stretch,
        VerticalAlignment = VerticalAlignment.Stretch,
    };

    private string _keyString = string.Empty;

    public override object? Result()
    {
        return _result;
    }

    public override void OnCreate()
    {
        base.OnCreate();
        _listView.SelectedIndexChanged += ListViewOnSelectedIndexChanged;
        _verticalStackPanel.Widgets.Add(_inputText);
        _verticalStackPanel.Widgets.Add(_listView);
        _verticalStackPanel.Widgets.Add(_cannotFindResourceLabel);
        _verticalStackPanel.Widgets.Add(_confirmButton);
        _confirmButton.Click += ConfirmButtonOnClick;
        UpdateList();
        SetWidget(_verticalStackPanel);
    }

    private void ConfirmButtonOnClick(object sender, MyraEventArgs e)
    {
        _result = _selectedResource;
        AppContext.SceneStack.AddTask(new SceneStackTask() { Type = StackType.Pop });
    }

    private void ListViewOnSelectedIndexChanged(object sender, MyraEventArgs e)
    {
        if (_listView.SelectedIndex is { } index and >= 0 &&
            index < _resourcesList.Count)
        {
            _selectedResource = _resourcesList[index];
            _confirmButton.Visible = true;
        }
        else
        {
            _selectedResource = null;
            _confirmButton.Visible = false;
        }
    }

    private bool Filter(T resource)
    {
        if (string.IsNullOrEmpty(_keyString))
        {
            return true;
        }

        return resource.Filter(AppContext, _keyString);
    }

    private Widget GetListItem(T resource)
    {
        VerticalStackPanel verticalStackPanel = new();
        verticalStackPanel.Widgets.Add(new Label(theme: AppContext.Theme)
        {
            Text = resource.GetDisplayName(AppContext)
        });
        verticalStackPanel.Widgets.Add(new Label(theme: AppContext.Theme)
        {
            Text = $"{resource.PackId}:{resource.ResourceId}",
            Font = AppContext.Theme.GetFont(ThemeFontKind.Small)
        });
        return verticalStackPanel;
    }


    private void UpdateList()
    {
        var resourcesList = AppContext.DataRepository.GetResourceList<T>(Filter);
        var count = 0;
        _listView.Widgets.Clear();
        _resourcesList.Clear();
        foreach (var resource in resourcesList)
        {
            count++;
            _listView.Widgets.Add(GetListItem(resource));
            _resourcesList.Add(resource);
        }

        _listView.Visible = count > 0;
        _cannotFindResourceLabel.Visible = count == 0;
    }


    public override void OnTick(long deltaMs, long globalTick)
    {
        if (_inputText.Text != _keyString)
        {
            _keyString = _inputText.Text;
            UpdateList();
        }
    }
}