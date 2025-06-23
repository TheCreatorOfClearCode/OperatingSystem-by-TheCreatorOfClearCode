using System;
using System.Collections.Generic;
using OS.System.Sounds.StandardSoundTheme;

namespace OS.Modules.SystemModules.Shell
{
    public class CommandDispatcher
    {
        private readonly Dictionary<string, Action<string[]>> commands = new();

        public void Register(string name, Action<string[]> action)
        {
            commands[name] = action;
        }

        public void Execute(string input)
        {
            if (string.IsNullOrWhiteSpace(input)) return;

            var segments = input.Split("&&", StringSplitOptions.RemoveEmptyEntries);
            foreach (var segment in segments)
            {
                string trimmed = segment.Trim();
                if (string.IsNullOrWhiteSpace(trimmed)) continue;

                string commandName;
                string[] args;

                if (trimmed.Contains('='))
                {
                    var parts = trimmed.Split('=', 2);
                    commandName = parts[0].Trim();
                    args = new[] { parts[1].Trim() };
                }
                else
                {
                    var parts = trimmed.Split(' ', StringSplitOptions.RemoveEmptyEntries);
                    commandName = parts[0];
                    args = parts.Length > 1 ? parts[1..] : Array.Empty<string>();
                }

                if (commands.TryGetValue(commandName, out var action))
                {
                    action(args);
                }
                else
                {
                    try { Sounds.PlayErrorSound(); } catch { }

                    Console.ForegroundColor = ConsoleColor.Red;
                    Console.WriteLine($"ERROR: Command \"{commandName}\" not found");
                    Console.ResetColor();
                }
            }
        }
    }
}