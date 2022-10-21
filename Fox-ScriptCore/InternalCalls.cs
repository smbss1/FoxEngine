using System;
using System.Runtime.CompilerServices;

namespace Fox
{
	public static class InternalCalls
	{
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static object GetScriptInstance(ulong entityID);
		
	#region Entity
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static bool Entity_HasComponent(ulong entityID, Type componentType);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static ulong Entity_FindEntityByName(string name);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static ulong Entity_Instantiate(ulong entityToInstantiate);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void Entity_Destroy(ulong entityToDestroy);
	#endregion

		
	#region Log
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Log_LogMessage(Log.LogLevel level, string formattedMessage);
	#endregion

	#region NameComponent
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static string NameComponent_GetName(ulong entityID);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void NameComponent_SetName(ulong entityID, string name);
	#endregion
		
	#region SpriteRenderer
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void SpriteRendererComponent_GetColor(ulong entityID, out Color tint);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void SpriteRendererComponent_SetColor(ulong entityID, ref Color tint);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void SpriteRendererComponent_GetTilingFactor(ulong entityID, out float tiling);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void SpriteRendererComponent_SetTilingFactor(ulong entityID, ref float tiling);
	#endregion
	
	#region Transform Component
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void TransformComponent_GetTranslation(ulong entityID, out Vector3 translation);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void TransformComponent_SetTranslation(ulong entityID, ref Vector3 translation);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void TransformComponent_GetRotation(ulong entityID, out Vector3 eulerAngles);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void TransformComponent_SetRotation(ulong entityID, ref Vector3 eulerAngles);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void TransformComponent_GetScale(ulong entityID, out Vector3 scale);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void TransformComponent_SetScale(ulong entityID, ref Vector3 scale);
	#endregion

	#region Rigidbody2D Component
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void Rigidbody2DComponent_ApplyLinearImpulse(ulong entityID, ref Vector2 impulse, ref Vector2 point, bool wake);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void Rigidbody2DComponent_ApplyLinearImpulseToCenter(ulong entityID, ref Vector2 impulse, bool wake);
	#endregion

	#region Input
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static bool Input_IsKeyPressed(KeyCode keycode);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static bool Input_IsKeyDown(KeyCode keycode);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static bool Input_IsKeyReleased(KeyCode keycode);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool Input_IsMouseButtonPressed(MouseCode button);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Input_GetMousePosition(out Vector2 mousePosition);
	#endregion
	}
}
