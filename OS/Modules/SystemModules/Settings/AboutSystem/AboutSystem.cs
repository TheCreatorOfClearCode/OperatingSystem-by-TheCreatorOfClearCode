// Information about the OS: OS/System/AboutSystem/OSInfo.cs
// Information about hardware: OS/System/AboutSystem/HardInfo.cs 

using System;
using System.Collections.Generic;
using OS.Modules.ModulesControl;
using OS.Modules.SystemModules.Shell;
using OS.System.AboutSystem;

namespace OS.Modules.SystemModules.Settings
{
    public class AboutSystem : IModule, ICommandInfoProvider
    {
        public string Name => "AboutSystem";
        public string Description => "Shows OS and hardware information.";
        public string Category => "Utility";

        public void RegisterCommands(CommandDispatcher dispatcher)
        {
            dispatcher.Register("about", args =>
            {
                Console.Clear();
                Console.ForegroundColor = ConsoleColor.Blue;
                Console.WriteLine("--------------------------------------------------------------------------------");
                Console.ResetColor();
                Console.ForegroundColor = ConsoleColor.Yellow;
                Console.WriteLine("Information about the operating system");
                Console.ResetColor();
                Console.ForegroundColor = ConsoleColor.Blue;
                Console.WriteLine("--------------------------------------------------------------------------------");
                Console.ResetColor();
                Console.WriteLine($"OS name: {OSInfo.OSName}");
                Console.WriteLine($"OS version: {OSInfo.OSVersion}");
                Console.WriteLine();
                Console.WriteLine($"Developer: {OSInfo.Developer}");
                Console.ForegroundColor = ConsoleColor.Blue;
                Console.WriteLine("--------------------------------------------------------------------------------");
                Console.ResetColor();
                Console.ForegroundColor = ConsoleColor.Yellow;
                Console.WriteLine("Information about hardware");
                Console.ResetColor();
                Console.ForegroundColor = ConsoleColor.Blue;
                Console.WriteLine("--------------------------------------------------------------------------------");
                Console.ResetColor();
                Console.WriteLine($"CPU: {HardInfo.CPUName}");
                Console.WriteLine();
                Console.WriteLine($"Amount of RAM: {HardInfo.TotalRAM}MB");
                Console.WriteLine($"Available RAM: {HardInfo.AvailableRAM}MB");
                Console.WriteLine($"Used RAM: {HardInfo.UsedRAM}B");
                Console.ForegroundColor = ConsoleColor.Blue;
                Console.WriteLine("--------------------------------------------------------------------------------");
                Console.ResetColor();
            });
        }
        public Dictionary<string, string> GetCommands() => new()
        {
             { "about", "Displays information about the OS and hardware." }
        };
    }
}