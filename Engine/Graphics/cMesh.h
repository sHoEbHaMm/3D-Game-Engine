#pragma once

#include <Engine/Results/cResult.h>
#include <Engine/Graphics/cVertexFormat.h>
#include <Engine/Graphics/VertexFormats.h>
#include <Engine/Assets/ReferenceCountedAssets.h>
#include "Configuration.h"
#include <Engine/Graphics/Graphics.h>
#include <Engine/ScopeGuard/cScopeGuard.h>

#ifdef EAE6320_PLATFORM_D3D
#include <Engine/Graphics/Direct3D/Includes.h>
#endif
#ifdef EAE6320_PLATFORM_GL
#include <Engine/Graphics/OpenGL/Includes.h>
#endif


namespace eae6320
{
	namespace Graphics
	{
		class cMesh
		{
		public:

			EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS()

#ifdef EAE6320_PLATFORM_D3D
			eae6320::Graphics::cVertexFormat* s_vertexFormat = nullptr;
			// A vertex buffer holds the data for each vertex
			ID3D11Buffer* s_vertexBuffer = nullptr;
			ID3D11Buffer* s_indexBuffer = nullptr;
#endif
#ifdef EAE6320_PLATFORM_GL
			// A vertex buffer holds the data for each vertex
			GLuint s_vertexBufferId = 0;
			GLuint s_indexBufferId = 0;
			// A vertex array encapsulates the vertex data as well as the vertex input layout
			GLuint s_vertexArrayId = 0;
			uint16_t* adjustWinding(uint16_t arr[]) ;
#endif
			cMesh() = default;
			~cMesh();
			cResult Initialize(const sInitializationParameters& i_initializationParameters, uint16_t index[], eae6320::Graphics::VertexFormats::sVertex_mesh vertexData[]);
			cResult CleanUp();

			static cResult Load(cMesh*& o_mesh, uint16_t index[], eae6320::Graphics::VertexFormats::sVertex_mesh vertexData[]);

			void Draw();
			
			eae6320::cResult InitializeGeometry(uint16_t index[], eae6320::Graphics::VertexFormats::sVertex_mesh vertexData[]);

			EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS(cMesh)
			EAE6320_ASSETS_DECLAREREFERENCECOUNT()
		};
	}
}