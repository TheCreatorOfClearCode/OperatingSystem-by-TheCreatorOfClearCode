using System;

namespace OS.Modules.StandardModules.DateAndTime.Helpers
{
    public static class ClocksHelper
    {
        public static string GetTime()
        {
            return DateTime.Now.ToLongTimeString();
        }
    }
}
