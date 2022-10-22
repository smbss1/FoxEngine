namespace Fox
{
using System.Runtime.CompilerServices;

	/// <summary>
	/// Class to handle logging
	/// </summary>
	public static class Log
	{
		internal enum LogLevel
		{
			Info		= (1 << 0),
			Warn		= (1 << 1),
			Error		= (1 << 2),
		}

	#region Info Overloads

		/// <summary>
		/// Log Info.
		/// </summary>
		/// <param name="format">String format</param>
		/// <param name="parameters">Format parameters</param>
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public static void Info(string format, params object[] parameters) =>InternalCalls.Log_LogMessage(LogLevel.Info, string.Format(format, parameters));

		/// <summary>
		/// Log Info.
		/// </summary>
		/// <param name="value">Object type that implements IEquatable</param>
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public static void Info(object value) => InternalCalls.Log_LogMessage(LogLevel.Info, value != null ? value.ToString() : "null");

	#endregion

	#region Warn Overloads

		/// <summary>
		/// Log Warn.
		/// </summary>
		/// <param name="format">String format</param>
		/// <param name="parameters">Format parameters</param>
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public static void Warn(string format, params object[] parameters) =>InternalCalls.Log_LogMessage(LogLevel.Warn, string.Format(format, parameters));

		/// <summary>
		/// Log Warn.
		/// </summary>
		/// <param name="value">Object type that implements IEquatable</param>
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public static void Warn(object value) => InternalCalls.Log_LogMessage(LogLevel.Warn, value != null ? value.ToString() : "null");

	#endregion

	#region Error Overloads

		/// <summary>
		/// Log Error.
		/// </summary>
		/// <param name="format">String format</param>
		/// <param name="parameters">Format parameters</param>
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public static void Error(string format, params object[] parameters) =>InternalCalls.Log_LogMessage(LogLevel.Error, string.Format(format, parameters));

		/// <summary>
		/// Log Error.
		/// </summary>
		/// <param name="value">Object type that implements IEquatable</param>
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public static void Error(object value) => InternalCalls.Log_LogMessage(LogLevel.Error, value != null ? value.ToString() : "null");

	#endregion
	}
}