using System;
using Fox;

namespace Sandbox
{
    public class Player : Entity
    {
        public void OnCreate()
        {
            Console.WriteLine("Player.OnCreate");
        }

        public void OnUpdate(float ts)
        {
            Console.WriteLine($"Player.OnUpdate: {ts}");

            Vector3 translation = Translation;
            Vector3 velocity = Vector3.Zero;

            if (Input.IsKeyDown(KeyCode.Q))
            {
                Console.WriteLine($"Player.Q");
                velocity.X = -1.0f;
            }

            if (Input.IsKeyDown(KeyCode.D))
            {
                velocity.X = 1.0f;
                Console.WriteLine($"Player.A");
            }

            velocity *= 1.0f;
            translation += velocity * ts;
            Translation = translation;
        }
    }
}
