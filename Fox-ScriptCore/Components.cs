using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.CompilerServices;

namespace Fox
{
    public abstract class Component
    {
        public Entity Entity { get; internal set; }
    }

#region Name Component
    /// <summary>
    /// Contains name of the entity.
    /// </summary>
    public class NameComponent : Component
    {
    	/// <summary>
    	/// Entity name.
    	/// </summary>
    	public string Name
    	{
    		get => InternalCalls.NameComponent_GetName(Entity.ID);
    		set => InternalCalls.NameComponent_SetName(Entity.ID, value);
    	}
    }
#endregion

#region Transform
    public class TransformComponent : Component
    {
        /// <summary>
        /// World space position.
        /// </summary>
        public Vector3 position
        {
            get
            {
                InternalCalls.TransformComponent_GetTranslation(Entity.ID, out Vector3 translation);
                return translation;
            }
            set
            {
                InternalCalls.TransformComponent_SetTranslation(Entity.ID, ref value);
            }
        }

        /// <summary>
        /// World space rotation in euler angles (radians).
        /// </summary>
        public Vector3 rotation
        {
            get
            {
                InternalCalls.TransformComponent_GetRotation(Entity.ID, out Vector3 v);
                return v;
            }
            set => InternalCalls.TransformComponent_SetRotation(Entity.ID, ref value);
        }

        /// <summary>
        /// World space scale.
        /// </summary>
        public Vector3 scale
        {
            get
            {
                InternalCalls.TransformComponent_GetScale(Entity.ID, out Vector3 v);
                return v;
            }
            set => InternalCalls.TransformComponent_SetScale(Entity.ID, ref value);
        }
    }
#endregion

#region Rigidbody2D
    public class Rigidbody2D : Component
    {
        /// <summary>
		/// Define the degree to which the Entity is affected by gravity.
		/// </summary>
		public float gravityScale
		{
			[MethodImpl(MethodImplOptions.AggressiveInlining)]
			get
			{
				InternalCalls.Rigidbody2DComponent_GetGravityScale(Entity.ID, out float v);
				return v;
			}
			[MethodImpl(MethodImplOptions.AggressiveInlining)]
			set => InternalCalls.Rigidbody2DComponent_SetGravityScale(Entity.ID, ref value);
		}

        public void ApplyLinearImpulse(Vector2 impulse, Vector2 worldPosition, bool wake)
        {
            InternalCalls.Rigidbody2DComponent_ApplyLinearImpulse(Entity.ID, ref impulse, ref worldPosition, wake);
        }

        public void ApplyLinearImpulse(Vector2 impulse, bool wake)
        {
            InternalCalls.Rigidbody2DComponent_ApplyLinearImpulseToCenter(Entity.ID, ref impulse, wake);
        }
    }
#endregion

#region Sprite Renderer
/// <summary>
    /// Renders a sprite
    /// </summary>
    public class SpriteRenderer : Component
    {
        /// <summary>
        /// Sprite tint color
        /// </summary>
        public Color color
        {
            get
            {
                InternalCalls.SpriteRendererComponent_GetColor(Entity.ID, out Color v);
                return v;
            }
            set => InternalCalls.SpriteRendererComponent_SetColor(Entity.ID, ref value);
        }

        /// <summary>
        /// Tiling factor for repeated tiling
        /// </summary>
        public float tilingFactor
        {
            get
            {
                InternalCalls.SpriteRendererComponent_GetTilingFactor(Entity.ID, out float v);
                return v;
            }
            set => InternalCalls.SpriteRendererComponent_SetTilingFactor(Entity.ID, ref value);
        }
    }
    #endregion
  
#region Colliders

    public class Collider2D : Component
    {
        // public delegate void OnTriggerEnterEvent();
        // public event OnTriggerEnterEvent OnTriggerEnter;
        // // public Action<Collider2D> OnTriggerExit;
        // // public Action<Collider2D> OnTriggerStay;
        // internal Collider2D()
        // {
        //     OnTriggerEnter = () => { Console.WriteLine("Delegate called !!"); };
        //     this.Collider_RegisterTriggerEvent();
        // }
        //
        // public void Internal_OnTriggerEnter()
        // {
        //     OnTriggerEnter?.Invoke();
        // }
    }

    public class BoxCollider2D : Collider2D
    {
    }
    #endregion
    
#region Animator

    public class Animator : Component
    {
        public delegate void AnimatorEventDelegate();

        private Dictionary<ulong, AnimatorEventDelegate> Events = new Dictionary<ulong, AnimatorEventDelegate>();

        private void OnEventCb(ulong eventID)
        {
            if (Events.ContainsKey(eventID))
            {
                Events[eventID]?.Invoke();
            }
        }

        public void On(string eventName, AnimatorEventDelegate callback)
        {
            ulong id = InternalCalls.Animator_GetHashID(eventName);
            if (Events.ContainsKey(id))
            {
                Events[id] += callback;
            }
            else
            {
                Events[id] = callback;
                InternalCalls.Animator_SubscribeToEvent(Entity.ID, id, OnEventCb);
            }
        }
        
        public void RemoveCb(string eventName, AnimatorEventDelegate callback)
        {
            ulong id = InternalCalls.Animator_GetHashID(eventName);
            if (Events.ContainsKey(id))
            {
                Events[id] -= callback;
            }
        }
    }
#endregion
}
