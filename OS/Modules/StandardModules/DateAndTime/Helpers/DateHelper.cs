using System;

namespace OS.Modules.StandardModules.DateAndTime.Helpers
{
    public static class DateHelper
    {
        public static string GetDate()
        {
            return DateTime.Now.ToLongDateString();
        }
    }
}
