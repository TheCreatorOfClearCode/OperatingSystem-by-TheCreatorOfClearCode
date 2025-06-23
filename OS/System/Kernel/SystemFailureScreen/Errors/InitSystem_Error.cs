namespace OS.System.Kernel.SystemFailureScreen.Errors
{
    public static class InitSystem_Error
    {
        public static readonly string Message =
            "Critical Error: Init System failed to initialize.\n" +
            "The system cannot continue without a properly initialized Init System.\n" +
            "Please check modules and configuration.";
    }
}
