using System.Drawing;
using GlimmerWorks.Engine.Log;
using Myra.Graphics2D.UI;
using Myra.Platform;
using SDL3;
using MyraKeys = Myra.Platform.Keys;

namespace GlimmerWorks.Game.Ui;

/// <summary>
/// <para>Myra platform implementation that supplies window, mouse and keyboard state from SDL3.</para>
/// <para>基于 SDL3 的 Myra 平台实现，提供窗口尺寸、鼠标与键盘状态。</para>
/// </summary>
internal sealed class Sdl3UiPlatform(IntPtr window, Sdl3UiRenderer renderer) : IMyraPlatform
{
    private static readonly (SDL.Scancode Scancode, MyraKeys Key)[] KeyMap =
    [
        (SDL.Scancode.A, MyraKeys.A), (SDL.Scancode.B, MyraKeys.B), (SDL.Scancode.C, MyraKeys.C),
        (SDL.Scancode.D, MyraKeys.D), (SDL.Scancode.E, MyraKeys.E), (SDL.Scancode.F, MyraKeys.F),
        (SDL.Scancode.G, MyraKeys.G), (SDL.Scancode.H, MyraKeys.H), (SDL.Scancode.I, MyraKeys.I),
        (SDL.Scancode.J, MyraKeys.J), (SDL.Scancode.K, MyraKeys.K), (SDL.Scancode.L, MyraKeys.L),
        (SDL.Scancode.M, MyraKeys.M), (SDL.Scancode.N, MyraKeys.N), (SDL.Scancode.O, MyraKeys.O),
        (SDL.Scancode.P, MyraKeys.P), (SDL.Scancode.Q, MyraKeys.Q), (SDL.Scancode.R, MyraKeys.R),
        (SDL.Scancode.S, MyraKeys.S), (SDL.Scancode.T, MyraKeys.T), (SDL.Scancode.U, MyraKeys.U),
        (SDL.Scancode.V, MyraKeys.V), (SDL.Scancode.W, MyraKeys.W), (SDL.Scancode.X, MyraKeys.X),
        (SDL.Scancode.Y, MyraKeys.Y), (SDL.Scancode.Z, MyraKeys.Z),

        (SDL.Scancode.Alpha0, MyraKeys.D0), (SDL.Scancode.Alpha1, MyraKeys.D1),
        (SDL.Scancode.Alpha2, MyraKeys.D2), (SDL.Scancode.Alpha3, MyraKeys.D3),
        (SDL.Scancode.Alpha4, MyraKeys.D4), (SDL.Scancode.Alpha5, MyraKeys.D5),
        (SDL.Scancode.Alpha6, MyraKeys.D6), (SDL.Scancode.Alpha7, MyraKeys.D7),
        (SDL.Scancode.Alpha8, MyraKeys.D8), (SDL.Scancode.Alpha9, MyraKeys.D9),

        (SDL.Scancode.Return, MyraKeys.Enter), (SDL.Scancode.KpEnter, MyraKeys.Enter),
        (SDL.Scancode.Escape, MyraKeys.Escape), (SDL.Scancode.Backspace, MyraKeys.Back),
        (SDL.Scancode.Tab, MyraKeys.Tab), (SDL.Scancode.Space, MyraKeys.Space),

        (SDL.Scancode.Minus, MyraKeys.OemMinus), (SDL.Scancode.Equals, MyraKeys.OemPlus),
        (SDL.Scancode.Leftbracket, MyraKeys.OemOpenBrackets),
        (SDL.Scancode.Rightbracket, MyraKeys.OemCloseBrackets),
        (SDL.Scancode.Backslash, MyraKeys.OemPipe),
        (SDL.Scancode.NonUsBackSlash, MyraKeys.OemBackslash),
        (SDL.Scancode.Semicolon, MyraKeys.OemSemicolon),
        (SDL.Scancode.Apostrophe, MyraKeys.OemQuotes), (SDL.Scancode.Grave, MyraKeys.OemTilde),
        (SDL.Scancode.Comma, MyraKeys.OemComma), (SDL.Scancode.Period, MyraKeys.OemPeriod),
        (SDL.Scancode.Slash, MyraKeys.OemQuestion),

        (SDL.Scancode.Capslock, MyraKeys.CapsLock), (SDL.Scancode.Printscreen, MyraKeys.PrintScreen),
        (SDL.Scancode.Scrolllock, MyraKeys.Scroll), (SDL.Scancode.Pause, MyraKeys.Pause),
        (SDL.Scancode.Insert, MyraKeys.Insert), (SDL.Scancode.Home, MyraKeys.Home),
        (SDL.Scancode.Pageup, MyraKeys.PageUp), (SDL.Scancode.Delete, MyraKeys.Delete),
        (SDL.Scancode.End, MyraKeys.End), (SDL.Scancode.Pagedown, MyraKeys.PageDown),
        (SDL.Scancode.Right, MyraKeys.Right), (SDL.Scancode.Left, MyraKeys.Left),
        (SDL.Scancode.Down, MyraKeys.Down), (SDL.Scancode.Up, MyraKeys.Up),

        (SDL.Scancode.F1, MyraKeys.F1), (SDL.Scancode.F2, MyraKeys.F2), (SDL.Scancode.F3, MyraKeys.F3),
        (SDL.Scancode.F4, MyraKeys.F4), (SDL.Scancode.F5, MyraKeys.F5), (SDL.Scancode.F6, MyraKeys.F6),
        (SDL.Scancode.F7, MyraKeys.F7), (SDL.Scancode.F8, MyraKeys.F8), (SDL.Scancode.F9, MyraKeys.F9),
        (SDL.Scancode.F10, MyraKeys.F10), (SDL.Scancode.F11, MyraKeys.F11), (SDL.Scancode.F12, MyraKeys.F12),

        (SDL.Scancode.NumLockClear, MyraKeys.NumLock), (SDL.Scancode.KpDivide, MyraKeys.Divide),
        (SDL.Scancode.KpMultiply, MyraKeys.Multiply), (SDL.Scancode.KpMinus, MyraKeys.Subtract),
        (SDL.Scancode.KpPlus, MyraKeys.Add), (SDL.Scancode.KpPeriod, MyraKeys.Decimal),
        (SDL.Scancode.Kp0, MyraKeys.NumPad0), (SDL.Scancode.Kp1, MyraKeys.NumPad1),
        (SDL.Scancode.Kp2, MyraKeys.NumPad2), (SDL.Scancode.Kp3, MyraKeys.NumPad3),
        (SDL.Scancode.Kp4, MyraKeys.NumPad4), (SDL.Scancode.Kp5, MyraKeys.NumPad5),
        (SDL.Scancode.Kp6, MyraKeys.NumPad6), (SDL.Scancode.Kp7, MyraKeys.NumPad7),
        (SDL.Scancode.Kp8, MyraKeys.NumPad8), (SDL.Scancode.Kp9, MyraKeys.NumPad9),

        (SDL.Scancode.LCtrl, MyraKeys.LeftControl), (SDL.Scancode.RCtrl, MyraKeys.RightControl),
        (SDL.Scancode.LShift, MyraKeys.LeftShift), (SDL.Scancode.RShift, MyraKeys.RightShift),
        (SDL.Scancode.LAlt, MyraKeys.LeftAlt), (SDL.Scancode.RAlt, MyraKeys.RightAlt),
        (SDL.Scancode.LGUI, MyraKeys.LeftWindows), (SDL.Scancode.RGUI, MyraKeys.RightWindows),
        (SDL.Scancode.Application, MyraKeys.Apps), (SDL.Scancode.Sleep, MyraKeys.Sleep)
    ];

    private static readonly Dictionary<MouseCursorType, SDL.SystemCursor> CursorMap = new()
    {
        [MouseCursorType.Arrow] = SDL.SystemCursor.Default,
        [MouseCursorType.IBeam] = SDL.SystemCursor.Text,
        [MouseCursorType.Wait] = SDL.SystemCursor.Wait,
        [MouseCursorType.Crosshair] = SDL.SystemCursor.Crosshair,
        [MouseCursorType.WaitArrow] = SDL.SystemCursor.Progress,
        [MouseCursorType.SizeNWSE] = SDL.SystemCursor.NWSEResize,
        [MouseCursorType.SizeNESW] = SDL.SystemCursor.NESWResize,
        [MouseCursorType.SizeWE] = SDL.SystemCursor.EWResize,
        [MouseCursorType.SizeNS] = SDL.SystemCursor.NSResize,
        [MouseCursorType.SizeAll] = SDL.SystemCursor.Move,
        [MouseCursorType.No] = SDL.SystemCursor.NotAllowed,
        [MouseCursorType.Hand] = SDL.SystemCursor.Pointer
    };

    private readonly Dictionary<MouseCursorType, IntPtr> _cursors = [];

    private float _mouseWheel;

    public Point ViewSize
    {
        get
        {
            if (!SDL.GetWindowSizeInPixels(window, out var width, out var height) || width <= 0 || height <= 0)
            {
                if (!SDL.GetWindowSize(window, out width, out height))
                {
                    return new Point(1, 1);
                }
            }

            return new Point(Math.Max(1, width), Math.Max(1, height));
        }
    }

    public IMyraRenderer Renderer => renderer;

    public MouseInfo GetMouseInfo()
    {
        var buttons = SDL.GetGlobalMouseState(out var globalX, out var globalY);

        var windowX = globalX;
        var windowY = globalY;
        if (SDL.GetWindowPosition(window, out var windowPositionX, out var windowPositionY))
        {
            windowX -= windowPositionX;
            windowY -= windowPositionY;
        }

        var viewSize = ViewSize;
        var position = new Point((int)windowX, (int)windowY);
        if (SDL.GetWindowSize(window, out var windowWidth, out var windowHeight) &&
            windowWidth > 0 && windowHeight > 0 &&
            (windowWidth != viewSize.X || windowHeight != viewSize.Y))
        {
            position.X = (int)(windowX * viewSize.X / windowWidth);
            position.Y = (int)(windowY * viewSize.Y / windowHeight);
        }

        return new MouseInfo
        {
            Position = position,
            IsLeftButtonDown = (buttons & SDL.MouseButtonFlags.Left) != 0,
            IsMiddleButtonDown = (buttons & SDL.MouseButtonFlags.Middle) != 0,
            IsRightButtonDown = (buttons & SDL.MouseButtonFlags.Right) != 0,
            Wheel = _mouseWheel
        };
    }

    public void SetKeysDown(bool[] keys)
    {
        Array.Clear(keys);

        var state = SDL.GetKeyboardState(out var numKeys);
        foreach (var (scancode, key) in KeyMap)
        {
            var scancodeIndex = (int)scancode;
            var keyIndex = (int)key;
            if (scancodeIndex < numKeys && scancodeIndex < state.Length &&
                keyIndex >= 0 && keyIndex < keys.Length && state[scancodeIndex])
            {
                keys[keyIndex] = true;
            }
        }
    }

    public void SetMouseCursorType(MouseCursorType mouseCursorType)
    {
        if (!_cursors.TryGetValue(mouseCursorType, out var cursor))
        {
            if (!CursorMap.TryGetValue(mouseCursorType, out var systemCursor))
            {
                return;
            }

            cursor = SDL.CreateSystemCursor(systemCursor);
            if (cursor == IntPtr.Zero)
            {
                return;
            }

            _cursors[mouseCursorType] = cursor;
        }

        if (!SDL.SetCursor(cursor))
        {
            LogCat.Debug($"Failed to set the mouse cursor: {SDL.GetError()}");
        }
    }

    public TouchCollection GetTouchState() => TouchCollection.Empty;

    /// <summary>
    /// <para>Accumulates a mouse wheel delta. Myra expects the total wheel value, not a per-frame delta.</para>
    /// <para>累计鼠标滚轮值。Myra 期望的是累计值，而不是单帧增量。</para>
    /// </summary>
    public void AddWheel(float delta)
    {
        _mouseWheel += delta;
    }
}
