// Basic Texture Shader

#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in float a_TilingFactor;
layout(location = 5) in int a_EntityID;

// uniform vec3 transform;
// uniform vec4 a_Color;
// uniform float a_TexIndex;
// uniform float a_TilingFactor;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
};

struct VertexOutput
{
	vec4 Color;
	vec2 TexCoord;
	float TilingFactor;
};

layout (location = 0) out VertexOutput Output;
layout (location = 3) out flat float v_TexIndex;
layout (location = 4) out flat int v_EntityID;

//layout (location = 5) out vec3 v_lightPos;
//out vec3 ex_pos;
//vec3 lightPos = vec3(0, 0, 0);

void main()
{
	Output.Color = a_Color;
	Output.TexCoord = a_TexCoord;
	Output.TilingFactor = a_TilingFactor;
	v_TexIndex = a_TexIndex;
	v_EntityID = a_EntityID;

//	v_lightPos = lightPos;
//	ex_pos = a_Position;

	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;

struct VertexOutput
{
	vec4 Color;
	vec2 TexCoord;
	float TilingFactor;
};

layout (location = 0) in VertexOutput Input;
layout (location = 3) in flat float v_TexIndex;
layout (location = 4) in flat int v_EntityID;

layout (binding = 0) uniform sampler2D u_Textures[32];


//struct Light
//{
//	vec2 position;
//	vec3 color;
//	float intensity;
//};

//uniform vec3 lightColor;
//uniform vec3 lightPos;

//uniform int lightCount;
//uniform Light lights[256];

//layout (location = 5) out vec3 v_lightPos;
//in vec3 ex_pos;

//float constant = 1.0f;
//float linear = 0.09f;
//float quadratic = 0.032f;
//vec3 lightColor = vec3(1, 0, 0);
//float intensity = 2.0f;
//float radius = 5;

void main()
{
	vec4 texColor = Input.Color;
	switch(int(v_TexIndex))
	{
		case  0: texColor *= texture(u_Textures[ 0], Input.TexCoord * Input.TilingFactor); break;
		case  1: texColor *= texture(u_Textures[ 1], Input.TexCoord * Input.TilingFactor); break;
		case  2: texColor *= texture(u_Textures[ 2], Input.TexCoord * Input.TilingFactor); break;
		case  3: texColor *= texture(u_Textures[ 3], Input.TexCoord * Input.TilingFactor); break;
		case  4: texColor *= texture(u_Textures[ 4], Input.TexCoord * Input.TilingFactor); break;
		case  5: texColor *= texture(u_Textures[ 5], Input.TexCoord * Input.TilingFactor); break;
		case  6: texColor *= texture(u_Textures[ 6], Input.TexCoord * Input.TilingFactor); break;
		case  7: texColor *= texture(u_Textures[ 7], Input.TexCoord * Input.TilingFactor); break;
		case  8: texColor *= texture(u_Textures[ 8], Input.TexCoord * Input.TilingFactor); break;
		case  9: texColor *= texture(u_Textures[ 9], Input.TexCoord * Input.TilingFactor); break;
		case 10: texColor *= texture(u_Textures[10], Input.TexCoord * Input.TilingFactor); break;
		case 11: texColor *= texture(u_Textures[11], Input.TexCoord * Input.TilingFactor); break;
		case 12: texColor *= texture(u_Textures[12], Input.TexCoord * Input.TilingFactor); break;
		case 13: texColor *= texture(u_Textures[13], Input.TexCoord * Input.TilingFactor); break;
		case 14: texColor *= texture(u_Textures[14], Input.TexCoord * Input.TilingFactor); break;
		case 15: texColor *= texture(u_Textures[15], Input.TexCoord * Input.TilingFactor); break;
		case 16: texColor *= texture(u_Textures[16], Input.TexCoord * Input.TilingFactor); break;
		case 17: texColor *= texture(u_Textures[17], Input.TexCoord * Input.TilingFactor); break;
		case 18: texColor *= texture(u_Textures[18], Input.TexCoord * Input.TilingFactor); break;
		case 19: texColor *= texture(u_Textures[19], Input.TexCoord * Input.TilingFactor); break;
		case 20: texColor *= texture(u_Textures[20], Input.TexCoord * Input.TilingFactor); break;
		case 21: texColor *= texture(u_Textures[21], Input.TexCoord * Input.TilingFactor); break;
		case 22: texColor *= texture(u_Textures[22], Input.TexCoord * Input.TilingFactor); break;
		case 23: texColor *= texture(u_Textures[23], Input.TexCoord * Input.TilingFactor); break;
		case 24: texColor *= texture(u_Textures[24], Input.TexCoord * Input.TilingFactor); break;
		case 25: texColor *= texture(u_Textures[25], Input.TexCoord * Input.TilingFactor); break;
		case 26: texColor *= texture(u_Textures[26], Input.TexCoord * Input.TilingFactor); break;
		case 27: texColor *= texture(u_Textures[27], Input.TexCoord * Input.TilingFactor); break;
		case 28: texColor *= texture(u_Textures[28], Input.TexCoord * Input.TilingFactor); break;
		case 29: texColor *= texture(u_Textures[29], Input.TexCoord * Input.TilingFactor); break;
		case 30: texColor *= texture(u_Textures[30], Input.TexCoord * Input.TilingFactor); break;
		case 31: texColor *= texture(u_Textures[31], Input.TexCoord * Input.TilingFactor); break;
	}

	if (texColor.a == 0.0)
		discard;

//	float dist = length(v_lightPos - ex_pos) / radius;
//	float att = 1 / (constant + linear * dist + quadratic * dist * dist);
//
////	att *= clamp(1.0 - dist, 0.0, 1.0);
//	//	att *= att;
//	vec3 diffuse = clamp(att * lightColor, 0.0, 1.0);
//	vec4 finalcolor = vec4(diffuse * intensity, 1.0);
//	texColor *= finalcolor;


//	float dist = distance(v_lightPos, ex_pos);
//	float attenuation = 1.0 / (1.0 + 0.1*dist + 0.01*dist*dist);


//	vec3 lAtt = vec3(0.0001, 0.0001, 0.001);
//	vec2 uv = gl_FragCoord.xy;
//	vec4 outc = vec4(0.0);
//	float lightIntensity = 1.0f;
//	vec3 lightColor = vec3(1, 0, 0);

//	for (int i = 0; i < lightCount; i++)
//	{
//		float dist = distance(lightPos, uv);
//		float att = 1.0 / (lAtt.x + lAtt.y * dist + lAtt.z * dist * dist);
//		outc += vec4(vec3(att), 1.0) * lightIntensity * vec4(lightColor, 1.0);
//	}

//	o_Color = vec4(vec3(attenuation), 1.0);
	o_Color = texColor;
	o_EntityID = v_EntityID;
}