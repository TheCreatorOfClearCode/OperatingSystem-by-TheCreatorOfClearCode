using OS.System.AboutSystem;
using OS.System.FileSystem;
using OS.System.InitSystem;
using OS.System.Kernel;
using OS.System.Kernel.SystemFailureScreen;
using OS.System.Kernel.SystemFailureScreen.Errors;
using OS.System.SplashScreen.StandardTheme;
using System;
using System.Threading;
using Sys = Cosmos.System;

namespace OS
{
    public class Kernel : Sys.Kernel
    {
        private bool triedInit = false;

        protected override void BeforeRun()
        {
            KernelLogger.Log("Kernel BeforeRun started.");

            Console.WriteLine("--------------------------------------------------------------------------------");
            Console.ForegroundColor = ConsoleColor.Green;
            Console.WriteLine($"Booting {OSInfo.OSName} v{OSInfo.OSVersion}...");
            Console.ResetColor();
            Console.WriteLine("--------------------------------------------------------------------------------");

            KernelLogger.Log($"Booting {OSInfo.OSName} v{OSInfo.OSVersion}...");

            Console.WriteLine("Mounting the file system...");
            KernelLogger.Log("Mounting the file system...");
            Thread.Sleep(100);

            try
            {
                MountFS.Mount();
                KernelLogger.Log("File system mounted.");

                if (!MountFS.IsMounted)
                {
                    KernelLogger.Log("File system mount failed.");
                    SystemFailureScreen.Trigger(MountFS_Error.Message);
                }
            }
            catch (Exception ex)
            {
                KernelLogger.Log("File system mount exception: " + ex.Message);
                SystemFailureScreen.Trigger(MountFS_Error.Message);
            }
        }

        protected override void Run()
        {
            if (!triedInit)
            {
                triedInit = true;
                Console.WriteLine();
                Console.WriteLine($"Starting {InitSystem.InitSystemName}...");
                KernelLogger.Log($"Starting {InitSystem.InitSystemName}...");
                Thread.Sleep(100);

                try
                {
                    InitSystem.Init();
                    KernelLogger.Log("InitSystem completed successfully.");
                }
                catch (Exception ex)
                {
                    KernelLogger.Log("InitSystem exception: " + ex.Message);
                    Thread.Sleep(100);
                    SystemFailureScreen.Trigger(InitSystem_Error.Message + "\n" + ex.Message);
                }
            }
        }
    }
}