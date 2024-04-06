using Fox;

namespace Sandbox
{
    public class CameraFollow : Entity
    {
        public Entity Player;
        public float dampTime = 0.4f;
        private Vector3 cameraPos;
        private Vector3 velocity = Vector3.zero;

        private TransformComponent PlayerTransform;
        private TransformComponent m_Transform;

        void OnCreate()
        {
            PlayerTransform = Player.GetComponent<TransformComponent>();
            m_Transform = GetComponent<TransformComponent>();
        }
        
        void OnUpdate(float ts)
        {
            cameraPos = new Vector3(PlayerTransform.position.x, PlayerTransform.position.y, 0);
            m_Transform.position = Vector3.SmoothDamp(m_Transform.position, cameraPos, ref velocity, dampTime);
        }
    }
}