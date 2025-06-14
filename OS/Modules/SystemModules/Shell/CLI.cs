using System;
using System.Collections.Generic;
using OS.Modules.ModulesControl;
using OS.System.AboutSystem;
using OS.System.InitSystem;
using OS.System.InitSystem.ServicesControl;

namespace OS.Modules.SystemModules.Shell
{
    public class CLI : IModule, IAutoStartService, IStoppableService, ICommandInfoProvider

    {
        public string Name => "CLI";
        public string Description => "Console shell interface.";
        public string Category => "System";
        public static string ShellName => "CLI";
        public static string ShellVersion => "1.0";

        private bool isRunning = false;

        public void RegisterCommands(CommandDispatcher dispatcher)
        {
        }

        public void Start()
        {
            Console.Clear();
            Console.WriteLine("--------------------------------------------------------------------------------");
            Console.ForegroundColor = ConsoleColor.Yellow;
            Console.WriteLine($"Welcome to {OSInfo.OSName}!");
            Console.ResetColor();
            Console.WriteLine("--------------------------------------------------------------------------------");

            isRunning = true;
            while (isRunning)
            {
                Console.ForegroundColor = ConsoleColor.DarkGreen;
                Console.Write("-> ");
                Console.ResetColor();
                var input = Console.ReadLine();

                if (input == "clear") Console.Clear();
                else InitSystem.Dispatcher.Execute(input);
            }
        }

        public void Stop()
        {
            isRunning = false;
        }
        public Dictionary<string, string> GetCommands() => new()
        {
            { "clear", "Clears the console screen." }
        };
    }
}