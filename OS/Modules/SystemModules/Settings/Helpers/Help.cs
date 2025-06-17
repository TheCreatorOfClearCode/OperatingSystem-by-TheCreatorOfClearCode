using OS.Modules.ModulesControl;
using OS.Modules.SystemModules.Shell;
using System;
using System.Collections.Generic;

namespace OS.Modules.SystemModules.Settings.Helpers
{
    public class Help : IModule
    {
        public string Name => "Help";
        public string Description => "Displays available modules and their commands.";
        public string Category => "Utility";

        private const int PageSize = 13;

        public void RegisterCommands(CommandDispatcher dispatcher)
        {
            dispatcher.Register("help", args =>
            {
                Console.Clear();
                PrintHeader();

                var outputLines = new List<string>();

                foreach (var module in ModuleRegistry.GetModules())
                {
                    if (module is ICommandInfoProvider provider)
                    {
                        var commands = provider.GetCommands();
                        if (commands.Count > 0)
                        {
                            outputLines.Add($"Module name: {module.Name}");
                            foreach (var command in commands)
                            {
                                outputLines.Add($"  {command.Key} - {command.Value}");
                            }
                        }
                    }
                }

                outputLines.Add("--------------------------------------------------------------------------------");

                int totalLines = outputLines.Count;
                int pages = (totalLines + PageSize - 1) / PageSize;

                for (int page = 0; page < pages; page++)
                {
                    Console.Clear();
                    PrintHeader();

                    int start = page * PageSize;
                    int end = Math.Min(start + PageSize, totalLines);

                    for (int i = start; i < end; i++)
                    {
                        var line = outputLines[i];
                        if (line.StartsWith("Module name: "))
                        {
                            Console.ForegroundColor = ConsoleColor.Green;
                            Console.WriteLine(line);
                            Console.ResetColor();
                        }
                        else if (line.StartsWith("--------------------------------------------------------------------------------"))
                        {
                            Console.ForegroundColor = ConsoleColor.Blue;
                            Console.WriteLine(line);
                            Console.ResetColor();
                        }
                        else
                        {
                            Console.WriteLine(line);
                        }
                    }

                    if (page < pages - 1)
                    {
                        Console.ForegroundColor = ConsoleColor.Yellow;
                        Console.WriteLine("\n-- Press any key for next page, or 'q' to quit --");
                        Console.ResetColor();

                        var key = Console.ReadKey(true);
                        if (key.KeyChar == 'q' || key.KeyChar == 'Q')
                        {
                            break;
                        }
                    }
                    else
                    {
                        Console.ForegroundColor = ConsoleColor.Yellow;
                        Console.WriteLine("\n-- End of help output. Press any key to return. --");
                        Console.ResetColor();
                        Console.ReadKey(true);
                    }
                }
            });
        }

        private void PrintHeader()
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
        }
    }
}