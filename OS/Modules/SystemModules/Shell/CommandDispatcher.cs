using System;
using System.Collections.Generic;

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
                action(parts.Length > 1 ? parts[1..] : Array.Empty<string>());
            else
            {
                Console.ForegroundColor = ConsoleColor.Red;
                Console.WriteLine("ERROR: Command not found");
                Console.ResetColor();
            }
        }
    }
}