// Includes
//=========

#include "cMyGame.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/UserInput/UserInput.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Graphics/Graphics.h>
#include <Engine/Graphics/Graphics.cpp>
// Inherited Implementation
//=========================

// Run
//----

void eae6320::cMyGame::SubmitDataToBeRendered(const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{
	eae6320::Graphics::SetBGColor(bg_color);
	if (GameState == 1)
	{
		eae6320::Graphics::SubmitMeshEffect(o_mesh, nullptr, o_effect, nullptr, meshEffectCount);
	}
	else if(GameState == 2)
	{
		eae6320::Graphics::SubmitMeshEffect(o_mesh, o_mesh2, o_effect, o_effect, meshEffectCount);
	}
	
	if(GameState == 0)
	{
		eae6320::Graphics::SubmitMeshEffect(o_mesh, o_mesh2, o_effect, o_effect2, meshEffectCount);
	}

}

void eae6320::cMyGame::UpdateBasedOnInput()
{
	// Is the user pressing the ESC key?
	if ( UserInput::IsKeyPressed( UserInput::KeyCodes::Escape ) )
	{
		// Exit the application
		const auto result = Exit( EXIT_SUCCESS );
		EAE6320_ASSERT( result );
	}
}

void eae6320::cMyGame::UpdateSimulationBasedOnInput()
{
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Control))
	{
		GameState = 1;
	}
	else if((UserInput::IsKeyPressed(UserInput::KeyCodes::Space)))
	{
		GameState = 2;
	}
	else
	{
		GameState = 0;
	}
}

// Initialize / Clean Up
//----------------------

eae6320::cResult eae6320::cMyGame::Initialize()
{
	eae6320::Logging::OutputMessage("Real Engine Tournament has been Initialized");
	auto result = eae6320::Results::Success;

	eae6320::Graphics::cEffect::Load(o_effect, "data/Shaders/Fragment/standard.shader");
	eae6320::Graphics::cEffect::Load(o_effect2, "data/Shaders/Fragment/special.shader");

	eae6320::Graphics::cMesh::Load(o_mesh, index, vertexData);
	eae6320::Graphics::cMesh::Load(o_mesh2, index2, vertexData2);

	return result;
}

eae6320::cResult eae6320::cMyGame::CleanUp()
{
	eae6320::Logging::OutputMessage("Real Engine Tournament cleanup success");
	auto result = eae6320::Results::Success;

	if (o_mesh != nullptr)
	{
		o_mesh->DecrementReferenceCount();
		o_mesh = nullptr;
	}
	if (o_effect != nullptr)
	{
		o_effect->DecrementReferenceCount();
		o_effect = nullptr;
	}
	if (o_mesh2 != nullptr)
	{
		o_mesh2->DecrementReferenceCount();
		o_mesh2 = nullptr;
	}
	if (o_effect2 != nullptr)
	{
		o_effect2->DecrementReferenceCount();
		o_effect2 = nullptr;
	}

	return result;
}
