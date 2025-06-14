using System;
using System.Collections.Generic;
using OS.Modules.ModulesControl;
using OS.Modules.SystemModules.Shell;
using OS.System.AboutSystem;

namespace OS.Modules.SystemModules.Settings
{
    public class AboutShell : IModule, ICommandInfoProvider
    {
        public string Name => "AboutShell";
        public string Description => "Shows Shell information.";
        public string Category => "Utility";

        public void RegisterCommands(CommandDispatcher dispatcher)
        {
            dispatcher.Register("about-shell", args =>
            {
                Console.ForegroundColor = ConsoleColor.Blue;
                Console.WriteLine("--------------------------------------------------------------------------------");
                Console.ResetColor();
                Console.ForegroundColor = ConsoleColor.Yellow;
                Console.WriteLine("Information about the shell");
                Console.ResetColor();
                Console.ForegroundColor = ConsoleColor.Blue;
                Console.WriteLine("--------------------------------------------------------------------------------");
                Console.ResetColor();
                Console.WriteLine($"Shell name: {CLI.ShellName}");
                Console.WriteLine($"Shell version: {CLI.ShellVersion}");
                Console.ForegroundColor = ConsoleColor.Blue;
                Console.WriteLine("--------------------------------------------------------------------------------");
                Console.ResetColor();
            });
        }
        public Dictionary<string, string> GetCommands() => new()
        {
            { "about-shell", "Displays information about the shell." }
        };
    }
}