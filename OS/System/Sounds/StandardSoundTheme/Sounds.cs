using System;

namespace OS.System.Sounds.StandardSoundTheme
{
    public static class Sounds
    {
        public static void PlayLoginSound() 
        {
            Console.Beep(523, 120);
            Console.Beep(587, 120);
            Console.Beep(659, 150);
            Console.Beep(784, 170);
        }
        public static void PlayLogoutSound()
        {
            Console.Beep(784, 170);
            Console.Beep(659, 150);
            Console.Beep(587, 120);
            Console.Beep(523, 120);
        }
        public static void PlayErrorSound()
        {
            Console.Beep(659, 100);
            Console.Beep(523, 100);
        }
        public static void PlayCalculatorErrorSound()
        {
            Console.Beep(659, 120);
            Console.Beep(587, 160);
        }
        public static void PlayEmptyLineBackspaceSound()
        {
            Console.Beep(330, 100);
            Console.Beep(294, 120);
        }
    }
}