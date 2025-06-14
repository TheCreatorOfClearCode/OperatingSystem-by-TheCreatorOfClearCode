using System;
using Sys = Cosmos.System;
using OS.System.InitSystem;
using OS.System.AboutSystem;

namespace OS
{
    public class Kernel : Sys.Kernel
    {
        protected override void BeforeRun()
        {
            Console.WriteLine("--------------------------------------------------------------------------------");
            Console.ForegroundColor = ConsoleColor.Green;
            Console.WriteLine($"Booting {OSInfo.OSName} v{OSInfo.OSVersion}...");
            Console.ResetColor();
            Console.WriteLine("--------------------------------------------------------------------------------");
            Console.ForegroundColor = ConsoleColor.Blue;
            Console.WriteLine("--------------------------------------------------------------------------------");
            Console.ResetColor();
            Console.WriteLine($"Starting {InitSystem.InitSystemName}...");
            Console.ForegroundColor = ConsoleColor.Blue;
            Console.WriteLine("--------------------------------------------------------------------------------");
            Console.ResetColor();
            InitSystem.Init();
        }

        protected override void Run()
        {
        }
    }
}