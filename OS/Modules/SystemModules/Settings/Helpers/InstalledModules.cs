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
                // Собираем все строки вывода в список
                var outputLines = new List<string>();

                outputLines.Add(new string('-', 80));
                outputLines.Add("Installed Modules:");
                outputLines.Add(new string('-', 80));

                foreach (var module in ModuleRegistry.GetModules())
                {
                    // Формируем строку с цветом отдельно, добавим в список просто текст
                    // Цвета применим при выводе
                    outputLines.Add($"[{module.Category}] Module Name: {module.Name} - {module.Description}");
                }

                outputLines.Add(new string('-', 80));

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
                        string line = outputLines[i];

                        // Для заголовков — цвет синий
                        if (line.StartsWith(new string('-', 80)))
                        {
                            Console.ForegroundColor = ConsoleColor.Blue;
                            Console.WriteLine(line);
                            Console.ResetColor();
                        }
                        else if (line == "Installed Modules:")
                        {
                            Console.ForegroundColor = ConsoleColor.Yellow;
                            Console.WriteLine(line);
                            Console.ResetColor();
                        }
                        else
                        {
                            // Попытка достать категорию из строки
                            // Формат: [Category] Module Name: ...
                            int startCat = line.IndexOf('[');
                            int endCat = line.IndexOf(']');
                            if (startCat >= 0 && endCat > startCat)
                            {
                                string category = line.Substring(startCat + 1, endCat - startCat - 1);
                                Console.ForegroundColor = GetCategoryColor(category);

                                // Выводим категорию отдельно
                                Console.Write(line.Substring(0, endCat + 1));
                                Console.ResetColor();

                                // Остальная часть строки без категории
                                Console.WriteLine(line.Substring(endCat + 1));
                            }
                            else
                            {
                                // Если что-то не так, просто выводим строку белым
                                Console.WriteLine(line);
                            }
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
                        Console.WriteLine("\n-- End of installed modules list. Press any key to return. --");
                        Console.ResetColor();
                        Console.ReadKey(true);
                    }
                }
            });
        }

        private void PrintHeader()
        {
            // Заголовок здесь не меняется (можно добавить, если нужно)
            // В этом конкретном случае — заголовок выводится в outputLines, так что тут можно оставить пустым
        }

        public Dictionary<string, string> GetCommands() => new()
        {
            { "installed-modules", "Displays all installed modules and their descriptions." }
        };
    }
}