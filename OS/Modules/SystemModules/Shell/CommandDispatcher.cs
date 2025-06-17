using System;
using System.Collections.Generic;
using OS.System.Sounds.StandardSoundTheme;

namespace OS.Modules.SystemModules.Shell
{
    public class CommandDispatcher
    {
        private Dictionary<string, Action<string[]>> commands = new();

        public void Register(string name, Action<string[]> action)
        {
            commands[name] = action;
        }

        public void Execute(string input)
        {
            var parts = input.Split(' ', StringSplitOptions.RemoveEmptyEntries);
            if (parts.Length == 0) return;

            if (commands.TryGetValue(parts[0], out var action))
            {
                action(parts.Length > 1 ? parts[1..] : Array.Empty<string>());
            }
            else
            {
                try
                {
                    Sounds.PlayErrorSound();
                }
                catch
                {
                }

                Console.ForegroundColor = ConsoleColor.Red;
                Console.WriteLine($"ERROR: Command \"{parts[0]}\" not found");
                Console.ResetColor();
            }
        }
    }
}