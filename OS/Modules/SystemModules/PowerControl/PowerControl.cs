using OS.Modules.ModulesControl;
using OS.Modules.SystemModules.Shell;
using OS.System.InitSystem;
using OS.System.Sounds.StandardSoundTheme;
using System;
using System.Collections.Generic;

namespace OS.Modules.SystemModules.PowerControl
{
    public class PowerControl : IModule, ICommandInfoProvider
    {
        public string Name => "PowerControl";
        public string Description => "Provides power off and reboot commands.";
        public string Category => "System utility";

        public void RegisterCommands(CommandDispatcher dispatcher)
        {
            dispatcher.Register("poweroff", args =>
            {
                Console.Clear();
                Sounds.PlayLogoutSound();
                InitSystem.PerformShutdown(ShutdownType.PowerOff);
            });

            dispatcher.Register("reboot", args =>
            {
                Console.Clear();
                Sounds.PlayLogoutSound();
                InitSystem.PerformShutdown(ShutdownType.Reboot);
            });
        }

        public Dictionary<string, string> GetCommands() => new()
        {
            { "power-poweroff", "Shuts down the system" },
            { "power-reboot", "Reboots the system" }
        };
    }
}