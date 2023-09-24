#pragma once
#include <Engine/Results/cResult.h>
#include <Engine/Graphics/cVertexFormat.h>
#include <Engine/Graphics/VertexFormats.h>
#include <Engine/Assets/ReferenceCountedAssets.h>
#include "Configuration.h"
#include <Engine/Graphics/Graphics.h>

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
		class swapBuffers
		{
		public:
			void bufferSwap();
		};
	}
}