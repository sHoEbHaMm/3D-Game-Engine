/*
	This file should be #included by all shaders
*/

//DataTypes
#if defined( EAE6320_PLATFORM_GL )
	#define vector4x4 mat4
	#define vector2 vec2
	#define vector3 vec3
	#define vector4 vec4
#endif

#if defined( EAE6320_PLATFORM_D3D )
	#define vector4x4 float4x4
	#define vector2 float2
	#define vector3 float3
	#define vector4 float4
#endif

// Version Information
//====================

#if defined( EAE6320_PLATFORM_GL )

	// GLSL shaders require the version to be #defined before anything else in the shader
	#version 420

#endif

// Constant Buffers
//=================

#if defined( EAE6320_PLATFORM_D3D )

	#define DeclareConstantBuffer( i_name, i_id ) cbuffer i_name : register( b##i_id )

#elif defined( EAE6320_PLATFORM_GL )

	#define DeclareConstantBuffer( i_name, i_id ) layout( std140, binding = i_id ) uniform i_name

#endif

//ConstantBuffers
DeclareConstantBuffer(g_constantBuffer_frame, 0)
{
	vector4x4 g_transform_worldToCamera;
	vector4x4 g_transform_cameraToProjected;

	float g_elapsedSecondCount_systemTime;
	float g_elapsedSecondCount_simulationTime;

	vector2 g_padding;
};
DeclareConstantBuffer(g_constantBuffer_drawCall, 2)
{
	vector4x4 g_transform_localToWorld;
};
