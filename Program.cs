using System.Text;
using GlimmerWorks.Game;

namespace GlimmerWorks;

public static class Program
{
    public static void Main()
    {
        Console.OutputEncoding = Encoding.UTF8;
        var glimmerGame = new GlimmerGame();
        glimmerGame.Run();
    }
}