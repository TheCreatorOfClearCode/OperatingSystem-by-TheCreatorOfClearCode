using Cosmos.Core;
using Cosmos.HAL;
using OS.System.FileSystem;

namespace OS.System.AboutSystem
{
    public static class HardInfo
    {
        public static string CPUName => CPU.GetCPUBrandString();

        public static ulong TotalRAM => Cosmos.Core.CPU.GetAmountOfRAM();

        public static ulong AvailableRAM => Cosmos.Core.GCImplementation.GetAvailableRAM();

        public static ulong UsedRAM => Cosmos.Core.GCImplementation.GetUsedRAM();

        public static long AvailableDiskSpaceBytes
        {
            get
            {
                if (MountFS.VFS == null)
                    return 0;

                try
                {
                    return MountFS.VFS.GetAvailableFreeSpace(@"0:\");
                }
                catch
                {
                    return 0;
                }
            }
        }

        public static double BytesToMegabytes(long bytes)
        {
            return bytes / (1024.0 * 1024.0);
        }
        public static double AvailableDiskSpaceMB => BytesToMegabytes(AvailableDiskSpaceBytes);
    }
}
