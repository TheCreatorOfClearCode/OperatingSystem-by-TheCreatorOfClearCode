using System;
using System.Collections.Generic;
using OS.Modules.ModulesControl;
using OS.System.AboutSystem;
using OS.System.InitSystem;
using OS.System.InitSystem.ServicesControl;
using OS.System.Sounds.StandardSoundTheme;
using OS.System.FileSystem;

namespace OS.Modules.SystemModules.Shell
{
    public class CLI : IModule, IAutoStartService, IStoppableService, ICommandInfoProvider
    {
        public string Name => "CLI";
        public string Description => "Console shell interface.";
        public string Category => "System";
        public static string ShellName => "CLI";
        public static string ShellVersion => "2.0";

        private bool isRunning = false;

        private List<string> commandHistory = new();
        private int historyIndex = -1;

        public void RegisterCommands(CommandDispatcher dispatcher)
        {
            dispatcher.Register("clear-history", args =>
            {
                commandHistory.Clear();
                historyIndex = -1;
                Console.ForegroundColor = ConsoleColor.Cyan;
                Console.WriteLine("Command history cleared.");
                Console.ResetColor();
            });
        }

        public void Start()
        {
            Console.Clear();

            int width = Console.WindowWidth;
            int height = Console.WindowHeight;

            string welcome = $"Welcome to {OSInfo.OSName}!";
            string separator = new string('-', welcome.Length);

            int startRow = height / 27;

            Console.SetCursorPosition((width - separator.Length) / 2, startRow);
            Console.ForegroundColor = ConsoleColor.Yellow;
            Console.WriteLine(separator);

            Console.SetCursorPosition((width - welcome.Length) / 2, startRow + 1);
            Console.WriteLine(welcome);

            Console.SetCursorPosition((width - separator.Length) / 2, startRow + 2);
            Console.WriteLine(separator);
            Console.ResetColor();

            Sounds.PlayLoginSound();

            isRunning = true;
            while (isRunning)
            {
                Console.ForegroundColor = ConsoleColor.DarkGreen;

                if (FileSystem.CurrentDirectory == "Root/Home/user")
                {
                    Console.Write("-> ");
                }
                else
                {
                    Console.Write($"[{FileSystem.CurrentDirectory}] > ");
                }

                Console.ResetColor();

                string input = ReadLineWithHistory();

                if (string.IsNullOrWhiteSpace(input))
                    continue;

                if (input == "clear")
                {
                    Console.Clear();
                }
                else
                {
                    InitSystem.Dispatcher.Execute(input);
                }
            }
        }

        private string ReadLineWithHistory()
        {
            var inputBuffer = new List<char>();
            historyIndex = commandHistory.Count;

            while (true)
            {
                var keyInfo = Console.ReadKey(true);

                if (keyInfo.Key == ConsoleKey.Enter)
                {
                    Console.WriteLine();
                    string command = new string(inputBuffer.ToArray());

                    if (!string.IsNullOrWhiteSpace(command))
                    {
                        commandHistory.Add(command);
                    }

                    historyIndex = commandHistory.Count;
                    return command;
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
                else if (keyInfo.Key == ConsoleKey.UpArrow)
                {
                    if (commandHistory.Count == 0)
                        continue;

                    if (historyIndex > 0)
                        historyIndex--;
                    else
                        historyIndex = 0;

                    ReplaceCurrentLineWith(commandHistory[historyIndex], inputBuffer);
                }
                else if (keyInfo.Key == ConsoleKey.DownArrow)
                {
                    if (commandHistory.Count == 0)
                        continue;

                    if (historyIndex < commandHistory.Count - 1)
                    {
                        historyIndex++;
                        ReplaceCurrentLineWith(commandHistory[historyIndex], inputBuffer);
                    }
                    else
                    {
                        historyIndex = commandHistory.Count;
                        ReplaceCurrentLineWith(string.Empty, inputBuffer);
                    }
                }
                else if (!char.IsControl(keyInfo.KeyChar))
                {
                    inputBuffer.Add(keyInfo.KeyChar);
                    Console.Write(keyInfo.KeyChar);
                }
            }
        }

        private void ReplaceCurrentLineWith(string newText, List<char> inputBuffer)
        {
            int cursorLeft = Console.CursorLeft;
            int cursorTop = Console.CursorTop;

            Console.SetCursorPosition(cursorLeft - inputBuffer.Count, cursorTop);
            Console.Write(new string(' ', inputBuffer.Count));
            Console.SetCursorPosition(cursorLeft - inputBuffer.Count, cursorTop);

            Console.Write(newText);

            inputBuffer.Clear();
            inputBuffer.AddRange(newText.ToCharArray());
        }

        public void Stop()
        {
            isRunning = false;
        }

        public Dictionary<string, string> GetCommands() => new()
        {
            { "clear", "Clears the console screen." },
            { "clear-history", "Clears the command input history." }
        };
    }
}
