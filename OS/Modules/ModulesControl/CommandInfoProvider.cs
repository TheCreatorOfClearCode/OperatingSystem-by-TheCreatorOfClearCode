using System.Collections.Generic;

namespace OS.Modules.ModulesControl
{
    public interface ICommandInfoProvider
    {
        Dictionary<string, string> GetCommands();
    }
}
