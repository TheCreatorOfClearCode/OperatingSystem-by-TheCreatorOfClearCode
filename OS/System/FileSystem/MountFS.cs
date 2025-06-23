using Cosmos.System.FileSystem;
using System;

namespace OS.System.FileSystem
{
    public static class MountFS
    {
        public static CosmosVFS VFS;
        public static bool IsMounted { get; private set; }

        public static void Mount()
        {
            IsMounted = false;

            try
            {
                VFS = new CosmosVFS();

                if (VFS == null)
                {
                    throw new Exception("CosmosVFS creation failed");
                }

                Cosmos.System.FileSystem.VFS.VFSManager.RegisterVFS(VFS);

                IsMounted = true;

                Console.ForegroundColor = ConsoleColor.Green;
                Console.WriteLine("The file system is mounted!");
                Console.ResetColor();
            }
            catch (Exception ex)
            {
                Console.ForegroundColor = ConsoleColor.Red;
                Console.WriteLine("Filesystem mount failed: " + ex.Message);
                Console.ResetColor();

                throw new Exception("MountFS failure", ex);
            }
        }
    }
}