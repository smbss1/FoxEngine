namespace Fox
{
    public class Input
    {
        // <summary>
        /// Key held down.
        /// </summary>
        /// <param name="keycode">KeyCode</param>
        /// <returns>true if key is held down, otherwise false.</returns>
        public static bool IsKeyPressed(KeyCode keycode) => InternalCalls.Input_IsKeyPressed(keycode);
        public static bool IsKeyDown(KeyCode keycode) => InternalCalls.Input_IsKeyDown(keycode);
        public static bool IsKeyReleased(KeyCode keycode) => InternalCalls.Input_IsKeyReleased(keycode);

        /// <summary>
        /// Mouse button held down.
        /// </summary>
        /// <param name="button">MouseCode</param>
        /// <returns>true if mouse button is held down, otherwise false.</returns>
        public static bool IsMouseButtonPressed(MouseCode button) => InternalCalls.Input_IsMouseButtonPressed(button);

        /// <summary>
        /// Mouse position.
        /// </summary>
        /// <returns>Vector2 with Mouse Position in screen space.</returns>
        public static Vector2 GetMousePosition()
        {
            InternalCalls.Input_GetMousePosition(out Vector2 result);
            return result;
        }
    }
}
