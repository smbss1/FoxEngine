using System;
using Fox;

namespace Sandbox
{
    public class Player : Entity
    {
        private TransformComponent m_Transform;
        private Rigidbody2D m_Rb2d;

        public float Speed = 15.2f;

        public void OnCreate()
        {
            m_Transform = GetComponent<TransformComponent>();
            m_Rb2d = GetComponent<Rigidbody2D>();
        }

        public void OnUpdate(float ts)
        {
            Vector3 translation = m_Transform.Translation;
            Vector3 velocity = Vector3.Zero;

            if (Input.IsKeyDown(KeyCode.Q))
            {
                velocity.X = -1.0f;
            }

            if (Input.IsKeyDown(KeyCode.D))
            {
                velocity.X = 1.0f;
            }

            velocity *= Speed;
            m_Rb2d.ApplyLinearImpulse(velocity.XY, Vector2.Zero, true);
            // translation += velocity * ts;
            // m_Transform.Translation = translation;
        }
    }
}