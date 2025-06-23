using OS.System.Sounds.StandardSoundTheme;
using OS.Modules.ModulesControl;
using OS.Modules.SystemModules.Shell;
using System;
using System.Collections.Generic;

namespace OS.Modules.SystemModules.Settings
{
    public class Help : IModule, ICommandInfoProvider
    {
        public string Name => "Help";
        public string Description => "Displays available modules and their commands.";
        public string Category => "Utility";

        private const int PageSize = 14;

        public void RegisterCommands(CommandDispatcher dispatcher)
        {
            dispatcher.Register("help", args =>
            {
                Console.Clear();
                var outputLines = new List<string>();

                if (args.Length == 1 && args[0].Contains("&"))
                {
                    foreach (var rawName in args[0].Split('&'))
                        AppendModuleCommands(rawName.Trim(), outputLines);
                }
                else if (args.Length == 1)
                {
                    AppendModuleCommands(args[0], outputLines);
                }
                else
                {
                    foreach (var module in ModuleRegistry.GetModules())
                        if (module is ICommandInfoProvider)
                            AppendModuleCommands(module.Name, outputLines, suppressError: true);
                }

                if (outputLines.Count == 0)
                {
                    Console.ForegroundColor = ConsoleColor.Red;
                    Console.WriteLine("No commands found.");
                    Console.ResetColor();
                    return;
                }

                PaginateOutput(outputLines, "Help - Available Commands");
            });
        }

        private void AppendModuleCommands(string moduleName, List<string> output, bool suppressError = false)
        {
            var module = ModuleRegistry.GetModules().Find(m =>
                string.Equals(m.Name, moduleName, StringComparison.OrdinalIgnoreCase));

            if (module == null)
            {
                if (!suppressError)
                {
                    Sounds.PlayErrorSound();
                    Console.ForegroundColor = ConsoleColor.Red;
                    Console.WriteLine($"ERROR: Module \"{moduleName}\" not found.");
                    Console.ResetColor();
                }
                return;
            }

            if (module is ICommandInfoProvider provider)
            {
                var commands = provider.GetCommands();
                if (commands.Count > 0)
                {
                    output.Add($"Module: {module.Name}");

                    foreach (var command in commands)
                        output.Add($"{command.Key} :: {command.Value}");

                    output.Add("");
                }
            }
        }

        private void PaginateOutput(List<string> output, string title)
        {
            int totalPages = (output.Count + PageSize - 1) / PageSize;
            int page = 0;
            ConsoleKey key;

            do
            {
                Console.Clear();
                PrintHeader($"{title} ({page + 1}/{totalPages})");

                int start = page * PageSize;
                int end = Math.Min(start + PageSize, output.Count);

                for (int i = start; i < end; i++)
                {
                    string line = output[i];
                    if (line.StartsWith("Module: "))
                    {
                        Console.ForegroundColor = ConsoleColor.Green;
                        Console.WriteLine(line);
                    }
                    else if (line.Contains("::"))
                    {
                        string[] parts = line.Split("::", 2);
                        Console.ForegroundColor = ConsoleColor.Blue;
                        Console.Write("  " + parts[0].Trim());
                        Console.ForegroundColor = ConsoleColor.Gray;
                        Console.WriteLine(" - " + parts[1].Trim());
                    }
                    else
                    {
                        Console.WriteLine();
                    }

                    Console.ResetColor();
                }

                Console.ForegroundColor = ConsoleColor.DarkGray;
                if (totalPages == 1)
                {
                    Console.WriteLine("\n[Q Quit]");
                }
                else if (page == 0)
                {
                    Console.WriteLine("\n[-> Next] [Q Quit]");
                }
                else if (page == totalPages - 1)
                {
                    Console.WriteLine("\n[<- Prev] [Q Quit]");
                }
                else
                {
                    Console.WriteLine("\n[<- Prev] [-> Next] [Q Quit]");
                }
                Console.ResetColor();

                key = Console.ReadKey(true).Key;
                if (key == ConsoleKey.RightArrow && page < totalPages - 1)
                    page++;
                else if (key == ConsoleKey.LeftArrow && page > 0)
                    page--;
            }
            while (key != ConsoleKey.Q);
        }

        private void PrintHeader(string title)
        {
            Console.ForegroundColor = ConsoleColor.Cyan;
            Console.WriteLine(new string('=', Console.WindowWidth));
            Console.ResetColor();
            Console.ForegroundColor = ConsoleColor.Yellow;
            Console.WriteLine(title);
            Console.ResetColor();
            Console.ForegroundColor = ConsoleColor.Cyan;
            Console.WriteLine(new string('-', Console.WindowWidth));
            Console.ResetColor();
        }

        public Dictionary<string, string> GetCommands() => new()
        {
            { "help", "Shows all available module commands." },
            { "help=<module>", "Shows commands for specified module." },
            { "help=<mod1>&<mod2>", "Shows commands for multiple modules." }
        };
    }
}