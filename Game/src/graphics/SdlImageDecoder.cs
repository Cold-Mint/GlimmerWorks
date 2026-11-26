using System.Runtime.InteropServices;
using GlimmerWorks.Engine.Log;
using SDL3;

namespace GlimmerWorks.Game.Graphics;

/// <summary>
/// <para>
/// Decodes image files through SDL_image. The result is always converted to
/// 8-bit RGBA so it can be uploaded to an SDL GPU texture directly.
/// </para>
/// <para>
/// 通过 SDL_image 解码图片。结果统一转换为 8 位 RGBA，
/// 以便直接上传到 SDL GPU 纹理。
/// </para>
/// </summary>
internal static class SdlImageDecoder
{
    public static TextureData? DecodePng(byte[] bytes, string name)
    {
        if (bytes.Length == 0)
        {
            LogCat.Error($"Failed to decode PNG '{name}': the file is empty.");
            return null;
        }

        // SDL_image reads from the stream synchronously, so the source bytes only
        // need to stay pinned during the call.
        // SDL_image 会同步读取流，因此源字节只需要在调用期间保持固定。
        var bytesHandle = GCHandle.Alloc(bytes, GCHandleType.Pinned);
        var surface = nint.Zero;
        var rgbaSurface = nint.Zero;
        try
        {
            var data = bytesHandle.AddrOfPinnedObject();
            var ioStream = SDL.IOFromConstMem(data, (nuint)bytes.Length);
            if (ioStream == nint.Zero)
            {
                LogCat.Error($"Failed to decode PNG '{name}': {SDL.GetError()}");
                return null;
            }

            // IMG_LoadPNG_IO takes ownership of the stream and closes it before returning.
            // IMG_LoadPNG_IO 会接管流并在返回前关闭它。
            surface = Image.LoadPNGIO(ioStream);
            if (surface == nint.Zero)
            {
                LogCat.Error($"Failed to decode PNG '{name}': {SDL.GetError()}");
                return null;
            }

            // SDL_PIXELFORMAT_ABGR8888 has the byte order R,G,B,A on little-endian.
            // SDL_PIXELFORMAT_ABGR8888 在小端平台上按 R、G、B、A 排列字节。
            rgbaSurface = SDL.ConvertSurface(surface, SDL.PixelFormat.ABGR8888);
            if (rgbaSurface == nint.Zero)
            {
                LogCat.Error($"Failed to convert PNG '{name}' to RGBA: {SDL.GetError()}");
                return null;
            }

            var surfaceInfo = Marshal.PtrToStructure<SDL.Surface>(rgbaSurface);
            if (surfaceInfo.Width <= 0 || surfaceInfo.Height <= 0 ||
                surfaceInfo.Pixels == nint.Zero || surfaceInfo.Pitch < surfaceInfo.Width * 4)
            {
                LogCat.Error($"Failed to decode PNG '{name}': the surface layout is invalid.");
                return null;
            }

            var rowBytes = surfaceInfo.Width * 4;
            var pixels = new byte[rowBytes * surfaceInfo.Height];
            for (var y = 0; y < surfaceInfo.Height; y++)
            {
                var source = nint.Add(surfaceInfo.Pixels, y * surfaceInfo.Pitch);
                Marshal.Copy(source, pixels, y * rowBytes, rowBytes);
            }

            return new TextureData
            {
                Width = surfaceInfo.Width,
                Height = surfaceInfo.Height,
                Pixels = pixels,
            };
        }
        catch (Exception e)
        {
            LogCat.Error($"Failed to decode PNG '{name}': {e.Message}");
            return null;
        }
        finally
        {
            if (rgbaSurface != nint.Zero)
            {
                SDL.DestroySurface(rgbaSurface);
            }

            if (surface != nint.Zero)
            {
                SDL.DestroySurface(surface);
            }

            bytesHandle.Free();
        }
    }
}
