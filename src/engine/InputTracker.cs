using Microsoft.Xna.Framework.Input;

namespace GlimmerWorks.Engine;

public static class InputTracker
{
    private static KeyboardState _currentState;
    private static KeyboardState _previousState;

    public static void Update()
    {
        _previousState = _currentState;
        _currentState = Keyboard.GetState();
    }

    public static bool IsKeyDown(Keys key) => _currentState.IsKeyDown(key);

    public static bool IsKeyUp(Keys key) => _currentState.IsKeyUp(key);

    public static bool IsKeyJustPressed(Keys key) => _currentState.IsKeyDown(key) && _previousState.IsKeyUp(key);

    public static bool IsKeyJustReleased(Keys key) => _currentState.IsKeyUp(key) && _previousState.IsKeyDown(key);
}