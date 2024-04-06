using System;
using Fox;

namespace Sandbox
{
   public class Player : Entity
    {
        private TransformComponent m_Transform;
        private Rigidbody2D m_Rb2d;
        // private Animator m_Animator;

        public float Speed = 15.1f;
        public Prefab BulletPrefab;

        public Entity CameraEntity;

        CameraComponent Camera;
        Vector3 mousePos;
        bool isPressed;

        public void OnCreate()
        {
            m_Transform = GetComponent<TransformComponent>();
            m_Rb2d = GetComponent<Rigidbody2D>();
            // m_Animator = GetComponent<Animator>();
            // OnCollisionEnter2DEvent += OnCollisionEnter2D;

            // m_Animator.Subscribe("myevent", () =>
            // {
            //     Log.Info("My event is sended");
            // });
            //
            // m_Animator = GetComponent<Animator>();
            // m_Animator.Subscribe("myevent", () =>
            // {
            //     Log.Info("My event is sended 2222");
            // });

            if (CameraEntity)
            {
                Camera = CameraEntity.GetComponent<CameraComponent>();
            }
        }

        // private void OnCollisionEnter2D(CollisionData obj)
        // {
        //     Log.Info($"Collide withs {obj.entity.GetComponent<NameComponent>().Name}");
        // }

        public void OnUpdate(float ts)
        {
            Vector3 translation = m_Transform.position;
            Vector3 velocity = Vector3.zero;

            if (Input.IsKeyDown(KeyCode.A))
            {
                velocity.x = -1.0f;
            }

            if (Input.IsKeyDown(KeyCode.D))
            {
                velocity.x = 1.0f;
            }
            
            if (Input.IsKeyDown(KeyCode.W))
            {
                velocity.y = 1.0f;
            }

            if (Input.IsKeyDown(KeyCode.S))
            {
                velocity.y = -1.0f;
            }

            if (Input.IsMouseButtonPressed(MouseCode.Button0) && BulletPrefab != null && !isPressed)
            {
                Vector3 pos = m_Transform.position;
                pos += m_Transform.right * 1;
                Instantiate(BulletPrefab, pos, m_Transform.rotation);
                isPressed = true;
            }
            else
            {
                isPressed = false;
            }

            velocity *= Speed;
            translation += velocity * ts;
            m_Transform.position = translation;
            // m_Rb2d.ApplyLinearImpulse(velocity.XZ(), Vector2.zero, true);

            mousePos = Camera.ScreenToWorld(Input.GetMousePosition());
            // m_Transform.position = mousePos;
            Vector3 lookDir = mousePos - m_Transform.position;
            float angle = Mathfs.Atan2(lookDir.y, lookDir.x) * Mathfs.Rad2Deg;
            m_Transform.rotation = new Vector3(0, 0, angle);
        }
    }
}
