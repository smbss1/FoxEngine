
#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;                  // (x,y,z)
uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

uniform vec2 u_pos; //light source positon
uniform vec2 u_radius; //light radius
uniform float u_z; //depth position

void main()
{
    vec2 pos = a_Position.xy;
    vec2 dis = pos - u_pos;

    if (dis < u_radius)
    {
        //check if vertex requires repositioning
         pos += dis/sqrt(dis.x*dis.x + dis.y*dis.y) * 100000.; //repositioning the vertex with respect to the light position
    }
    vec4 object_space_pos = vec4( pos.x, pos.y, u_z - 0.5, 1.0); //shadow is drawn at a z-value closer to the screen than its corresponding light.
    gl_Position = u_ViewProjection * u_Transform * object_space_pos;
}

#type fragment
#version 330 core

void main()
{
    gl_FragColor = vec4(0.); //draws an invisible shadow that can block the light when Z-buffer is on
}
