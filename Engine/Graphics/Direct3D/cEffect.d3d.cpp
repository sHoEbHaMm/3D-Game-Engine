
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
#include <Engine/Graphics/cEffect.h>

eae6320::cResult eae6320::Graphics::cEffect::Initialize(const sInitializationParameters& i_initializationParameters, std::string shaderPath)
{
	auto result = Results::Success;
	// Initialize the shading data
	{
		if (!(result = InitializeShadingData(shaderPath)))
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without the shading data");
			return result;
		}
	}

	return result;
}

void eae6320::Graphics::cEffect::Bind()
{
	// Bind the shading data
	{
		auto* const direct3dImmediateContext = sContext::g_context.direct3dImmediateContext;
		EAE6320_ASSERT(direct3dImmediateContext);
		{
			constexpr ID3D11ClassInstance* const* noInterfaces = nullptr;
			constexpr unsigned int interfaceCount = 0;
			// Vertex shader
			{
				EAE6320_ASSERT((s_vertexShader != nullptr) && (s_vertexShader->m_shaderObject.vertex != nullptr));
				direct3dImmediateContext->VSSetShader(s_vertexShader->m_shaderObject.vertex, noInterfaces, interfaceCount);
			}
			// Fragment shader
			{
				EAE6320_ASSERT((s_fragmentShader != nullptr) && (s_fragmentShader->m_shaderObject.vertex != nullptr));
				direct3dImmediateContext->PSSetShader(s_fragmentShader->m_shaderObject.fragment, noInterfaces, interfaceCount);
			}
		}
		// Render state
		{
			s_renderState.Bind();
		}
	}
}

eae6320::cResult eae6320::Graphics::cEffect::InitializeShadingData(std::string shaderPath)
{
	auto result = eae6320::Results::Success;

	if (!(result = eae6320::Graphics::cShader::Load("data/Shaders/Vertex/standard.shader",
		s_vertexShader, eae6320::Graphics::eShaderType::Vertex)))
	{
		EAE6320_ASSERTF(false, "Can't initialize shading data without vertex shader");
		return result;
	}
	if (!(result = eae6320::Graphics::cShader::Load(shaderPath,
		s_fragmentShader, eae6320::Graphics::eShaderType::Fragment)))
	{
		EAE6320_ASSERTF(false, "Can't initialize shading data without fragment shader");
		return result;
	}
	{
		constexpr auto renderStateBits = []
		{
			uint8_t renderStateBits = 0;

			eae6320::Graphics::RenderStates::DisableAlphaTransparency(renderStateBits);
			eae6320::Graphics::RenderStates::DisableDepthTesting(renderStateBits);
			eae6320::Graphics::RenderStates::DisableDepthWriting(renderStateBits);
			eae6320::Graphics::RenderStates::DisableDrawingBothTriangleSides(renderStateBits);

			return renderStateBits;
		}();
		if (!(result = s_renderState.Initialize(renderStateBits)))
		{
			EAE6320_ASSERTF(false, "Can't initialize shading data without render state");
			return result;
		}
	}

	return result;
}

eae6320::cResult eae6320::Graphics::cEffect::CleanUp()
{
	auto result = Results::Success;

	if (s_vertexShader)
	{
		s_vertexShader->DecrementReferenceCount();
		s_vertexShader = nullptr;
	}
	if (s_fragmentShader)
	{
		s_fragmentShader->DecrementReferenceCount();
		s_fragmentShader = nullptr;
	}

	return result;
}