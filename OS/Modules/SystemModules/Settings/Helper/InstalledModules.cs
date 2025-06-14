using System;
using System.Collections.Generic;
using OS.Modules.ModulesControl;
using OS.Modules.SystemModules.Shell;

namespace OS.Modules.SystemModules.Settings.Helper
{
    public class InstalledModules : IModule, ICommandInfoProvider
    {
        public string Name => "InstalledModules";
        public string Description => "Displays all installed modules and their descriptions.";
        public string Category => "Utility";

        // Метод для выбора цвета по категории
        private ConsoleColor GetCategoryColor(string category)
        {
            return category switch
            {
                "System" => ConsoleColor.Cyan,
                "System utility" => ConsoleColor.Cyan,
                "Utility" => ConsoleColor.Green,
                "User" => ConsoleColor.Yellow,
                "Settings" => ConsoleColor.Magenta,
                _ => ConsoleColor.White,
            };
        }

        public void RegisterCommands(CommandDispatcher dispatcher)
        {
            dispatcher.Register("installed-modules", args =>
            {
                Console.ForegroundColor = ConsoleColor.Blue;
                Console.WriteLine("--------------------------------------------------------------------------------");
                Console.ResetColor();
                Console.ForegroundColor = ConsoleColor.Yellow;
                Console.WriteLine("Installed Modules:");
                Console.ResetColor();
                Console.ForegroundColor = ConsoleColor.Blue;
                Console.WriteLine("--------------------------------------------------------------------------------");
                Console.ResetColor();

                foreach (var module in ModuleRegistry.GetModules())
                {
                    Console.ForegroundColor = GetCategoryColor(module.Category);
                    Console.Write($"[{module.Category}] ");
                    Console.ResetColor();
                    Console.WriteLine($"Module Name: {module.Name} - {module.Description}");
                }

                Console.ForegroundColor = ConsoleColor.Blue;
                Console.WriteLine("--------------------------------------------------------------------------------");
                Console.ResetColor();
            });
        }

        public Dictionary<string, string> GetCommands() => new()
        {
            { "installed-modules", "Displays all installed modules and their descriptions." }
        };
    }
}