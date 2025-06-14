using System;
using System.Collections.Generic;
using OS.Modules.ModulesControl;
using OS.Modules.SystemModules.Shell;
using OS.System.InitSystem.ServicesControl;

namespace OS.System.InitSystem
{
    public enum ShutdownType { PowerOff, Reboot }

    public static class InitSystem
    {
        private static List<IModule> modules = new();
        public static CommandDispatcher Dispatcher { get; private set; } = new();
        public static string InitSystemName => "InitializationSystem";

        public static void Init()
        {
            Console.ForegroundColor = ConsoleColor.Green;
            Console.WriteLine("[Init] Initialization of the system...");
            Console.ResetColor();
            Console.WriteLine("[Init] Registering modules...");
            modules = ModuleRegistry.GetModules();

            foreach (var module in modules)
            {
                Console.ForegroundColor = ConsoleColor.Green;
                Console.WriteLine($"[Init] Found module: {module.Name}");
                Console.ResetColor();
                module.RegisterCommands(Dispatcher);
            }

            foreach (var module in modules)
            {
                if (module is IAutoStartService)
                    Console.WriteLine($"[Init] Module marked for autostart: {module.Name}");
            }

            foreach (var module in modules)
            {
                if (module is IAutoStartService auto)
                {
                    Console.ForegroundColor = ConsoleColor.Yellow;
                    Console.WriteLine($"[Init] Starting: {module.Name}");
                    Console.ResetColor();
                    auto.Start();
                    Console.ForegroundColor = ConsoleColor.Green;
                    Console.WriteLine($"[Init] Started: {module.Name}");
                    Console.ResetColor();
                }
            }
            Console.WriteLine("[Init] System initialized.");
        }

        public static void PerformShutdown(ShutdownType type)
        {
            Console.WriteLine("[Init] Beginning system shutdown...");

            foreach (var module in modules)
            {
                if (module is IStoppableService stoppable)
                {
                    Console.ForegroundColor = ConsoleColor.Yellow;
                    Console.WriteLine($"[Init] Stopping: {module.Name}");
                    Console.ResetColor();
                    stoppable.Stop();
                    Console.ForegroundColor = ConsoleColor.Green;
                    Console.WriteLine($"[Init] Stopped: {module.Name}");
                    Console.ResetColor();
                }
            }

            if (type == ShutdownType.PowerOff)
                Cosmos.System.Power.Shutdown();
            else
                Cosmos.System.Power.Reboot();
        }
    }
}