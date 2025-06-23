using System;
using System.IO;

namespace OS.System.Kernel
{
    public static class KernelLogger
    {
        private static readonly string LogsDir = @"0:\Logs";
        private static readonly string LogPath = Path.Combine(LogsDir, "Kernel.log");

        public static void Log(string message)
        {
            try
            {
                EnsureLogDirectory();
                string timestamp = GetTimestamp();
                string logLine = $"[{timestamp}] {message}";
                File.AppendAllText(LogPath, logLine + "\n");
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