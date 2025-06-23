using System;
using System.IO;
using System.Collections.Generic;
using OS.Modules.ModulesControl;
using OS.Modules.SystemModules.Shell;
using OS.System.FileSystem;
using OS.System.Sounds.StandardSoundTheme;

namespace OS.Modules.SystemModules.StandardFSCommands
{
    public class StandardFSCommands : IModule, ICommandInfoProvider
    {
        public string Name => "StandardFS";
        public string Description => "Standard filesystem commands.";
        public string Category => "System utility";

        public void RegisterCommands(CommandDispatcher dispatcher)
        {
            dispatcher.Register("pwd", args =>
            {
                Console.WriteLine(FileSystem.CurrentDirectory);
            });

            dispatcher.Register("cd", args =>
            {
                foreach (var arg in args[0].Split('|'))
                    FileSystem.ChangeDirectory(arg);
            });

            dispatcher.Register("cd..", args =>
            {
                FileSystem.ChangeDirectory("..");
            });

            dispatcher.Register("mkdir", args =>
            {
                foreach (var arg in args[0].Split('|'))
                {
                    var path = FileSystem.ResolvePath(arg);
                    if (!Directory.Exists(path))
                    {
                        Directory.CreateDirectory(path);
                        Console.ForegroundColor = ConsoleColor.Green;
                        Console.WriteLine($"Created directory: {arg}");
                        Console.ResetColor();
                    }
                    else
                    {
                        Console.WriteLine($"Directory already exists: {arg}");
                    }
                }
            });

            dispatcher.Register("rmdir", args =>
            {
                foreach (var arg in args[0].Split('|'))
                {
                    var path = FileSystem.ResolvePath(arg);
                    if (File.Exists(path))
                    {
                        Sounds.PlayErrorSound();
                        Console.ForegroundColor = ConsoleColor.Red;
                        Console.WriteLine($"ERROR: \"{arg}\" is a file, not a directory.");
                        Console.ResetColor();
                    }
                    else if (!Directory.Exists(path))
                    {
                        Sounds.PlayErrorSound();
                        Console.ForegroundColor = ConsoleColor.Red;
                        Console.WriteLine($"ERROR: Directory \"{arg}\" does not exist.");
                        Console.ResetColor();
                    }
                    else
                    {
                        Directory.Delete(path, true);
                        Console.ForegroundColor = ConsoleColor.Green;
                        Console.WriteLine($"Deleted directory: {arg}");
                        Console.ResetColor();
                    }
                }
            });

            dispatcher.Register("touch", args =>
            {
                foreach (var arg in args[0].Split('|'))
                {
                    var path = FileSystem.ResolvePath(arg);
                    File.Create(path).Close();
                    Console.ForegroundColor = ConsoleColor.Green;
                    Console.WriteLine($"Created file: {arg}");
                    Console.ResetColor();
                }
            });

            dispatcher.Register("rmfile", args =>
            {
                foreach (var arg in args[0].Split('|'))
                {
                    var path = FileSystem.ResolvePath(arg);
                    if (Directory.Exists(path))
                    {
                        Sounds.PlayErrorSound();
                        Console.ForegroundColor = ConsoleColor.Red;
                        Console.WriteLine($"ERROR: \"{arg}\" is a directory, not a file.");
                        Console.ResetColor();
                    }
                    else if (!File.Exists(path))
                    {
                        Sounds.PlayErrorSound();
                        Console.ForegroundColor = ConsoleColor.Red;
                        Console.WriteLine($"ERROR: File \"{arg}\" does not exist.");
                        Console.ResetColor();
                    }
                    else
                    {
                        File.Delete(path);
                        Console.ForegroundColor = ConsoleColor.Green;
                        Console.WriteLine($"Deleted file: {arg}");
                        Console.ResetColor();
                    }
                }
            });

            dispatcher.Register("read", args =>
            {
                foreach (var arg in args[0].Split('|'))
                {
                    var path = FileSystem.ResolvePath(arg);
                    if (!File.Exists(path))
                    {
                        Sounds.PlayErrorSound();
                        Console.ForegroundColor = ConsoleColor.Red;
                        Console.WriteLine($"ERROR: File \"{arg}\" does not exist.");
                        Console.ResetColor();
                    }
                    else if (Directory.Exists(path))
                    {
                        Sounds.PlayErrorSound();
                        Console.ForegroundColor = ConsoleColor.Red;
                        Console.WriteLine($"ERROR: \"{arg}\" is a directory.");
                        Console.ResetColor();
                    }
                    else
                    {
                        Console.WriteLine($"--- {arg} ---");
                        Console.WriteLine(File.ReadAllText(path));
                    }
                }
            });

            dispatcher.Register("write", args =>
            {
                var split = args[0].Split("->", 2);
                if (split.Length != 2)
                {
                    Console.WriteLine("Usage: write=\"file\"->text");
                    return;
                }

                var path = FileSystem.ResolvePath(split[0]);
                if (Directory.Exists(path))
                {
                    Sounds.PlayErrorSound();
                    Console.ForegroundColor = ConsoleColor.Red;
                    Console.WriteLine($"ERROR: \"{split[0]}\" is a directory.");
                    Console.ResetColor();
                    return;
                }

                File.WriteAllText(path, split[1]);
                Console.ForegroundColor = ConsoleColor.Green;
                Console.WriteLine($"Wrote to file: {split[0]}");
                Console.ResetColor();
            });

            dispatcher.Register("ls", args =>
            {
                foreach (var arg in args.Length == 0 ? new[] { FileSystem.CurrentDirectory } : args[0].Split('|'))
                {
                    var path = FileSystem.ResolvePath(arg);
                    if (!Directory.Exists(path))
                    {
                        if (File.Exists(path))
                        {
                            Sounds.PlayErrorSound();
                            Console.ForegroundColor = ConsoleColor.Red;
                            Console.WriteLine($"ERROR: \"{arg}\" is a file, not a directory.");
                            Console.ResetColor();
                        }
                        else
                        {
                            Sounds.PlayErrorSound();
                            Console.ForegroundColor = ConsoleColor.Red;
                            Console.WriteLine($"ERROR: Directory \"{arg}\" not found.");
                            Console.ResetColor();
                        }
                        continue;
                    }

                    Console.ForegroundColor = ConsoleColor.Cyan;
                    Console.WriteLine($"Contents of {arg}:");
                    Console.ResetColor();

                    foreach (var dir in Directory.GetDirectories(path))
                        Console.WriteLine("[DIR]  " + Path.GetFileName(dir));

                    foreach (var file in Directory.GetFiles(path))
                        Console.WriteLine("[FILE] " + Path.GetFileName(file));
                }
            });

            dispatcher.Register("copy", args =>
            {
                var split = args[0].Split("->", 2);
                if (split.Length != 2)
                {
                    Console.WriteLine("Usage: copy=\"source\"->\"destination\"");
                    return;
                }

                string[] sources = split[0].Split('|');
                string destination = FileSystem.ResolvePath(split[1]);

                foreach (var src in sources)
                {
                    string source = FileSystem.ResolvePath(src);

                    if (File.Exists(source))
                    {
                        File.Copy(source, Path.Combine(destination, Path.GetFileName(source)), true);
                        Console.ForegroundColor = ConsoleColor.Green;
                        Console.WriteLine($"Copied file: {src} -> {split[1]}");
                        Console.ResetColor();
                    }
                    else if (Directory.Exists(source))
                    {
                        CopyDirectory(source, Path.Combine(destination, Path.GetFileName(source)));
                        Console.ForegroundColor = ConsoleColor.Green;
                        Console.WriteLine($"Copied directory: {src} -> {split[1]}");
                        Console.ResetColor();
                    }
                    else
                    {
                        Sounds.PlayErrorSound();
                        Console.ForegroundColor = ConsoleColor.Red;
                        Console.WriteLine($"ERROR: Source \"{src}\" does not exist.");
                        Console.ResetColor();
                    }
                }
            });

            dispatcher.Register("move", args =>
            {
                var split = args[0].Split("->", 2);
                if (split.Length != 2)
                {
                    Console.WriteLine("Usage: move=\"source\"->\"destination\"");
                    return;
                }

                string[] sources = split[0].Split('|');
                string destination = FileSystem.ResolvePath(split[1]);

                foreach (var src in sources)
                {
                    string source = FileSystem.ResolvePath(src);

                    if (File.Exists(source))
                    {
                        string destPath = Path.Combine(destination, Path.GetFileName(source));
                        File.Copy(source, destPath, true);
                        File.Delete(source);
                        Console.ForegroundColor = ConsoleColor.Green;
                        Console.WriteLine($"Moved file: {src} -> {split[1]}");
                        Console.ResetColor();
                    }
                    else if (Directory.Exists(source))
                    {
                        CopyDirectory(source, Path.Combine(destination, Path.GetFileName(source)));
                        Directory.Delete(source, true);
                        Console.ForegroundColor = ConsoleColor.Green;
                        Console.WriteLine($"Moved directory: {src} -> {split[1]}");
                        Console.ResetColor();
                    }
                    else
                    {
                        Sounds.PlayErrorSound();
                        Console.ForegroundColor = ConsoleColor.Red;
                        Console.WriteLine($"ERROR: Source \"{src}\" does not exist.");
                        Console.ResetColor();
                    }
                }
            });
        }

        private void CopyDirectory(string sourceDir, string destDir)
        {
            Directory.CreateDirectory(destDir);

            foreach (string file in Directory.GetFiles(sourceDir))
            {
                string destFile = Path.Combine(destDir, Path.GetFileName(file));
                File.Copy(file, destFile, true);
            }

            foreach (string dir in Directory.GetDirectories(sourceDir))
            {
                string destSubDir = Path.Combine(destDir, Path.GetFileName(dir));
                CopyDirectory(dir, destSubDir);
            }
        }

        public Dictionary<string, string> GetCommands() => new()
        {
            { "pwd", "Print current directory" },
            { "cd", "Change directory. Empty to go home" },
            { "cd..", "Go up one level" },
            { "mkdir=", "Create directories (| for multiple)" },
            { "rmdir=", "Remove directories (| for multiple)" },
            { "touch=", "Create empty files (| for multiple)" },
            { "rmfile=", "Remove files (| for multiple)" },
            { "read=", "Read file content (| for multiple)" },
            { "write=", "Write to file: write=\"file\"->text" },
            { "ls=", "List contents of directory (| for multiple)" },
            { "copy=", "Copy files or directories: copy=\"src|src2\"->\"dest\"" },
            { "move=", "Move files or directories: move=\"src|src2\"->\"dest\"" }
        };
    }
}