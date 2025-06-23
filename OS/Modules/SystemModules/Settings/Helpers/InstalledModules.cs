using System;
using System.Collections.Generic;
using OS.Modules.ModulesControl;
using OS.Modules.SystemModules.Shell;

namespace OS.Modules.SystemModules.Settings.Helpers
{
    public class InstalledModules : IModule, ICommandInfoProvider
    {
        public string Name => "InstalledModules";
        public string Description => "Displays all installed modules and their descriptions.";
        public string Category => "Utility";

        private const int PageSize = 13;

        public void RegisterCommands(CommandDispatcher dispatcher)
        {
            dispatcher.Register("installed-modules", args =>
            {
                var modules = ModuleRegistry.GetModules();
                var output = new List<string>();

                output.Add($"Total installed modules: {modules.Count}");
                output.Add("");

                foreach (var module in modules)
                {
                    output.Add($"[{module.Category}] {module.Name} :: {module.Description}");
                }

                PaginateOutput(output, "Installed Modules");
            });
        }

        private void PaginateOutput(List<string> lines, string title)
        {
            int totalPages = (lines.Count + PageSize - 1) / PageSize;
            int page = 0;
            ConsoleKey key;

            do
            {
                Console.Clear();
                PrintHeader($"{title} ({page + 1}/{totalPages})");

                int start = page * PageSize;
                int end = Math.Min(start + PageSize, lines.Count);

                for (int i = start; i < end; i++)
                {
                    string line = lines[i];

                    if (line.StartsWith("Total installed modules"))
                    {
                        Console.ForegroundColor = ConsoleColor.Cyan;
                        Console.WriteLine(line);
                        Console.ResetColor();
                    }
                    else if (string.IsNullOrWhiteSpace(line))
                    {
                        Console.WriteLine();
                    }
                    else if (line.StartsWith("["))
                    {
                        int catEnd = line.IndexOf(']');
                        if (catEnd != -1)
                        {
                            string category = line.Substring(1, catEnd - 1);
                            Console.ForegroundColor = GetCategoryColor(category);
                            Console.Write(line.Substring(0, catEnd + 1));
                            Console.ResetColor();

                            var parts = line.Substring(catEnd + 1).Split("::", 2);
                            if (parts.Length == 2)
                            {
                                Console.ForegroundColor = ConsoleColor.Green;
                                Console.Write(" " + parts[0].Trim());
                                Console.ForegroundColor = ConsoleColor.Gray;
                                Console.WriteLine(" - " + parts[1].Trim());
                            }
                            else
                            {
                                Console.WriteLine(line.Substring(catEnd + 1));
                            }

                            Console.ResetColor();
                        }
                        else
                        {
                            Console.WriteLine(line);
                        }
                    }
                    else
                    {
                        Console.WriteLine(line);
                    }
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

        private ConsoleColor GetCategoryColor(string category) => category switch
        {
            "System" or "System utility" => ConsoleColor.Cyan,
            "Utility" => ConsoleColor.Green,
            "User" => ConsoleColor.Yellow,
            "Settings" => ConsoleColor.Magenta,
            _ => ConsoleColor.White,
        };

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
            { "installed-modules", "Displays all installed modules and their descriptions." }
        };
    }
}