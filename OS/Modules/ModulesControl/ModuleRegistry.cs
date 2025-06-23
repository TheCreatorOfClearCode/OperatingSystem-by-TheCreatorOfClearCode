using System;
using System.Collections.Generic;
using System.Linq;

// Standard Modules
using OS.Modules.StandardModules.Calculator;
using OS.Modules.StandardModules.DateAndTime;

// System Modules
using OS.Modules.SystemModules.Installer;
using OS.Modules.SystemModules.StandardFSCommands;
using OS.Modules.SystemModules.PowerControl;
using OS.Modules.SystemModules.Settings;
using OS.Modules.SystemModules.Settings.Helpers;
using OS.Modules.SystemModules.Shell;
using OS.Modules.SystemModules.Logs;

namespace OS.Modules.ModulesControl
{
    public static class ModuleRegistry
    {
        private static readonly List<IModule> modules = new()
        {
            // System Modules
            new Installer(),
            new CLI(),
            new PowerControl(),
            new StandardFSCommands(),
            new FStype(), 
            new AboutSystem(),
            new InstalledModules(),
            new Help(),
            new AboutShell(),
            new Logs(),

            // Standard Modules
            new Calculator(),
            new DateAndTime(),
        };

        public static List<IModule> GetModules() => modules;

        public static IModule? GetModuleByName(string name)
        {
            return modules.FirstOrDefault(m => m.Name.Equals(name, StringComparison.OrdinalIgnoreCase));
        }
    }
}
