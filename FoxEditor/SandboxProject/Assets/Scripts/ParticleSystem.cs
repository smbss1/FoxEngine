using System;
using System.Collections.Generic;
using System.Linq;
using Fox;

namespace Sandbox
{
    public class ParticleSystem : Entity
    {
        class ParticleProps
        {
            public Vector2 Position = Vector2.zero;
            public Vector2 Velocity = Vector2.zero;
            public Vector2 VelocityVariation = Vector2.zero;
            public Color ColorBegin = Color.Black;
            public  Color ColorEnd = Color.Black;
            public float SizeBegin = 1.0f, SizeEnd = 1.0f, SizeVariation = 1.0f;
            public float LifeTime = 1.0f;
        }
        
        class Particle
        {
            public Particle(int index)
            {
                Index = index;
            }
            public Vector2 Position;
            public Vector2 Velocity = Vector2.zero;
            public Color ColorBegin = Color.Black, ColorEnd = Color.Black;
            public float Rotation = 0.0f;
            public float SizeBegin = 1.0f, SizeEnd = 1.0f;

            public float LifeTime = 1.0f;
            public float LifeRemaining = 0.0f;

            public bool Active = false;

            public Entity _entity;
            public TransformComponent transform;
            public int Index;
        }
        
        List<Particle> m_ParticlePool;
        int m_PoolIndex = 0;
        public Prefab ParticlePrefab;

        Color Lerp(Color first, Color second, float by)
        {
            float r = Mathfs.Lerp(first.r, second.r, by);
            float g = Mathfs.Lerp(first.g, second.g, by);
            float b = Mathfs.Lerp(first.b, second.b, by);
            float a = Mathfs.Lerp(first.a, second.a, by);
            return new Color(r, g, b, a);
        }

        private TransformComponent m_Transform;
        ParticleProps m_Particle = new ParticleProps();

        void OnCreate()
        {
            m_ParticlePool = new List<Particle>();
            for (int i = 0; i < 500; ++i)
            {
                m_ParticlePool.Add(new Particle(i));
            }

            foreach (var particle in m_ParticlePool)
            {
                particle._entity = Instantiate(ParticlePrefab);
                particle.transform = particle._entity.GetComponent<TransformComponent>();
            }

            m_Transform = GetComponent<TransformComponent>();
            // Init here
            m_Particle.ColorBegin = new Color(254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f);
            m_Particle.ColorEnd = new Color(254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f);
            m_Particle.SizeBegin = 1.0f;
            m_Particle.SizeVariation = 0.3f;
            m_Particle.SizeEnd = 0.0f;
            m_Particle.LifeTime = 3.0f;
            m_Particle.Velocity = new Vector2(0.0f, 0.0f);
            m_Particle.VelocityVariation = new Vector2( 1f, 1f);
            m_Particle.Position = new Vector2( m_Transform.position.x, m_Transform.position.y );
        }

        void OnUpdate(float ts)
        {
            m_Particle.Position = new Vector2( m_Transform.position.x, m_Transform.position.y );
            for (int i = 0; i < 3; i++)
                Emit(m_Particle);

            foreach (var particle in m_ParticlePool)
            {
                if (!particle.Active)
                    continue;

                if (particle.LifeRemaining <= 0.0f)
                {
                    particle.Active = false;
                    continue;
                }

                particle.LifeRemaining -= ts;
                particle.Position += particle.Velocity * ts;
                particle.transform.position = particle.Position.XYtoXYZ();

                // particle.Rotation += 0.01f * ts;
                
                
                 // Fade away particles
                 float life = particle.LifeRemaining / particle.LifeTime;
                 Color color = Lerp(particle.ColorEnd, particle.ColorBegin, life);
                color.a = color.a * life;
                
                 float size = Mathfs.Lerp(particle.SizeEnd, particle.SizeBegin, life);
                 particle.transform.scale = new Vector3(size);

                // Render
                // transform.scale = new Vector3(size);
                // particle._entity.GetComponent<SpriteRenderer>().color = color;
            }
        }


        void Emit(ParticleProps particleProps)
        {
            Particle particle = m_ParticlePool[m_PoolIndex];
            int current = m_PoolIndex;
            while (particle.Active)
            {
                --m_PoolIndex;
                if (m_PoolIndex <= 0)
                    m_PoolIndex = m_ParticlePool.Count - 1;
                particle = m_ParticlePool[m_PoolIndex];
                if (current == m_PoolIndex)
                    return;
            }
            // Log.Info(m_PoolIndex);

            var random = new Random();
            particle.Active = true;
            particle.Position = particleProps.Position;
            particle.Rotation = (float)random.NextDouble() * 2.0f * Mathfs.PI;
            
            // Velocity
            particle.Velocity = particleProps.Velocity;
            particle.Velocity.x += particleProps.VelocityVariation.x * ((float)random.NextDouble() - 0.5f);
            particle.Velocity.y += particleProps.VelocityVariation.y * ((float)random.NextDouble() - 0.5f);

            // Color
            particle.ColorBegin = particleProps.ColorBegin;
            particle.ColorEnd = particleProps.ColorEnd;
            
            particle.LifeTime = particleProps.LifeTime;
            particle.LifeRemaining = particleProps.LifeTime;
            particle.SizeBegin = particleProps.SizeBegin + particleProps.SizeVariation * ((float)random.NextDouble() - 0.5f);
            particle.SizeEnd = particleProps.SizeEnd;
            
            --m_PoolIndex;
            if (m_PoolIndex <= 0)
                m_PoolIndex = m_ParticlePool.Count - 1;
        }
    }
}