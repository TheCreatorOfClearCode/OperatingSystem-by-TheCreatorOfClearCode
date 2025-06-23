using OS.Modules.ModulesControl;
using OS.Modules.SystemModules.Shell;
using OS.System.InitSystem.ServicesControl;
using OS.System.SplashScreen.StandardTheme;
using System;
using System.Collections.Generic;

namespace OS.System.InitSystem
{
    public enum ShutdownType { PowerOff, Reboot }

    public static class InitSystem
    {
        private static List<IModule> modules = new();
        public static CommandDispatcher Dispatcher { get; private set; } = new();

        public static string InitSystemName => "InitializationSystem";

        public static bool IsInitialized { get; private set; } = false;

        public static void Init()
        {
            InitLogger.Log("Init started.");
            SplashScreen.StandardTheme.SplashScreen.Start(SplashMode.Loading);
            InitLogger.Log("SplashScreen started.");

            modules = ModuleRegistry.GetModules();
            InitLogger.Log($"Found {modules.Count} modules.");

            int index = 0;
            while (index < modules.Count)
            {
                SplashScreen.StandardTheme.SplashScreen.Update();

                var module = modules[index];
                InitLogger.Log($"Registering commands for module: {module.Name}");
                module.RegisterCommands(Dispatcher);
                index++;
            }

            index = 0;
            while (index < modules.Count)
            {
                SplashScreen.StandardTheme.SplashScreen.Update();

                var module = modules[index];
                if (module is IAutoStartService auto)
                {
                    InitLogger.Log($"Starting module: {module.Name}");
                    auto.Start();
                    InitLogger.Log($"Started module: {module.Name}");
                }
                index++;
            }

            SplashScreen.StandardTheme.SplashScreen.Stop();
            InitLogger.Log("SplashScreen stopped.");
            InitLogger.Log("Init completed.");
            IsInitialized = true;
        }

        public static void PerformShutdown(ShutdownType type)
        {
            InitLogger.Log("SplashScreen started.");
            SplashScreen.StandardTheme.SplashScreen.Start(type == ShutdownType.PowerOff ? SplashMode.ShuttingDown : SplashMode.Rebooting);
            InitLogger.Log(type == ShutdownType.PowerOff ? "Shutdown started." : "Reboot started.");

            int index = 0;
            while (index < modules.Count)
            {
                SplashScreen.StandardTheme.SplashScreen.Update();

                var module = modules[index];
                if (module is IStoppableService stoppable)
                {
                    InitLogger.Log($"Stopping module: {module.Name}");
                    stoppable.Stop();
                    InitLogger.Log($"Stopped module: {module.Name}");
                }
                index++;
            }

            SplashScreen.StandardTheme.SplashScreen.Stop();
            InitLogger.Log(type == ShutdownType.PowerOff ? "Shutdown completed." : "Reboot completed.");
            InitLogger.Log("SplashScreen stopped.");

            if (type == ShutdownType.PowerOff)
                Cosmos.System.Power.Shutdown();
            else
                Cosmos.System.Power.Reboot();
        }
    }
}