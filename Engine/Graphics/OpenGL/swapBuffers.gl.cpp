#include <Engine/Graphics/cMesh.h>
#include <Engine/Graphics/cEffect.h>
#include <Engine/Graphics/swapBuffers.h>
#include <Engine/Results/Results.h>
#include <Engine/Graphics/sContext.h>
#include <Engine/Asserts/Asserts.h>
#include <Engine/Graphics/Direct3D/Includes.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Graphics/cConstantBuffer.h>
#include <Engine/Graphics/cShader.h>
#include <Engine/Graphics/ConstantBufferFormats.h>
#include <Engine/Concurrency/cEvent.h>
#include <Engine/UserOutput/UserOutput.h>

void eae6320::Graphics::swapBuffers::bufferSwap()
{
	{
		const auto deviceContext = sContext::g_context.deviceContext;
		EAE6320_ASSERT(deviceContext != NULL);
		const auto glResult = SwapBuffers(deviceContext);
		EAE6320_ASSERT(glResult != FALSE);
	}
}