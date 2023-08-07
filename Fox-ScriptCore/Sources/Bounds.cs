using System.Runtime.InteropServices;

namespace Fox
{
    [StructLayout(LayoutKind.Sequential)]
    public struct Bounds
    {
        public Vector2 center;
        public Vector2 size;

        public Vector2 extents
        {
            get => size * 0.5f;
            set => size = value * 2.0f;
        }

        public Vector2 max
        {
            get => center + extents;
            set => center = value - extents;
        }

        public Vector2 min
        {
            get => center - extents;
            set => center = value + extents;
        }

        public Bounds(in Vector2 center, in Vector2 size)
        {
            this.center = center;
            this.size = size;
        }
    }
}