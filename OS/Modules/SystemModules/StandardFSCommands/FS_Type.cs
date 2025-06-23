using OS.Modules.ModulesControl;
using OS.Modules.SystemModules.Shell;
using OS.System.FileSystem;
using System;
using System.Collections.Generic;

namespace OS.Modules.SystemModules.StandardFSCommands
{
    public class FStype : IModule, ICommandInfoProvider
    {
        public string Name => "FStype";

        public string Description => "Provides commands to get file system type.";

        public string Category => "System utility";

        public void RegisterCommands(CommandDispatcher dispatcher)
        {
            dispatcher.Register("fs-type", FsTypeCommand);
        }

        private void FsTypeCommand(string[] args)
        {
            string path = @"0:\";

            string fsType = FileSystem.GetFileSystemType(path);

            Console.WriteLine("File System type: " + fsType);
        }
        public Dictionary<string, string> GetCommands() => new()
        {
            { "fs-type", "Shows the file system type." },
        };
    }
}