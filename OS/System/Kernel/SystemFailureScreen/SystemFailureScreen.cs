using System;
using System.Collections.Generic;
using System.Threading;
using OS.System.Sounds.StandardSoundTheme;

namespace OS.System.Kernel.SystemFailureScreen
{
    public static class SystemFailureScreen
    {
        public static void Trigger(string error)
        {
            Console.Clear();
            Console.BackgroundColor = ConsoleColor.DarkRed;
            Console.ForegroundColor = ConsoleColor.White;
            Console.Clear();

            int width = Console.WindowWidth;
            int height = Console.WindowHeight;

            Sounds.StandardSoundTheme.Sounds.PlayFatalErrorSound();

            List<string> errorLines = new();
            foreach (var line in error.Split('\n'))
            {
                if (line.Length <= width - 2)
                {
                    errorLines.Add(line);
                }
                else
                {
                    int index = 0;
                    while (index < line.Length)
                    {
                        int chunkSize = Math.Min(width - 2, line.Length - index);
                        errorLines.Add(line.Substring(index, chunkSize));
                        index += chunkSize;
                    }
                }
            }

            List<string> lines = new()
            {
                "SYSTEM FAILURE",
                ""
            };

            lines.AddRange(errorLines);
            lines.Add("");
            lines.Add("The system has been halted to prevent damage.");
            lines.Add("Press [Enter] to reboot or [Space] to shutdown.");

            int totalLines = lines.Count;
            int startRow = height / 2 - totalLines / 2;

            for (int i = 0; i < totalLines; i++)
            {
                string line = lines[i];
                int startCol = width / 2 - line.Length / 2;
                if (startCol < 0) startCol = 0;

                Console.SetCursorPosition(startCol, startRow + i);

                if (i == 0)
                {
                    Console.ForegroundColor = ConsoleColor.White;
                }
                else if (i >= 2 && i < 2 + errorLines.Count)
                {
                    Console.ForegroundColor = ConsoleColor.Yellow;
                }
                else
                {
                    Console.ForegroundColor = ConsoleColor.White;
                }

                Console.Write(line);
            }

            Thread.Sleep(3000);

            while (true)
            {
                var key = Console.ReadKey(true);

                if (key.Key == ConsoleKey.Enter)
                {
                    Cosmos.System.Power.Reboot();
                }
                else if (key.Key == ConsoleKey.Spacebar)
                {
                    Cosmos.System.Power.Shutdown();
                }
                else
                {
                }
            }
        }
    }
}