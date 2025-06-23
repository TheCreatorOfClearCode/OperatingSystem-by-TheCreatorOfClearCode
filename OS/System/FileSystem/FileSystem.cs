using System;
using System.IO;
using Cosmos.System.FileSystem;

namespace OS.System.FileSystem
{
    public static class FileSystem
    {
        public static string CurrentDirectory { get; private set; } = "Root/Home/user";

        private static string ConvertToCosmosPath(string path)
        {
            if (string.IsNullOrEmpty(path))
                return @"0:\";

            if (path.StartsWith("Root/"))
            {
                return @"0:\" + path.Substring(5).Replace('/', '\\');
            }
            else
            {
                string combined = CurrentDirectory.TrimEnd('/') + "/" + path;
                return @"0:\" + combined.Substring(5).Replace('/', '\\');
            }
        }

        private static string ConvertToDisplayPath(string cosmosPath)
        {
            if (string.IsNullOrEmpty(cosmosPath))
                return "Root/";

            if (cosmosPath.StartsWith(@"0:\"))
            {
                return "Root/" + cosmosPath.Substring(3).Replace('\\', '/');
            }

            return cosmosPath.Replace('\\', '/');
        }

        public static void ChangeDirectory(string path)
        {
            if (string.IsNullOrEmpty(path))
            {
                CurrentDirectory = "Root/Home/user";
                return;
            }

            if (path == "..")
            {
                if (CurrentDirectory == "Root/")
                    return;

                int lastSlash = CurrentDirectory.LastIndexOf('/');
                if (lastSlash > 4)
                {
                    CurrentDirectory = CurrentDirectory.Substring(0, lastSlash);
                }
                else
                {
                    CurrentDirectory = "Root/";
                }
                return;
            }

            string cosmosPath = ConvertToCosmosPath(path);
            if (Directory.Exists(cosmosPath))
            {
                CurrentDirectory = ConvertToDisplayPath(cosmosPath);
            }
            else
            {
                Console.WriteLine("Directory not found: " + path);
            }
        }

        public static string ResolvePath(string path)
        {
            return ConvertToCosmosPath(path);
        }

        public static string GetFileSystemType(string path = @"0:\")
        {
            if (MountFS.VFS == null)
                return "File system not mounted";

            try
            {
                return MountFS.VFS.GetFileSystemType(path);
            }
            catch (Exception ex)
            {
                return "Error getting FS type: " + ex.Message;
            }
        }
    }
}