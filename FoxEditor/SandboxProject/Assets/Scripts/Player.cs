using System;
using Fox;

namespace Sandbox
{
    public class Player : Entity
    {
        private TransformComponent m_Transform;
        private Rigidbody2D m_Rb2d;
        private Animator m_Animator;

        public float Speed = 15.1f;
        public Prefab BulletPrefab;

        public void OnCreate()
        {
            m_Transform = GetComponent<TransformComponent>();
            m_Rb2d = GetComponent<Rigidbody2D>();
            m_Animator = GetComponent<Animator>();
            OnCollisionEnter2DEvent += OnCollisionEnter2D;

            // m_Animator.On("myevent", () =>
            // {
            //     Log.Info("My event is sended");
            // });
        }

        private void OnCollisionEnter2D(CollisionData obj)
        {
            Log.Info($"Collide with {obj.entity.GetComponent<NameComponent>().Name}");
        }

        public void OnUpdate(float ts)
        {
            Vector3 translation = m_Transform.position;
            Vector3 velocity = Vector3.zero;

            if (Input.IsKeyDown(KeyCode.Q))
            {
                velocity.x = -1.0f;
            }

            if (Input.IsKeyDown(KeyCode.D))
            {
                velocity.x = 1.0f;
            }

            if (Input.IsKeyPressed(KeyCode.Space) && BulletPrefab != null)
            {
                Vector3 pos = m_Transform.position;
                pos.y += 2;
                Instantiate(BulletPrefab, pos);
            }

            velocity *= Speed;
            translation += velocity * ts;
            m_Transform.position = translation;
            // m_Rb2d.ApplyLinearImpulse(velocity.XZ(), Vector2.zero, true);
        }
    }
}
