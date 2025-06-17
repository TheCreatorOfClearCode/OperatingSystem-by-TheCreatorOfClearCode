using System;
using System.Collections.Generic;
// 
using OS.Modules.ModulesControl;
using OS.System.AboutSystem;
using OS.System.InitSystem;
using OS.System.InitSystem.ServicesControl;
using OS.System.Sounds.StandardSoundTheme;

namespace OS.Modules.SystemModules.Shell
{
    public class CLI : IModule, IAutoStartService, IStoppableService, ICommandInfoProvider
    {
        public string Name => "CLI";
        public string Description => "Console shell interface.";
        public string Category => "System";
        public static string ShellName => "CLI";
        public static string ShellVersion => "1.1";

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

            Sounds.PlayLoginSound();

            isRunning = true;
            while (isRunning)
            {
                Console.ForegroundColor = ConsoleColor.DarkGreen;
                Console.Write("-> ");
                Console.ResetColor();

                string input = ReadLineWithBackspaceBeep();

                if (input == "clear") Console.Clear();
                else InitSystem.Dispatcher.Execute(input);
            }
        }

        private string ReadLineWithBackspaceBeep()
        {
            var inputBuffer = new List<char>();
            while (true)
            {
                var keyInfo = Console.ReadKey(true);

                if (keyInfo.Key == ConsoleKey.Enter)
                {
                    Console.WriteLine();
                    break;
                }
                else if (keyInfo.Key == ConsoleKey.Backspace)
                {
                    if (inputBuffer.Count > 0)
                    {
                        inputBuffer.RemoveAt(inputBuffer.Count - 1);

                        int cursorLeft = Console.CursorLeft;
                        int cursorTop = Console.CursorTop;

                        if (cursorLeft == 0 && cursorTop > 0)
                        {
                            Console.SetCursorPosition(Console.BufferWidth - 1, cursorTop - 1);
                            Console.Write(' ');
                            Console.SetCursorPosition(Console.BufferWidth - 1, cursorTop - 1);
                        }
                        else
                        {
                            Console.SetCursorPosition(cursorLeft - 1, cursorTop);
                            Console.Write(' ');
                            Console.SetCursorPosition(cursorLeft - 1, cursorTop);
                        }
                    }
                    else
                    {
                        Sounds.PlayEmptyLineBackspaceSound();
                    }
                }
                else if (!char.IsControl(keyInfo.KeyChar))
                {
                    inputBuffer.Add(keyInfo.KeyChar);
                    Console.Write(keyInfo.KeyChar);
                }
            }

            return new string(inputBuffer.ToArray());
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