#pragma once
#ifdef EAE6320_PLATFORM_D3D
#include <Engine/Graphics/Direct3D/Includes.h>
#endif
#ifdef EAE6320_PLATFORM_GL
#include <Engine/Graphics/OpenGL/Includes.h>
#endif
#include <Engine/Graphics/cRenderState.h>
#include <Engine/Graphics/cShader.h>
#include <Engine/Assets/ReferenceCountedAssets.h>
#include "Configuration.h"
#include <Engine/Graphics/Graphics.h>
#include <Engine/ScopeGuard/cScopeGuard.h>

namespace eae6320
{
	namespace Graphics
	{
		class cEffect
		{
		public:
			EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS()

#ifdef EAE6320_PLATFORM_GL
			GLuint s_programId = 0;
			cResult CleanUpProgram();
			cResult InitProgram(cResult result);
#endif
			eae6320::Graphics::cShader* s_vertexShader = nullptr;
			eae6320::Graphics::cShader* s_fragmentShader = nullptr;

			eae6320::Graphics::cRenderState s_renderState;

			static cResult Load(cEffect*& o_effect, std::string shaderPath);

			cEffect() = default;
			~cEffect();

			void Bind();
			eae6320::cResult InitializeShadingData(std::string shaderPath);
			cResult Initialize(const sInitializationParameters& i_initializationParameters, std::string shaderPath);
			cResult CleanUp();

			EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS(cEffect)
			EAE6320_ASSETS_DECLAREREFERENCECOUNT()

		};
	}
}