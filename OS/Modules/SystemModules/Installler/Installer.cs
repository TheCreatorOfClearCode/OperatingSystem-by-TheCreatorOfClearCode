using OS.Modules.ModulesControl;
using OS.Modules.SystemModules.Shell;
using OS.System.FileSystem;
using OS.System.InitSystem.ServicesControl;
using OS.System.Sounds.StandardSoundTheme;
using System;
using System.IO;

namespace OS.Modules.SystemModules.Installer
{
    public class Installer : IModule, IAutoStartService
    {
        public string Name => "Installer";
        public string Description => "Initial setup of folders.";
        public string Category => "System";

        private static readonly string MarkerFile = "Root/os_installed.marker";

        private static readonly string[] RequiredDirs =
        {
            "Root/Home", "Root/Home/user"
        };

        private static readonly string[] CosmosDefaults =
        {
            "Root/Dir Testing", "Root/TEST", "Root/Kudzu.txt", "Root/Root.txt"
        };

        public void Start()
        {
            try
            {
                string markerPath = FileSystem.ResolvePath(MarkerFile);

                foreach (var dir in RequiredDirs)
                {
                    string fullDir = FileSystem.ResolvePath(dir);
                    if (!Directory.Exists(fullDir))
                    {
                        Directory.CreateDirectory(fullDir);
                        Console.WriteLine($"[Installer] Created: {dir}");
                    }
                }

                if (File.Exists(markerPath))
                {
                    return;
                }

                Console.ForegroundColor = ConsoleColor.Yellow;
                Console.WriteLine("[Installer] Performing initial setup...");
                Console.ResetColor();

                foreach (var path in CosmosDefaults)
                {
                    string fullPath = FileSystem.ResolvePath(path);
                    if (File.Exists(fullPath))
                    {
                        File.Delete(fullPath);
                        Console.WriteLine($"[Installer] Deleted file: {path}");
                    }
                    else if (Directory.Exists(fullPath))
                    {
                        Directory.Delete(fullPath, true);
                        Console.WriteLine($"[Installer] Deleted directory: {path}");
                    }
                }

                using (var stream = File.Create(markerPath)) { }

                Console.WriteLine("[Installer] Installation complete. Marker file created.");
            }
            catch (Exception ex)
            {
                Console.ForegroundColor = ConsoleColor.Red;
                Console.WriteLine("[Installer] ERROR: " + ex.Message);
                Console.ResetColor();
                Sounds.PlayErrorSound();
            }
        }

        public void RegisterCommands(CommandDispatcher dispatcher)
        {
            dispatcher.Register("reinstall", args =>
            {
                try
                {
                    string markerPath = FileSystem.ResolvePath(MarkerFile);

                    foreach (var dir in RequiredDirs)
                    {
                        string fullDir = FileSystem.ResolvePath(dir);
                        if (Directory.Exists(fullDir))
                        {
                            Directory.Delete(fullDir, true);
                            Console.WriteLine($"[Installer] Removed: {dir}");
                        }
                    }

                    if (File.Exists(markerPath))
                    {
                        File.Delete(markerPath);
                        Console.ForegroundColor = ConsoleColor.Yellow;
                        Console.WriteLine("[Installer] Marker removed. Rebooting...");
                        Console.ResetColor();
                        Cosmos.System.Power.Reboot();
                    }
                    else
                    {
                        Console.WriteLine("[Installer] No marker file found. Nothing to do.");
                    }
                }
                catch (Exception ex)
                {
                    Console.ForegroundColor = ConsoleColor.Red;
                    Console.WriteLine("[Installer] ERROR during reinstall: " + ex.Message);
                    Console.ResetColor();
                    Sounds.PlayErrorSound();
                }
            });
        }
    }
}