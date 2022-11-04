
#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;                  // (x,y,z)
uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

varying vec2 pos;
uniform float u_z; //depth position

void main()
{
    vec4 object_space_pos = vec4( a_Position.x, a_Position.y, u_z, 1.0);
    gl_Position = u_ViewProjection * u_Transform * object_space_pos;
    pos = a_Position.xy;
}

#type fragment
#version 330 core

// layout(location = 0) out vec4 color;

// uniform vec4 u_Color;

//void main()
//{
//	color = u_Color;
//}

varying vec2 pos; //current pixel position
uniform vec2 u_pos; //light source positon

const float zz = 32.; //larger zz, larger light

void main()
{
    vec2 dis = pos - u_pos;
    float str = 1./(sqrt(dis.x * dis.x + dis.y * dis.y + zz*zz)-zz); //strength of light is the inverse distance
    gl_FragColor = vec4(vec3(str),1.);
}
