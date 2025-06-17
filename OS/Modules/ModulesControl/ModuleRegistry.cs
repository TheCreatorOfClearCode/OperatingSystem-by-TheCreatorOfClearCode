using OS.Modules.ModulesControl;

// The path to the modules

// System modules
using OS.Modules.SystemModules.PowerControl;
using OS.Modules.SystemModules.Settings;
using OS.Modules.SystemModules.Settings.Helpers;
using OS.Modules.SystemModules.Shell;
using System.Collections.Generic;

// Standard Modules
using OS.Modules.StandardModules.Calculator;
using OS.Modules.StandardModules.DateAndTime;

namespace OS.Modules.ModulesControl
{
    public static class ModuleRegistry
    {
        public static List<IModule> GetModules()
        {
            return new List<IModule>
            {
                // Register the module

                // SystemModules
                new CLI(),
                new PowerControl(),
                new AboutSystem(),
                new InstalledModules(),
                new Help(),
                new AboutShell(),

                // Standard Modules
                new Calculator(),
                new DateAndTime()
            };
        }
    }
}