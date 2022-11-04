

#ifndef FOX_SPRITE_RENDERER_HPP_
#define FOX_SPRITE_RENDERER_HPP_

#include <string>
#include <glm/glm.hpp>
#include <Renderer/SubTexture2D.hpp>
#include <Core/Property.hpp>
#include "Core/Logger/Logger.hpp"
#include "Core/Random.hpp"

namespace fox
{
    struct SpriteRenderer
    {
        SpriteRenderer() = default;
        SpriteRenderer(const SpriteRenderer&) = default;

        AssetHandle Sprite;
        glm::vec4 Color { 1.0f, 1.0f, 1.0f, 1.0f };
        float TilingFactor = 1.0f;
        int Depth = 0;
    };

    struct CircleRenderer
    {
        glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
        float Thickness = 1.0f;
        float Fade = 0.005f;

        CircleRenderer() = default;
        CircleRenderer(const CircleRenderer&) = default;
    };

    struct ParticleProps
    {
        glm::vec2 Position;
        glm::vec2 Velocity;
        glm::vec2 VelocityVariation;
        glm::vec4 ColorBegin;
        glm::vec4 ColorEnd;
        float SizeBegin = 1.0f, SizeEnd = 1.0f, SizeVariation = 1.0f;
        float Speed = 1.0f;
        float RotationSpeed = 0.0f;
        float LifeTime = 1.0f;

        float RateOverTime = 1;
    };

    struct Particle
    {
        glm::vec2 Position;
        glm::vec2 Size;
        glm::vec4 Color;

        glm::vec2 Velocity;
        glm::vec4 ColorBegin, ColorEnd;
        float Rotation;
        float SizeBegin, SizeEnd;
        float LifeTime;
        float LifeRemaining;
        float Speed;
        float RotationSpeed;
        bool Active = false;
    };

    struct ParticleSystem
    {
        using number_type = uint32_t;

        std::vector<Particle> ParticlePool;
        number_type PoolIndex = 0;
        number_type MaxParticles = 500;
        ParticleProps ParticleSettings;
        float accumulator = 0.0f;

        bool Play = false;

        Random random;

        ParticleSystem()
        {
            ParticlePool.resize(MaxParticles);

            // Init here
            ParticleSettings.ColorBegin = glm::vec4(254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f);
            ParticleSettings.ColorEnd = glm::vec4(254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f);
            ParticleSettings.SizeBegin = 1.0f;
            ParticleSettings.SizeVariation = 0.3f;
            ParticleSettings.SizeEnd = 0.0f;
            ParticleSettings.LifeTime = 5.0f;
            ParticleSettings.Velocity = glm::vec2(0.0f, 0.0f);
            ParticleSettings.VelocityVariation = glm::vec2(1.0f, 1.0f);
            ParticleSettings.Position = glm::vec2(0, 0);
        }
        ParticleSystem(const ParticleSystem&) = default;

        // stores the index of the last particle used (for quick access to next dead particle)
        number_type FirstUnusedParticle()
        {
            // first search from last used particle, this will usually return almost instantly
            for (number_type i = PoolIndex; i < MaxParticles; ++i){
                if (!ParticlePool[i].Active){
                    PoolIndex = i;
                    return i;
                }
            }
            // otherwise, do a linear search
            for (number_type i = 0; i < PoolIndex; ++i){
                if (!ParticlePool[i].Active){
                    PoolIndex = i;
                    return i;
                }
            }
            // all particles are taken, override the first one (note that if it repeatedly hits this case, more particles should be reserved)
            PoolIndex = 0;
            return 0;
        }

        void Resize()
        {
            ParticlePool.resize(MaxParticles);
        }

        void Update(float ts)
        {
            for (auto& particle : ParticlePool)
            {
                if (!particle.Active)
                    continue;

                if (particle.LifeRemaining <= 0.0f)
                {
                    particle.Active = false;
                    continue;
                }

                particle.LifeRemaining -= ts;
                particle.Position += particle.Velocity * particle.Speed * (float)ts;

                particle.Rotation += particle.RotationSpeed * ts;

                // Fade away particles
                float life = particle.LifeRemaining / particle.LifeTime;
                particle.Color = glm::mix(particle.ColorEnd, particle.ColorBegin, life);

                float size = glm::mix(particle.SizeEnd, particle.SizeBegin, life);
                particle.Size = glm::vec2(size);
            }
        }

        void Emit(number_type count = 1)
        {
            for (number_type i = 0; i < count; ++i)
            {
//                Particle& particle = ParticlePool[PoolIndex];
//                int current = PoolIndex;
//                while (particle.Active)
//                {
//                    --PoolIndex;
//                    if (PoolIndex <= 0)
//                        PoolIndex = ParticlePool.size() - 1;
//                    particle = ParticlePool[PoolIndex];
//                    if (current == PoolIndex)
//                        return;
//                }
//                --PoolIndex;
//                if (PoolIndex <= 0)
//                    PoolIndex = ParticlePool.size() - 1;

                Particle& particle = ParticlePool[FirstUnusedParticle()];

                particle.Active = true;
                particle.Position = ParticleSettings.Position;
//                particle.Rotation = random.GetFloat32() * 2.0f * M_PI;
                particle.RotationSpeed = ParticleSettings.RotationSpeed;

                // Velocity
                particle.Velocity = ParticleSettings.Velocity;
                particle.Speed = ParticleSettings.Speed;
                particle.Velocity.x += ParticleSettings.VelocityVariation.x * (random.GetFloat32() - 0.5f);
                particle.Velocity.y += ParticleSettings.VelocityVariation.y * (random.GetFloat32() - 0.5f);

                // Color
                particle.ColorBegin = ParticleSettings.ColorBegin;
                particle.ColorEnd = ParticleSettings.ColorEnd;

                particle.LifeTime = ParticleSettings.LifeTime;
                particle.LifeRemaining = ParticleSettings.LifeTime;
                particle.SizeBegin = ParticleSettings.SizeBegin + ParticleSettings.SizeVariation * (random.GetFloat32() - 0.5f);
                particle.SizeEnd = ParticleSettings.SizeEnd;
            }
        }
    };

    namespace Anim
    {
        using EventFn = std::function<void(const UUID&)>;
        struct Event : public RefCounted
        {
            UUID ID;
            std::string Name;
            Anim::EventFn Function;
        };
    }

    struct Frame : public RefCounted
    {
        AssetHandle Texture;
        UUID EventID = UUID::Empty();
    };

    struct Animation : public RefCounted
    {
    public:
        std::string Name;
        std::vector<Ref<Frame>> Frames;
        std::vector<Ref<Anim::Event>> Events;

        float Speed = 1;
        float timer;
        float resetTimer = 1.0f;
        Ref<Frame> CurrentFrame;
        bool Loop;

        int GetCurrentIndex()
        {
            return CurrentIndex;
        }

        void SetCurrentIndex(int index)
        {
            if (!Frames.empty())
            {
                CurrentIndex = index % Frames.size();
                CurrentFrame = Frames[CurrentIndex];
            }
        }

        void Update(float ts, bool& IsPlaying)
        {
            timer -= ts * Speed;
            if (timer <= 0)
            {
                timer = resetTimer;
                if (CurrentFrame && CurrentFrame->EventID != UUID::Empty())
                {
                    auto it = FindEvent(CurrentFrame->EventID);
                    if (it && it->Function)
                    {
                        it->Function(CurrentFrame->EventID);
                    }
                }

                SetCurrentIndex(++CurrentIndex);

                if (!Loop && CurrentIndex == 0)
                    IsPlaying = false;
            }
        }

        Ref<Anim::Event> FindEvent(const UUID& id)
        {
            auto it = std::find_if(Events.begin(), Events.end(), [id](Ref<Anim::Event> event) {
                return event->ID == id;
            });
            if (it == Events.end())
                return nullptr;
            return *it;
        }

    private:
        int CurrentIndex;

    };

    struct Animator
    {
    public:
        std::vector<Ref<Animation>> Animations;
        std::vector<Ref<Anim::Event>> Events;

        Ref<Animation> CurrentAnimation;
        bool Play;

        int GetCurrentIndex()
        {
            return CurrentIndex;
        }

        void SetCurrentIndex(int index)
        {
            if (!Animations.empty())
            {
                CurrentIndex = index % Animations.size();
                CurrentAnimation = Animations[CurrentIndex];
            }
        }

        void SetPlay(bool play)
        {
            Play = play;
        }

        void TogglePlay()
        {
            SetPlay(!Play);
        }

        void Update(float ts)
        {
            if (!Play)
                return;

            if (CurrentAnimation)
                CurrentAnimation->Update(ts, Play);
        }

        Ref<Anim::Event> FindEvent(const UUID& id)
        {
            auto it = std::find_if(Events.begin(), Events.end(), [id](Ref<Anim::Event> event) {
                return event->ID == id;
            });
            if (it == Events.end())
                return nullptr;
            return *it;
        }

        void SubscribeToEvent(const UUID& eventID, Anim::EventFn Function)
        {
            Ref<Anim::Event> event = FindEvent(eventID);
            if (event)
            {
                event->Function = Function;
            }
        }

        void Init()
        {
            SetCurrentIndex(0);
            for (auto& animation : Animations)
            {
                for (auto& event : animation->Events)
                {
                    auto it = std::find_if(Events.begin(), Events.end(), [event](const Ref<Anim::Event>& e) { return e->ID == event->ID; });
                    if (it == Events.end())
                    {
                        Events.push_back(event);
                    }
                }
            }
        }

    private:
        int CurrentIndex;
    };
}

#endif
