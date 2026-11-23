using System.Text;

namespace GlimmerWorks.Game;

public static class Program
{
    public static void Main()
    {
        Console.OutputEncoding = Encoding.UTF8;
        var glimmerGame = new GlimmerGame();
        glimmerGame.Run();
    }
}