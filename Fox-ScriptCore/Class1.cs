
using System;

namespace Fox
{
    public class Main
    {
        public float FloatVar { get; set; }
        public Main()
        {
            Console.WriteLine("Main ctr!");
        }

        public void PrintMessage()
        {
            Console.WriteLine("Hello From C#!");
        }
        
        public void PrintCustomMessage(string message)
        {
            Console.WriteLine($"C# says: {message}");
        }
    }
}
