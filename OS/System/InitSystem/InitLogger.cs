using System;
using System.IO;

namespace OS.System.InitSystem
{
    public static class InitLogger
    {
        private static readonly string LogsDir = @"0:\Logs";
        private static readonly string LogPath = Path.Combine(LogsDir, "Initialization_System.log");

        public static void Clear()
        {
            try
            {
                EnsureLogDirectory();
                if (File.Exists(LogPath))
                    File.Delete(LogPath);
            }
            catch { }
        }

        public static void Log(string message)
        {
            try
            {
                EnsureLogDirectory();
                string timestamp = GetTimestamp();
                File.AppendAllText(LogPath, $"[{timestamp}] {message}\n");
            }
            catch { }
        }

        private static void EnsureLogDirectory()
        {
            if (!Directory.Exists(LogsDir))
                Directory.CreateDirectory(LogsDir);
        }

        private static string GetTimestamp()
        {
            var now = DateTime.Now;
            return $"{now.Hour:00}:{now.Minute:00}:{now.Second:00}";
        }
    }
}