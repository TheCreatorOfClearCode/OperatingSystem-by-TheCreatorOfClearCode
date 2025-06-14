using OS.Modules.SystemModules.Shell;

namespace OS.Modules.ModulesControl
{
    public interface IModule
    {
        string Name { get; }
        string Description { get; }
        string Category { get; }

        void RegisterCommands(CommandDispatcher dispatcher);
    }
}
