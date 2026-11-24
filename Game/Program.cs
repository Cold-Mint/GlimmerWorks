namespace GlimmerWorks.Game;

internal static class Program
{
    [STAThread]
    private static void Main()
    {
        var game = new GlimmerGame();
        game.Start();
    }
}