using System;
using System.Runtime.CompilerServices;

namespace Fox
{
	public static class InternalCalls
	{
	#region AssetHandle
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool AssetHandle_IsValid(ref AssetHandle assetHandle);
	#endregion

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static object GetScriptInstance(ulong entityID);

	#region Entity
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static bool Entity_HasComponent(ulong entityID, Type componentType);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static ulong Entity_FindEntityByName(string name);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static ulong Entity_Instantiate(ulong entityToInstantiate);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern ulong Entity_InstantiatePrefab(ref AssetHandle prefabHandle);
		
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern ulong Entity_InstantiatePrefabWithPosition(ref AssetHandle prefabHandle, ref Vector3 position);

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
		// [MethodImpl(MethodImplOptions.InternalCall)]
		// internal static extern void Rigidbody2DComponent_GetBodyType(ulong entityID, out int v);
		// [MethodImpl(MethodImplOptions.InternalCall)]
		// internal static extern void Rigidbody2DComponent_SetBodyType(ulong entityID, ref int v);
		// [MethodImpl(MethodImplOptions.InternalCall)]
		// internal static extern void Rigidbody2DComponent_GetAutoMass(ulong entityID, out bool v);
		// [MethodImpl(MethodImplOptions.InternalCall)]
		// internal static extern void Rigidbody2DComponent_SetAutoMass(ulong entityID, ref bool v);
		// [MethodImpl(MethodImplOptions.InternalCall)]
		// internal static extern void Rigidbody2DComponent_GetMass(ulong entityID, out float v);
		// [MethodImpl(MethodImplOptions.InternalCall)]
		// internal static extern void Rigidbody2DComponent_SetMass(ulong entityID, ref float v);
		// [MethodImpl(MethodImplOptions.InternalCall)]
		// internal static extern void Rigidbody2DComponent_GetLinearDrag(ulong entityID, out float v);
		// [MethodImpl(MethodImplOptions.InternalCall)]
		// internal static extern void Rigidbody2DComponent_SetLinearDrag(ulong entityID, ref float v);
		// [MethodImpl(MethodImplOptions.InternalCall)]
		// internal static extern void Rigidbody2DComponent_GetAngularDrag(ulong entityID, out float v);
		// [MethodImpl(MethodImplOptions.InternalCall)]
		// internal static extern void Rigidbody2DComponent_SetAngularDrag(ulong entityID, ref float v);
		// [MethodImpl(MethodImplOptions.InternalCall)]
		// internal static extern void Rigidbody2DComponent_GetAllowSleep(ulong entityID, out bool v);
		// [MethodImpl(MethodImplOptions.InternalCall)]
		// internal static extern void Rigidbody2DComponent_SetAllowSleep(ulong entityID, ref bool v);
		// [MethodImpl(MethodImplOptions.InternalCall)]
		// internal static extern void Rigidbody2DComponent_GetAwake(ulong entityID, out bool v);
		// [MethodImpl(MethodImplOptions.InternalCall)]
		// internal static extern void Rigidbody2DComponent_SetAwake(ulong entityID, ref bool v);
		// [MethodImpl(MethodImplOptions.InternalCall)]
		// internal static extern void Rigidbody2DComponent_GetContinuous(ulong entityID, out bool v);
		// [MethodImpl(MethodImplOptions.InternalCall)]
		// internal static extern void Rigidbody2DComponent_SetContinuous(ulong entityID, ref bool v);
		// [MethodImpl(MethodImplOptions.InternalCall)]
		// internal static extern void Rigidbody2DComponent_GetFreezeRotation(ulong entityID, out bool v);
		// [MethodImpl(MethodImplOptions.InternalCall)]
		// internal static extern void Rigidbody2DComponent_SetFreezeRotation(ulong entityID, ref bool v);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Rigidbody2DComponent_GetGravityScale(ulong entityID, out float v);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Rigidbody2DComponent_SetGravityScale(ulong entityID, ref float v);

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

	#region Animator

	public delegate void AnimatorEvent();
	[MethodImpl(MethodImplOptions.InternalCall)]
	internal extern static void Animator_SubscribeToEvent(ulong entityID, ulong eventID, AnimatorEvent callback);
	
	[MethodImpl(MethodImplOptions.InternalCall)]
	internal extern static ulong Animator_GetHashID(string eventName);
	
	[MethodImpl(MethodImplOptions.InternalCall)]
	internal extern static void Animator_UpdateDelegate(ulong entityID, ulong eventID, AnimatorEvent callback);
	
	[MethodImpl(MethodImplOptions.InternalCall)]
	internal extern static void Animator_GetDelegate(ulong entityID, ulong eventID, ref AnimatorEvent callback);
	
	#endregion
	
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static float Time_deltaTime();
	}
}
