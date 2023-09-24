#pragma once
#include <Engine/Graphics/Graphics.h>

#include <Engine/Graphics/cConstantBuffer.h>
#include <Engine/Graphics/ConstantBufferFormats.h>
#include <Engine/Graphics/cRenderState.h>
#include <Engine/Graphics/cShader.h>
#include <Engine/Graphics/sContext.h>
#include <Engine/Graphics/VertexFormats.h>
#include <Engine/Asserts/Asserts.h>
#include <Engine/Concurrency/cEvent.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Platform/Platform.h>
#include <Engine/ScopeGuard/cScopeGuard.h>
#include <Engine/Time/Time.h>
#include <Engine/UserOutput/UserOutput.h>
#include <new>
#include <utility>
#include <Engine/Graphics/cMesh.h>
#include <Engine/Graphics/cEffect.h>

namespace eae6320
{
	namespace Graphics
	{
		class cColorRender
		{
		public:
#ifdef EAE6320_PLATFORM_D3D
			// In Direct3D "views" are objects that allow a texture to be used a particular way:
			// A render target view allows a texture to have color rendered to it
			ID3D11RenderTargetView* s_renderTargetView = nullptr;
			// A depth/stencil view allows a texture to have depth rendered to it
			ID3D11DepthStencilView* s_depthStencilView = nullptr;
#endif
			struct sDataRequiredToRenderAFrame
			{
				eae6320::Graphics::ConstantBufferFormats::sFrame constantData_frame;
			};

			sDataRequiredToRenderAFrame s_dataRequiredToRenderAFrame[2];
			

			void SetBuffer( float color[4]);
			void CleanUp();
			eae6320::cResult InitializeViews(const struct sInitializationParameters i_initializationParameters);
		};
	}
}