using Cosmos.Core;
using Cosmos.HAL;

namespace OS.System.AboutSystem
{
    public static class HardInfo
    {
        public static string CPUName => CPU.GetCPUBrandString();

        public static ulong TotalRAM => Cosmos.Core.CPU.GetAmountOfRAM();

        public static ulong AvailableRAM => Cosmos.Core.GCImplementation.GetAvailableRAM();

        public static ulong UsedRAM => Cosmos.Core.GCImplementation.GetUsedRAM();
    }
}