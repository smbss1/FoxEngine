namespace Fox
{
    public class Time
    {
        // The time in seconds it took to complete the last frame (RO).
        public static float deltaTime => InternalCalls.Time_deltaTime();
    }
}