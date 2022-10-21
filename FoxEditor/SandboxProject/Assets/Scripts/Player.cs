using System;
using Fox;

namespace Sandbox
{
    public class Player : Entity
    {
        private TransformComponent m_Transform;
        private Rigidbody2D m_Rb2d;

        public float Speed = 15.2f;
        public Entity BulletPrefab;

        public void OnCreate()
        {
            m_Transform = GetComponent<TransformComponent>();
            m_Rb2d = GetComponent<Rigidbody2D>();
            OnCollisionEnter2DEvent += OnCollisionEnter2D;
        }

        private void OnCollisionEnter2D(CollisionData obj)
        {
            Log.Info($"Collide with {obj.entity.GetComponent<EntityName>().Name}");
        }

        public void OnUpdate(float ts)
        {
            Vector3 translation = m_Transform.position;
            Vector3 velocity = Vector3.zero;

            if (Input.IsKeyPressed(KeyCode.Q))
            {
                velocity.x = -1.0f;
            }

            if (Input.IsKeyPressed(KeyCode.D))
            {
                velocity.x = 1.0f;
            }

            if (Input.IsKeyDown(KeyCode.Space) && BulletPrefab != null)
            {
                Instantiate(BulletPrefab).GetComponent<TransformComponent>().position = m_Transform.position;
            }

            velocity *= Speed;
            translation += velocity * ts;
            m_Transform.position = translation;
            // m_Rb2d.ApplyLinearImpulse(velocity.XZ(), Vector2.zero, true);
        }
    }
}
