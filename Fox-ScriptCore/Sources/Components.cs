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
            set
            {
                Vector3 tempRotation = value * Mathfs.Deg2Rad;
                InternalCalls.TransformComponent_SetRotation(Entity.ID, ref tempRotation);
            }
        }

        public Vector3 right
        {
            get
            {
                InternalCalls.TransformComponent_GetRight(Entity.ID, out Vector3 v);
                return v;
            }
        }

        public Vector3 forward
        {
            get
            {
                InternalCalls.TransformComponent_GetForward(Entity.ID, out Vector3 v);
                return v;
            }
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

    public class CircleCollider2D : Collider2D
    {
    }
    #endregion
    
#region Animator

    public class Animator : Component
    {
        public ulong HashString(string eventName)
        {
            return InternalCalls.Animator_GetHashID(eventName);
        }

        public void Subscribe(string eventName, InternalCalls.AnimatorEvent callback)
        {
            ulong eventID = HashString(eventName);
            Subscribe(eventID, callback);
        }
        
        public void Subscribe(ulong eventID, InternalCalls.AnimatorEvent callback)
        {
            InternalCalls.AnimatorEvent myDelegate = null;
            InternalCalls.Animator_GetDelegate(Entity.ID, eventID, ref myDelegate);
            if (myDelegate != null)
            {
                myDelegate += callback;
                InternalCalls.Animator_UpdateDelegate(Entity.ID, eventID, myDelegate);
            }
            else
            {
                myDelegate = callback;
                InternalCalls.Animator_SubscribeToEvent(Entity.ID, eventID, myDelegate);
            }
        }
        
        public void Unsubscribe(string eventName, InternalCalls.AnimatorEvent callback)
        {
            InternalCalls.AnimatorEvent myDelegate = null;

            ulong eventID = HashString(eventName);
            Unsubscribe(eventID, callback);
        }
        
        public void Unsubscribe(ulong eventID, InternalCalls.AnimatorEvent callback)
        {
            InternalCalls.AnimatorEvent myDelegate = null;
            InternalCalls.Animator_GetDelegate(Entity.ID, eventID, ref myDelegate);
            if (myDelegate != null)
            {
                myDelegate -= callback;
                InternalCalls.Animator_UpdateDelegate(Entity.ID, eventID, myDelegate);
            }
        }
    }
    #endregion

    #region Transform
    public class CameraComponent : Component
    {
        public Vector3 ScreenToWorld(Vector2 screenPos)
        {
            InternalCalls.Camera_ScreenToWorld(Entity.ID, ref screenPos, out Vector3 worldPos);
            return worldPos;
        }
    }
    #endregion
}
