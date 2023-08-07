
using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace Fox
{
    [StructLayout(LayoutKind.Sequential)]
	public struct Vector3 : IEquatable<Vector3>
	{
		public float x;
		public float y;
		public float z;

		public const float kEpsilonNormalSqrt = 1e-15F;
		
		public float magnitude => (float)Math.Sqrt(x * x + y * y + z * z);
		public float sqrMagnitude => x * x + y * y + z * z;
		public Vector3 normalized => this / magnitude;

		public static Vector3 one => new Vector3(1.0f);
		public static Vector3 zero => new Vector3(0.0f);

		#region Constructors

		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public Vector3(in float scalar)
		{
			x = y = z = scalar;
		}

		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public Vector3(in float x, in float y, in float z)
		{
			this.x = x;
			this.y = y;
			this.z = z;
		}

		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public Vector3(in Vector2 xy, in float z)
		{
			x = xy.x;
			y = xy.y;
			this.z = z;
		}

		#endregion

		#region OverloadedOperators

		public float this[int index]
		{
			[MethodImpl(MethodImplOptions.AggressiveInlining)]
			get
			{
				switch (index)
				{
					case 0:
						return x;
					case 1:
						return y;
					case 2:
						return z;
					default:
						throw new IndexOutOfRangeException();
				};
			}
			[MethodImpl(MethodImplOptions.AggressiveInlining)]
			set
			{
				switch (index)
				{
					case 0: x = value; break;
					case 1: y = value; break;
					case 2: z = value; break;
					default: throw new IndexOutOfRangeException();
				}
			}
		}

		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public static Vector3 operator +(in Vector3 left, in Vector3 right) { return new Vector3(left.x + right.x, left.y + right.y, left.z + right.z); }
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public static Vector3 operator -(in Vector3 left, in Vector3 right) { return new Vector3(left.x - right.x, left.y - right.y, left.z - right.z); }
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public static Vector3 operator *(in Vector3 left, in Vector3 right) { return new Vector3(left.x * right.x, left.y * right.y, left.z * right.z); }
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public static Vector3 operator *(in Vector3 left, in float scalar) { return new Vector3(left.x * scalar, left.y * scalar, left.z * scalar); }
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public static Vector3 operator *(in float scalar, in Vector3 right) { return new Vector3(scalar * right.x, scalar * right.y, scalar * right.z); }
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public static Vector3 operator /(in Vector3 left, in Vector3 right) { return new Vector3(left.x / right.x, left.y / right.y, left.z / right.z); }
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public static Vector3 operator /(in Vector3 left, in float scalar) { return new Vector3(left.x / scalar, left.y / scalar, left.z / scalar); }

		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public static implicit operator Vector2(in Vector3 v) { return new Vector2(v.x, v.y); }

		#endregion

		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public Vector3 Normalize()
		{
			this = normalized;
			return this;
		}

		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public override string ToString() { return "Vector3(" + x + ", " + y + ", " + z + ")"; }

		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public static float Dot(in Vector3 v1, in Vector3 v2) => v1.magnitude * v2.magnitude * Mathfs.Acos(Mathfs.AngleBetween(v1, v2));

		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public static Vector3 Cross(in Vector3 v1, in Vector3 v2)
		{
			return new Vector3((v1.y * v2.z) - (v1.z * v2.y),
				-(v1.x * v2.z) - (v1.z * v2.x),
				(v1.x * v2.y) - (v1.y * v2.x));
		}

		// Linearly interpolates between two vectors without clamping the interpolant
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public static Vector3 LerpUnclamped(Vector3 a, Vector3 b, float t)
		{
			return new Vector3(
				a.x + (b.x - a.x) * t,
				a.y + (b.y - a.y) * t,
				a.z + (b.z - a.z) * t
			);
		}
		
		// Linearly interpolates between two vectors.
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector3 Lerp(Vector3 a, Vector3 b, float t)
        {
            t = Mathfs.Clamp01(t);
            return new Vector3(
                a.x + (b.x - a.x) * t,
                a.y + (b.y - a.y) * t,
                a.z + (b.z - a.z) * t
            );
        }
        
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector3 MoveTowards(Vector3 current, Vector3 target, float maxDistanceDelta)
        {
            // avoid vector ops because current scripting backends are terrible at inlining
            float toVector_x = target.x - current.x;
            float toVector_y = target.y - current.y;
            float toVector_z = target.z - current.z;

            float sqdist = toVector_x * toVector_x + toVector_y * toVector_y + toVector_z * toVector_z;

            if (sqdist == 0 || (maxDistanceDelta >= 0 && sqdist <= maxDistanceDelta * maxDistanceDelta))
                return target;
            var dist = (float)Math.Sqrt(sqdist);

            return new Vector3(current.x + toVector_x / dist * maxDistanceDelta,
                current.y + toVector_y / dist * maxDistanceDelta,
                current.z + toVector_z / dist * maxDistanceDelta);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector3 SmoothDamp(Vector3 current, Vector3 target, ref Vector3 currentVelocity, float smoothTime, float maxSpeed)
        {
            float deltaTime = Time.deltaTime;
            return SmoothDamp(current, target, ref currentVelocity, smoothTime, maxSpeed, deltaTime);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector3 SmoothDamp(Vector3 current, Vector3 target, ref Vector3 currentVelocity, float smoothTime)
        {
            float deltaTime = Time.deltaTime;
            float maxSpeed = Mathfs.Infinity;
            return SmoothDamp(current, target, ref currentVelocity, smoothTime, maxSpeed, deltaTime);
        }

        // Gradually changes a vector towards a desired goal over time.
        public static Vector3 SmoothDamp(Vector3 current, Vector3 target, ref Vector3 currentVelocity, float smoothTime, float maxSpeed, float deltaTime)
        {
            float output_x = 0f;
            float output_y = 0f;
            float output_z = 0f;

            // Based on Game Programming Gems 4 Chapter 1.10
            smoothTime = Mathfs.Max(0.0001F, smoothTime);
            float omega = 2F / smoothTime;

            float x = omega * deltaTime;
            float exp = 1F / (1F + x + 0.48F * x * x + 0.235F * x * x * x);

            float change_x = current.x - target.x;
            float change_y = current.y - target.y;
            float change_z = current.z - target.z;
            Vector3 originalTo = target;

            // Clamp maximum speed
            float maxChange = maxSpeed * smoothTime;

            float maxChangeSq = maxChange * maxChange;
            float sqrmag = change_x * change_x + change_y * change_y + change_z * change_z;
            if (sqrmag > maxChangeSq)
            {
                var mag = (float)Math.Sqrt(sqrmag);
                change_x = change_x / mag * maxChange;
                change_y = change_y / mag * maxChange;
                change_z = change_z / mag * maxChange;
            }

            target.x = current.x - change_x;
            target.y = current.y - change_y;
            target.z = current.z - change_z;

            float temp_x = (currentVelocity.x + omega * change_x) * deltaTime;
            float temp_y = (currentVelocity.y + omega * change_y) * deltaTime;
            float temp_z = (currentVelocity.z + omega * change_z) * deltaTime;

            currentVelocity.x = (currentVelocity.x - omega * temp_x) * exp;
            currentVelocity.y = (currentVelocity.y - omega * temp_y) * exp;
            currentVelocity.z = (currentVelocity.z - omega * temp_z) * exp;

            output_x = target.x + (change_x + temp_x) * exp;
            output_y = target.y + (change_y + temp_y) * exp;
            output_z = target.z + (change_z + temp_z) * exp;

            // Prevent overshooting
            float origMinusCurrent_x = originalTo.x - current.x;
            float origMinusCurrent_y = originalTo.y - current.y;
            float origMinusCurrent_z = originalTo.z - current.z;
            float outMinusOrig_x = output_x - originalTo.x;
            float outMinusOrig_y = output_y - originalTo.y;
            float outMinusOrig_z = output_z - originalTo.z;

            if (origMinusCurrent_x * outMinusOrig_x + origMinusCurrent_y * outMinusOrig_y + origMinusCurrent_z * outMinusOrig_z > 0)
            {
                output_x = originalTo.x;
                output_y = originalTo.y;
                output_z = originalTo.z;

                currentVelocity.x = (output_x - originalTo.x) / deltaTime;
                currentVelocity.y = (output_y - originalTo.y) / deltaTime;
                currentVelocity.z = (output_z - originalTo.z) / deltaTime;
            }

            return new Vector3(output_x, output_y, output_z);
        }
        
        // Reflects a vector off the plane defined by a normal.
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector3 Reflect(Vector3 inDirection, Vector3 inNormal)
        {
	        float factor = -2F * Dot(inNormal, inDirection);
	        return new Vector3(factor * inNormal.x + inDirection.x,
		        factor * inNormal.y + inDirection.y,
		        factor * inNormal.z + inDirection.z);
        }
        
        // Projects a vector onto another vector.
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector3 Project(Vector3 vector, Vector3 onNormal)
        {
	        float sqrMag = Dot(onNormal, onNormal);
	        if (sqrMag < Mathfs.Epsilon)
		        return zero;
	        else
	        {
		        var dot = Dot(vector, onNormal);
		        return new Vector3(onNormal.x * dot / sqrMag,
			        onNormal.y * dot / sqrMag,
			        onNormal.z * dot / sqrMag);
	        }
        }

        // Projects a vector onto a plane defined by a normal orthogonal to the plane.
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector3 ProjectOnPlane(Vector3 vector, Vector3 planeNormal)
        {
	        float sqrMag = Dot(planeNormal, planeNormal);
	        if (sqrMag < Mathfs.Epsilon)
		        return vector;
	        else
	        {
		        var dot = Dot(vector, planeNormal);
		        return new Vector3(vector.x - planeNormal.x * dot / sqrMag,
			        vector.y - planeNormal.y * dot / sqrMag,
			        vector.z - planeNormal.z * dot / sqrMag);
	        }
        }
        
        // Returns the angle in degrees between /from/ and /to/. This is always the smallest
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static float Angle(Vector3 from, Vector3 to)
        {
	        // sqrt(a) * sqrt(b) = sqrt(a * b) -- valid for real numbers
	        float denominator = (float)Math.Sqrt(from.sqrMagnitude * to.sqrMagnitude);
	        if (denominator < kEpsilonNormalSqrt)
		        return 0F;

	        float dot = Mathfs.Clamp(Dot(from, to) / denominator, -1F, 1F);
	        return ((float)Math.Acos(dot)) * Mathfs.Rad2Deg;
        }
        
        // Returns the distance between /a/ and /b/.
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static float Distance(Vector3 a, Vector3 b)
        {
	        float diff_x = a.x - b.x;
	        float diff_y = a.y - b.y;
	        float diff_z = a.z - b.z;
	        return (float)Math.Sqrt(diff_x * diff_x + diff_y * diff_y + diff_z * diff_z);
        }


		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public bool Equals(Vector3 other)
		{
			return x == other.x && y == other.y && z == other.z;
		}
		
		// also required for being able to use Vector3s as keys in hash tables
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public override bool Equals(object other)
		{
			if (!(other is Vector3)) return false;

			return Equals((Vector3)other);
		}
		
		// used to allow Vector3s to be used as keys in hash tables
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public override int GetHashCode()
		{
			return x.GetHashCode() ^ (y.GetHashCode() << 2) ^ (z.GetHashCode() >> 2);
		}
	}
}