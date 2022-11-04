using System;
using Fox;

namespace Sandbox
{
    public class Bullet : Entity
    {
        private TransformComponent m_Transform;

        public float Speed = 6.0f;

        void OnCreate()
        {
            m_Transform = GetComponent<TransformComponent>();
            OnCollisionEnter2DEvent += OnCollisionEnter2D;
        }

        private void OnCollisionEnter2D(CollisionData obj)
        {
            Log.Info($"Collide with {obj.entity.GetComponent<NameComponent>().Name}");
            Destroy();
        }

        void OnUpdate(float ts)
        {
            Vector3 translation = m_Transform.position;
            Vector3 velocity = new Vector2(0, 1).XYtoXYZ();

            velocity *= Speed;
            translation += velocity * ts;
            m_Transform.position = translation;
            // m_Rb2d.ApplyLinearImpulse(velocity.XZ(), Vector2.zero, true);
        }
    }
}
