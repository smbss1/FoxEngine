using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Fox
{
    public abstract class Component
    {
        public Entity Entity { get; internal set; }
    }
    
    /// <summary>
    /// Contains name of the entity.
    /// </summary>
    public class EntityName : Component
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

    public class TransformComponent : Component
    {
        public Vector3 Translation
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
    }

    public class Rigidbody2D : Component
    {
        public void ApplyLinearImpulse(Vector2 impulse, Vector2 worldPosition, bool wake)
        {
            InternalCalls.Rigidbody2DComponent_ApplyLinearImpulse(Entity.ID, ref impulse, ref worldPosition, wake);
        }

        public void ApplyLinearImpulse(Vector2 impulse, bool wake)
        {
            InternalCalls.Rigidbody2DComponent_ApplyLinearImpulseToCenter(Entity.ID, ref impulse, wake);
        }
    }
    
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
}