using System;
using System.Collections.Generic;
using OS.Modules.ModulesControl;
using OS.Modules.SystemModules.Shell;

namespace OS.Modules.SystemModules.Settings
{
    public class Help : IModule
    {
        public string Name => "Help";
        public string Description => "Displays available modules and their commands.";
        public string Category => "Utility";

        public void RegisterCommands(CommandDispatcher dispatcher)
        {
            dispatcher.Register("help", args =>
            {
                Console.ForegroundColor = ConsoleColor.Blue;
                Console.WriteLine("--------------------------------------------------------------------------------");
                Console.ResetColor();
                Console.ForegroundColor = ConsoleColor.Yellow;
                Console.WriteLine("Available Commands by Modules:");
                Console.ResetColor();
                Console.ForegroundColor = ConsoleColor.Blue;
                Console.WriteLine("--------------------------------------------------------------------------------");
                Console.ResetColor();

                foreach (var module in ModuleRegistry.GetModules())
                {
                    if (module is ICommandInfoProvider provider)
                    {
                        var commands = provider.GetCommands();
                        if (commands.Count > 0)
                        {
                            Console.ForegroundColor = ConsoleColor.Green;
                            Console.WriteLine($"Module name: {module.Name}");
                            Console.ResetColor();

                            foreach (var command in commands)
                            {
                                Console.WriteLine($"  {command.Key} - {command.Value}");
                            }
                        }
                    }
                }

                Console.ForegroundColor = ConsoleColor.Blue;
                Console.WriteLine("--------------------------------------------------------------------------------");
                Console.ResetColor();
            });
        }
    }
}