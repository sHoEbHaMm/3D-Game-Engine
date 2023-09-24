// Includes
//=========

#include <Engine/Graphics/Graphics.h>

#include <Engine/Graphics/OpenGL/Includes.h>
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
#include <Engine/Graphics/cColorRender.h>


void eae6320::Graphics::cColorRender::SetBuffer( float clearColor[4])
{

	{
		// Black is usually used
		{
			glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
			EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
		}
		{
			constexpr GLbitfield clearColor = GL_COLOR_BUFFER_BIT;
			glClear(clearColor);
			EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
		}
	}
	// In addition to the color buffer there is also a hidden image called the "depth buffer"
	// which is used to make it less important which order draw calls are made.
	// It must also be "cleared" every frame just like the visible color buffer.
	{
		{
			glDepthMask(GL_TRUE);
			EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
			constexpr GLclampd clearToFarDepth = 1.0;
			glClearDepth(clearToFarDepth);
			EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
		}
		{
			constexpr GLbitfield clearDepth = GL_DEPTH_BUFFER_BIT;
			glClear(clearDepth);
			EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
		}
	}
}

void eae6320::Graphics::cColorRender::CleanUp()
{

}

eae6320::cResult eae6320::Graphics::cColorRender::InitializeViews(const struct sInitializationParameters i_initializationParameters)
{
	auto result = Results::Success;
	return result;
}