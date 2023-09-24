// Includes
//=========

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
#include <Engine/Graphics/cColorRender.h>
#include <Engine/Graphics/swapBuffers.h>

eae6320::Graphics::cConstantBuffer s_constantBuffer_frame(eae6320::Graphics::ConstantBufferTypes::Frame);
//eae6320::Graphics::cMesh* newMesh = new eae6320::Graphics::cMesh();
//eae6320::Graphics::cEffect* newEffect = new eae6320::Graphics::cEffect();
eae6320::Graphics::swapBuffers* bufferSwapper = new eae6320::Graphics::swapBuffers();
eae6320::Graphics::cColorRender* newRenderer = new eae6320::Graphics::cColorRender();
//uint16_t index[] = { 1,3,2 };
//eae6320::Graphics::VertexFormats::sVertex_mesh vertexData[] = { {0.0f,0.0f,0.0f},{1.0f,0.0f,0.0f},{1.0f,1.0f,0.0f},{0.0f,1.0f,0.0f} };

//eae6320::Graphics::cMesh* newMesh2 = new eae6320::Graphics::cMesh();
//eae6320::Graphics::cEffect* newEffect2 = new eae6320::Graphics::cEffect();
//eae6320::Graphics::cColorRender* newRenderer = new eae6320::Graphics::cColorRender();
//uint16_t index2[] = { 1,4,3 };
//eae6320::Graphics::VertexFormats::sVertex_mesh vertexData2[] = { {0.0f,0.0f,0.0f},{-1.0f,0.0f,0.0f},{-1.0f,-1.0f,0.0f},{0.0f,-1.0f,0.0f} };



namespace
{
	// Submission Data
//----------------

// This struct's data is populated at submission time;
// it must cache whatever is necessary in order to render a frame
	struct sDataRequiredToRenderAFrame
	{
		struct sMeshEffectPair
		{
			eae6320::Graphics::cMesh* o_mesh = nullptr;
			eae6320::Graphics::cEffect* o_effect = nullptr;
		};

		eae6320::Graphics::ConstantBufferFormats::sFrame constantData_frame;
		float bgColor[4];
		sMeshEffectPair meshEffectPair[20];
		size_t meshEffectPairCount = 20;
	};
	// In our class there will be two copies of the data required to render a frame:
	//	* One of them will be in the process of being populated by the data currently being submitted by the application loop thread
	//	* One of them will be fully populated and in the process of being rendered from in the render thread
	// (In other words, one is being produced while the other is being consumed)
	sDataRequiredToRenderAFrame s_dataRequiredToRenderAFrame[2];
	auto* s_dataBeingSubmittedByApplicationThread = &s_dataRequiredToRenderAFrame[0];
	auto* s_dataBeingRenderedByRenderThread = &s_dataRequiredToRenderAFrame[1];
	// The following two events work together to make sure that
	// the main/render thread and the application loop thread can work in parallel but stay in sync:
	// This event is signaled by the application loop thread when it has finished submitting render data for a frame
	// (the main/render thread waits for the signal)
	eae6320::Concurrency::cEvent s_whenAllDataHasBeenSubmittedFromApplicationThread;
	// This event is signaled by the main/render thread when it has swapped render data pointers.
	// This means that the renderer is now working with all the submitted data it needs to render the next frame,
	// and the application loop thread can start submitting data for the following frame
	// (the application loop thread waits for the signal)
	eae6320::Concurrency::cEvent s_whenDataForANewFrameCanBeSubmittedFromApplicationThread;
}

// Submission
//-----------

void eae6320::Graphics::SubmitElapsedTime(const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_simulationTime)
{
	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread);
	auto& constantData_frame = s_dataBeingSubmittedByApplicationThread->constantData_frame;
	constantData_frame.g_elapsedSecondCount_systemTime = i_elapsedSecondCount_systemTime;
	constantData_frame.g_elapsedSecondCount_simulationTime = i_elapsedSecondCount_simulationTime;
}

eae6320::cResult eae6320::Graphics::WaitUntilDataForANewFrameCanBeSubmitted(const unsigned int i_timeToWait_inMilliseconds)
{
	return Concurrency::WaitForEvent(s_whenDataForANewFrameCanBeSubmittedFromApplicationThread, i_timeToWait_inMilliseconds);
}

eae6320::cResult eae6320::Graphics::SignalThatAllDataForAFrameHasBeenSubmitted()
{
	return s_whenAllDataHasBeenSubmittedFromApplicationThread.Signal();
}

void eae6320::Graphics::RenderFrame()
// Wait for the application loop to submit data to be rendered
{
	if (Concurrency::WaitForEvent(s_whenAllDataHasBeenSubmittedFromApplicationThread))
	{
		// Switch the render data pointers so that
		// the data that the application just submitted becomes the data that will now be rendered
		std::swap(s_dataBeingSubmittedByApplicationThread, s_dataBeingRenderedByRenderThread);
		// Once the pointers have been swapped the application loop can submit new data
		if (!s_whenDataForANewFrameCanBeSubmittedFromApplicationThread.Signal())
		{
			EAE6320_ASSERTF(false, "Couldn't signal that new graphics data can be submitted");
			Logging::OutputError("Failed to signal that new render data can be submitted");
			UserOutput::Print("The renderer failed to signal to the application that new graphics data can be submitted."
				" The application is probably in a bad state and should be exited");
			return;
		}
	}
	else
	{
		EAE6320_ASSERTF(false, "Waiting for the graphics data to be submitted failed");
		Logging::OutputError("Waiting for the application loop to submit data to be rendered failed");
		UserOutput::Print("The renderer failed to wait for the application to submit data to be rendered."
			" The application is probably in a bad state and should be exited");
		return;
	}

	EAE6320_ASSERT(s_dataBeingRenderedByRenderThread);
	auto* const dataRequiredToRenderFrame = s_dataBeingRenderedByRenderThread;

	newRenderer->SetBuffer(s_dataRequiredToRenderAFrame->bgColor);

	// Update the frame constant buffer
	{
		// Copy the data from the system memory that the application owns to GPU memory
		auto& constantData_frame = dataRequiredToRenderFrame->constantData_frame;
		s_constantBuffer_frame.Update(&constantData_frame);
	}

	for (size_t i = 0; i < dataRequiredToRenderFrame->meshEffectPairCount; i++)
	{
		//Bind the effect
		if (dataRequiredToRenderFrame->meshEffectPair[i].o_effect != nullptr)
		{
			dataRequiredToRenderFrame->meshEffectPair[i].o_effect->Bind();
		}

		//Draw the mesh
		if (dataRequiredToRenderFrame->meshEffectPair[i].o_mesh != nullptr)
		{
			dataRequiredToRenderFrame->meshEffectPair[i].o_mesh->Draw();
		}
	}

	//o_effect->Bind();
	//o_mesh->Draw();
	//o_effect2->Bind();
	//o_mesh2->Draw();

	//newEffect->Bind();
	//newMesh->Draw();
	//newEffect2->Bind();
	//newMesh2->Draw();

	bufferSwapper->bufferSwap();

	//Make sure to clean up after data has been used by current frame

	for (size_t i = 0; i < dataRequiredToRenderFrame->meshEffectPairCount; i++)
	{
		//Clear the effect
		if (dataRequiredToRenderFrame->meshEffectPair[i].o_effect != nullptr)
		{
			dataRequiredToRenderFrame->meshEffectPair[i].o_effect->DecrementReferenceCount();
			dataRequiredToRenderFrame->meshEffectPair[i].o_effect = nullptr;
		}

		//Clear the mesh
		if (dataRequiredToRenderFrame->meshEffectPair[i].o_mesh != nullptr)
		{
			dataRequiredToRenderFrame->meshEffectPair[i].o_mesh->DecrementReferenceCount();
			dataRequiredToRenderFrame->meshEffectPair[i].o_mesh = nullptr;
		}
	}

}

void eae6320::Graphics::SubmitMeshEffect(cMesh* mesh1, cMesh* mesh2, cEffect* effect1, cEffect* effect2, size_t count)
{
	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread);

		s_dataBeingSubmittedByApplicationThread->meshEffectPair[0].o_mesh = mesh1;
		if (s_dataBeingSubmittedByApplicationThread->meshEffectPair[0].o_mesh != nullptr)
		{
			s_dataBeingSubmittedByApplicationThread->meshEffectPair[0].o_mesh->IncrementReferenceCount();
		}


		s_dataBeingSubmittedByApplicationThread->meshEffectPair[0].o_effect = effect1;
		if (s_dataBeingSubmittedByApplicationThread->meshEffectPair[0].o_effect != nullptr)
		{
			s_dataBeingSubmittedByApplicationThread->meshEffectPair[0].o_effect->IncrementReferenceCount();
		}

		s_dataBeingSubmittedByApplicationThread->meshEffectPair[1].o_mesh = mesh2;
		if (s_dataBeingSubmittedByApplicationThread->meshEffectPair[1].o_mesh != nullptr)
		{
			s_dataBeingSubmittedByApplicationThread->meshEffectPair[1].o_mesh->IncrementReferenceCount();
		}

		s_dataBeingSubmittedByApplicationThread->meshEffectPair[1].o_effect = effect2;
		if (s_dataBeingSubmittedByApplicationThread->meshEffectPair[1].o_effect != nullptr)
		{
			s_dataBeingSubmittedByApplicationThread->meshEffectPair[1].o_effect->IncrementReferenceCount();
		}
}

eae6320::cResult eae6320::Graphics::Initialize(const sInitializationParameters& i_initializationParameters)
{
	auto result = Results::Success;

	// Initialize the platform-specific context
	if (!(result = sContext::g_context.Initialize(i_initializationParameters)))
	{
		EAE6320_ASSERTF(false, "Can't initialize Graphics without context");
		return result;
	}
	// Initialize the platform-independent graphics objects
	{
		if (result = s_constantBuffer_frame.Initialize())
		{
			// There is only a single frame constant buffer that is reused
			// and so it can be bound at initialization time and never unbound
			s_constantBuffer_frame.Bind(
				// In our class both vertex and fragment shaders use per-frame constant data
				static_cast<uint_fast8_t>(eShaderType::Vertex) | static_cast<uint_fast8_t>(eShaderType::Fragment));
		}
		else
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without frame constant buffer");
			return result;
		}
	}
	// Initialize the events
	{
		if (!(result = s_whenAllDataHasBeenSubmittedFromApplicationThread.Initialize(Concurrency::EventType::ResetAutomaticallyAfterBeingSignaled)))
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without event for when data has been submitted from the application thread");
			return result;
		}
		if (!(result = s_whenDataForANewFrameCanBeSubmittedFromApplicationThread.Initialize(Concurrency::EventType::ResetAutomaticallyAfterBeingSignaled,
			Concurrency::EventState::Signaled)))
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without event for when data can be submitted from the application thread");
			return result;
		}
	}
	// Initialize the views
	{
		if (!(result = newRenderer->InitializeViews(i_initializationParameters)))
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without the views");
			return result;
		}
	}
	// Initialize the shading data
	{
		//eae6320::Graphics::cEffect::Load(o_effect, "data/Shaders/Fragment/standard.shader");

		//eae6320::Graphics::cEffect::Load(o_effect2, "data/Shaders/Fragment/special.shader");
	
		//if (!(result = newEffect->InitializeShadingData("data/Shaders/Fragment/standard.shader")))
		//{
		//	EAE6320_ASSERTF(false, "Can't initialize Graphics without the shading data");
		//	return result;
		//}

		//if (!(result = newEffect2->InitializeShadingData("data/Shaders/Fragment/special.shader")))
		//{
		//	EAE6320_ASSERTF(false, "Can't initialize Graphics without the shading data");
		//	return result;
		//}
	}
	// Initialize the geometry
	{	
		//eae6320::Graphics::cMesh::Load(o_mesh, index, vertexData);

		//eae6320::Graphics::cMesh::Load(o_mesh2, index2, vertexData2);
		//if (!(result = newMesh->InitializeGeometry(index, vertexData)))
		//{
		//	EAE6320_ASSERTF(false, "Can't initialize Graphics without the geometry data");
		//	return result;
		//}

		//if (!(result = newMesh2->InitializeGeometry(index2, vertexData2)))
		//{
		//	EAE6320_ASSERTF(false, "Can't initialize Graphics without the geometry data");
		//	return result;
		//}
	}
	return result;
}

eae6320::cResult eae6320::Graphics::CleanUp()
{
	auto result = Results::Success;

	newRenderer->CleanUp();

	for (size_t i = 0; i < s_dataBeingSubmittedByApplicationThread->meshEffectPairCount; i++)
	{
		//Clear the effect
		if (s_dataBeingSubmittedByApplicationThread->meshEffectPair[i].o_effect != nullptr)
		{
			s_dataBeingSubmittedByApplicationThread->meshEffectPair[i].o_effect->DecrementReferenceCount();
			s_dataBeingSubmittedByApplicationThread->meshEffectPair[i].o_effect = nullptr;
		}

		//Clear the mesh
		if (s_dataBeingSubmittedByApplicationThread->meshEffectPair[i].o_mesh != nullptr)
		{
			s_dataBeingSubmittedByApplicationThread->meshEffectPair[i].o_mesh->DecrementReferenceCount();
			s_dataBeingSubmittedByApplicationThread->meshEffectPair[i].o_mesh = nullptr;
		}
	}

	for (size_t i = 0; i < s_dataBeingRenderedByRenderThread->meshEffectPairCount; i++)
	{
		//Clear the effect
		if (s_dataBeingRenderedByRenderThread->meshEffectPair[i].o_effect != nullptr)
		{
			s_dataBeingRenderedByRenderThread->meshEffectPair[i].o_effect->DecrementReferenceCount();
			s_dataBeingRenderedByRenderThread->meshEffectPair[i].o_effect = nullptr;
		}

		//Clear the mesh
		if (s_dataBeingRenderedByRenderThread->meshEffectPair[i].o_mesh != nullptr)
		{
			s_dataBeingRenderedByRenderThread->meshEffectPair[i].o_mesh->DecrementReferenceCount();
			s_dataBeingRenderedByRenderThread->meshEffectPair[i].o_mesh = nullptr;
		}
	}
/*	o_mesh->CleanUp();
	o_mesh2->CleanUp();
	o_effect->CleanUp();
	o_effect2->CleanUp(); */// HI SHOE BUM. BELATED HAPPY BDAY. HAD KU

	{
		const auto result_constantBuffer_frame = s_constantBuffer_frame.CleanUp();
		if (!result_constantBuffer_frame)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = result_constantBuffer_frame;
			}
		}
	}

	{
		const auto result_context = sContext::g_context.CleanUp();
		if (!result_context)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = result_context;
			}
		}
	}

	//o_effect->DecrementReferenceCount();
	//o_effect = nullptr;
	//o_effect2->DecrementReferenceCount();
	//o_effect2 = nullptr;
	//o_mesh->DecrementReferenceCount();
	//o_mesh = nullptr;
	//o_mesh2->DecrementReferenceCount();
	//o_mesh2 = nullptr;

	delete newRenderer;
	delete bufferSwapper;

	return result;
}

void eae6320::Graphics::SetBGColor(float color[4])
{
	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread);

	s_dataBeingSubmittedByApplicationThread->bgColor[0] = color[0];
	s_dataBeingSubmittedByApplicationThread->bgColor[1] = color[1];
	s_dataBeingSubmittedByApplicationThread->bgColor[2] = color[2];
	s_dataBeingSubmittedByApplicationThread->bgColor[3] = color[3];
}