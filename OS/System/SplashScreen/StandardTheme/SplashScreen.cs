using System;
using System.Threading;
using OS.System.AboutSystem;

namespace OS.System.SplashScreen.StandardTheme
{
    public enum SplashMode
    {
        None,
        Loading,
        ShuttingDown,
        Rebooting
    }

    public static class SplashScreen
    {
        private static int step = 0;
        private static int delayMs = 150;
        private static string text = "";
        private static SplashMode currentMode = SplashMode.None;
        private static int width;
        private static int height;
        private static int textRow;
        private static int starsRow;
        private static int cursorRow;
        private static int authorRow;
        private static bool isVisible = false;

        public static void Start(SplashMode mode)
        {
            currentMode = mode;
            step = 0;
            text = mode switch
            {
                SplashMode.Loading => $"Loading {OSInfo.OSName}",
                SplashMode.ShuttingDown => $"Shutting down {OSInfo.OSName}",
                SplashMode.Rebooting => $"Rebooting {OSInfo.OSName}",
                _ => ""
            };

            width = Console.WindowWidth;
            height = Console.WindowHeight;
            textRow = height / 4;
            starsRow = height / 2;
            cursorRow = height - 2;
            authorRow = height - 3;

            Console.Clear();
            Console.BackgroundColor = ConsoleColor.Black;
            Console.ForegroundColor = ConsoleColor.Cyan;

            Console.SetCursorPosition((width - text.Length) / 2, textRow);
            Console.Write(text);

            string author = OSInfo.Developer;
            Console.SetCursorPosition((width - author.Length) / 2, authorRow);
            Console.Write(author);

            isVisible = true;
        }

        public static void Stop()
        {
            isVisible = false;
            Console.ResetColor();
            Console.Clear();
            currentMode = SplashMode.None;
        }

        public static bool IsVisible => isVisible;

        public static void Update()
        {
            if (!isVisible) return;

            int activeStar = step % 3;
            int startCol = width / 2 - 2;

            for (int i = 0; i < 3; i++)
            {
                Console.SetCursorPosition(startCol + i * 2, starsRow);

                if (i == activeStar)
                {
                    Console.ForegroundColor = ConsoleColor.Green;
                }
                else
                {
                    Console.ForegroundColor = ConsoleColor.White;
                }

                Console.Write("*");
            }

            Console.SetCursorPosition(width / 2, cursorRow);
            Console.Write(step / 4 % 2 == 0 ? " " : " ");

            Console.ForegroundColor = ConsoleColor.Cyan;
            step++;
            Thread.Sleep(delayMs);
        }
    }
}