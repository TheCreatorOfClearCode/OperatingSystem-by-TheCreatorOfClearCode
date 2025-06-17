using OS.Modules.ModulesControl;
using OS.Modules.SystemModules.Shell;
using OS.Modules.StandardModules.DateAndTime.Helpers;
using System;
using System.Collections.Generic;
using System.Threading;

namespace OS.Modules.StandardModules.DateAndTime
{
    public class DateAndTime : IModule, ICommandInfoProvider
    {
        public string Name => "DateAndTime";

        public string Category => "Utility";

        public string Description => "Shows date, time and date+time in real time";

        public void RegisterCommands(CommandDispatcher dispatcher)
        {
            dispatcher.Register("time", args => ShowLoop("Time", () => ClocksHelper.GetTime()));
            dispatcher.Register("date", args => ShowLoop("Date", () => DateHelper.GetDate()));
            dispatcher.Register("date&time", args => ShowLoop("Date and time", () => $"{DateHelper.GetDate()} {ClocksHelper.GetTime()}"));
        }

        // Реализация метода GetCommands()
        public Dictionary<string, string> GetCommands()
        {
            return new Dictionary<string, string>
            {
                { "time", "Displays the current time in real time" },
                { "date", "Displays the current date in real time" },
                { "date&time", "Displays the current date and time in real time" },
            };
        }

        private void ShowLoop(string label, Func<string> getValue)
        {
            Console.Clear();
            Console.WriteLine($"Mode: {label}. Enter 'exit' to exit.\n");

            while (true)
            {
                Console.Clear();
                Console.ForegroundColor = ConsoleColor.Yellow;
                Console.WriteLine($"{label}: {getValue()}");
                Console.ResetColor();
                Console.WriteLine();
                Console.Write("-> ");

                if (Console.KeyAvailable)
                {
                    string input = Console.ReadLine();
                    if (input != null && input.Trim().ToLower() == "exit")
                    {
                        Console.Clear();
                        Console.ForegroundColor = ConsoleColor.Blue;
                        Console.WriteLine("--------------------------------------------------------------------------------");
                        Console.ForegroundColor = ConsoleColor.Yellow;
                        Console.WriteLine("Exit...");
                        Console.ResetColor();
                        Console.ForegroundColor = ConsoleColor.Blue;
                        Console.WriteLine("--------------------------------------------------------------------------------");
                        Console.ResetColor();
                        Thread.Sleep(500);
                        Console.Clear();
                        break;
                    }
                }
                else
                {
                    Thread.Sleep(1000);
                }
            }
        }
    }
}
