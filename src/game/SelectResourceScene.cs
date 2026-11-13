using GlimmerWorks.Engine.Mods.Data;
using GlimmerWorks.Engine.Scene;
using GlimmerWorks.Engine.widgets;
using Microsoft.Xna.Framework;
using Myra.Graphics2D.UI;
using AppContext = GlimmerWorks.Engine.AppContext;
using Label = GlimmerWorks.Engine.widgets.Label;
using ListView = GlimmerWorks.Engine.widgets.ListView;

namespace GlimmerWorks.Game;

public class SelectResourceScene<T>(AppContext appContext)
    : Scene(appContext) where T : Resources
{
    private readonly InputText _inputText = new(appContext.Theme);
    private readonly VerticalStackPanel _verticalStackPanel = new();

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

    public override void OnCreate()
    {
        base.OnCreate();
        _verticalStackPanel.Widgets.Add(_inputText);
        _verticalStackPanel.Widgets.Add(_listView);
        _verticalStackPanel.Widgets.Add(_cannotFindResourceLabel);
        UpdateList();
        SetWidget(_verticalStackPanel);
    }

    private bool Filter(T resource)
    {
        if (string.IsNullOrEmpty(_keyString))
        {
            return true;
        }

        return resource.PackId.Contains(_keyString) || resource.ResourceId.Contains(_keyString);
    }

    private Widget GetListItem(T resource)
    {
        return new Label(theme: AppContext.Theme)
            { Text = $"{resource.PackId}/{resource.ResourceId}" };
    }


    private void UpdateList()
    {
        var resourcesList = AppContext.DataRepository.GetResourceList<T>(Filter);
        var count = 0;
        _listView.Widgets.Clear();
        foreach (var resource in resourcesList)
        {
            count++;
            _listView.Widgets.Add(GetListItem(resource));
        }

        _listView.Visible = count > 0;
        _cannotFindResourceLabel.Visible = count == 0;
    }


    public override void Update(GameTime gameTime)
    {
        base.Update(gameTime);
        if (_inputText.Text != _keyString)
        {
            _keyString = _inputText.Text;
            UpdateList();
        }
    }
}