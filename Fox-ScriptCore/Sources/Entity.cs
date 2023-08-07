using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace Fox
{
    public class Entity
    {
        private Dictionary<Type, Component> Components = new Dictionary<Type, Component>();

        protected Entity()
        {
            ID = 0;
        }


        internal Entity(ulong id)
        {
            ID = id;
        }

        public readonly ulong ID;

        public Vector3 Translation
        {
            get
            {
                InternalCalls.TransformComponent_GetTranslation(ID, out Vector3 result);
                return result;
            }
            set
            {
                InternalCalls.TransformComponent_SetTranslation(ID, ref value);
            }
        }

        public static implicit operator bool(Entity e) => e.ID != 0;

        [StructLayout(LayoutKind.Sequential)]
        public struct CollisionData
        {
            private ulong entityID;
            public Entity entity => new Entity(entityID);
        }

    #region Collisions
        public event Action<CollisionData> OnCollisionEnter2DEvent;
        public event Action<CollisionData> OnCollisionExit2DEvent;

        private void HandleOnCollisionEnter2D(CollisionData data) => OnCollisionEnter2DEvent?.Invoke(data);
        private void HandleOnCollisionExit2D(CollisionData data) => OnCollisionExit2DEvent?.Invoke(data);

    #endregion

        public bool HasComponent<T>() where T : Component, new()
        {
            return InternalCalls.Entity_HasComponent(ID, typeof(T));
        }

        public T GetComponent<T>() where T : Component, new()
        {
            if (!HasComponent<T>())
                return null;

            T component = null;
            if (!Components.ContainsKey(typeof(T)))
            {
                component = new T() { Entity = this };
                Components[typeof(T)] = component;
            }
            else
            {
                component = Components[typeof(T)] as T;
            }
            return component;
        }

        public Entity FindEntityByName(string name)
        {
            ulong entityID = InternalCalls.Entity_FindEntityByName(name);
            return new Entity(entityID);
        }

        public T As<T>() where T : Entity, new()
        {
            return InternalCalls.GetScriptInstance(ID) as T;
        }

        public Entity Instantiate(Entity entity)
        {
            ulong entityId = InternalCalls.Entity_Instantiate(entity.ID);
            return new Entity(entityId);
        }
        
        public Entity Instantiate(Prefab prefab)
        {
            ulong entityId = InternalCalls.Entity_InstantiatePrefab(ref prefab.m_Handle);
            return entityId == 0 ? null : new Entity(entityId);
        }
        
        public Entity Instantiate(Prefab prefab, Vector3 position)
        {
            ulong entityID = InternalCalls.Entity_InstantiatePrefabWithPosition(ref prefab.m_Handle, ref position);
            return entityID == 0 ? null : new Entity(entityID);
        }

        public void Destroy(Entity entity)
        {
            InternalCalls.Entity_Destroy(entity.ID);
        }

        public void Destroy()
        {
            InternalCalls.Entity_Destroy(ID);
        }
    }
}
