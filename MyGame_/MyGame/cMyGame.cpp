// Includes
//=========

#include "cMyGame.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/UserInput/UserInput.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Graphics/Graphics.h>
#include <Engine/Graphics/Graphics.cpp>
#include <Engine/Assets/cCamera.h>
#include <Engine/Math/cMatrix_transformation.h>
// Inherited Implementation
//=========================

// Run
//----

void eae6320::cMyGame::SubmitDataToBeRendered(const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{
	camTransform = camera.GetLocalTransform();
	camToProjectedTransform = camera.GetCameraToProjectedTransform();
	go1Transform = gameObjects[0].GetTransform();
	go2Transform = gameObjects[1].GetTransform();
	eae6320::Graphics::SetBGColor(bg_color);

	if (GameState == 3)
	{
		gameObjects[0].SetMesh(newMesh);
		eae6320::Graphics::SubmitMeshEffect(gameObjects[0].GetMesh(), gameObjects[1].GetMesh(), gameObjects[0].GetEffect(), gameObjects[1].GetEffect(),
			camTransform, camToProjectedTransform, go1Transform, go2Transform, meshEffectCount);
	}
	else
	{
		eae6320::Graphics::SubmitMeshEffect(gameObjects[0].GetMesh(), gameObjects[1].GetMesh(), gameObjects[0].GetEffect(), gameObjects[1].GetEffect(),
			camTransform, camToProjectedTransform, go1Transform, go2Transform, meshEffectCount);
	}

	//if (GameState == 1)
	//{
	//	eae6320::Graphics::SubmitMeshEffect(o_mesh, nullptr, o_effect, nullptr, meshEffectCount);
	//}
	//else if(GameState == 2)
	//{
	//	eae6320::Graphics::SubmitMeshEffect(o_mesh, o_mesh2, o_effect, o_effect, meshEffectCount);
	//}
	//
	//if(GameState == 0)
	//{
	//	eae6320::Graphics::SubmitMeshEffect(o_mesh, o_mesh2, o_effect, o_effect2, meshEffectCount);
	//}

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

void eae6320::cMyGame::UpdateBasedOnTime(const float i_elapsedSecondCount_sinceLastUpdate)
{
	camera.Update(i_elapsedSecondCount_sinceLastUpdate);
	gameObjects[0].Update(i_elapsedSecondCount_sinceLastUpdate);
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

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Tab))
	{
		GameState = 3;
	}

	//playermovement
	if (UserInput::IsKeyPressed('A'))
	{
		//move left
		gameObjects[0].SetVelocity(Math::sVector(-10, 0, 0));
	}
	else if (UserInput::IsKeyPressed('D'))
	{
		//move right
		gameObjects[0].SetVelocity(Math::sVector(10, 0, 0));
	}
	else if (UserInput::IsKeyPressed('S'))
	{
		//move down
		gameObjects[0].SetVelocity(Math::sVector(0, -10, 0));
	}
	else if (UserInput::IsKeyPressed('W'))
	{
		//move up
		gameObjects[0].SetVelocity(Math::sVector(0, 10, 0));
	}
	else
		gameObjects[0].SetVelocity(Math::sVector(0, 0, 0));

	//camera movement
	if (UserInput::IsKeyPressed('J'))
	{
		//move left
		camera.SetVelocity(Math::sVector(-10, 0, 0));
	}
	else if (UserInput::IsKeyPressed('L'))
	{
		//move right
		camera.SetVelocity(Math::sVector(10, 0, 0));
	}
	else if (UserInput::IsKeyPressed('K'))
	{
		//move up
		camera.SetVelocity(Math::sVector(0, 0, 10));
	}
	else if (UserInput::IsKeyPressed('I'))
	{
		//move down
		camera.SetVelocity(Math::sVector(0, 0, -10));
	}
	else
		camera.SetVelocity(Math::sVector(0, 0, 0));
}

// Initialize / Clean Up
//----------------------

eae6320::cResult eae6320::cMyGame::Initialize()
{
	eae6320::Logging::OutputMessage("Real Engine Tournament has been Initialized");
	auto result = eae6320::Results::Success;
	eae6320::Graphics::cMesh::Load(newMesh, newindex, vertexData);
	{
		if (!(result = gameObjects[0].InitializeMeshEffect(index, vertexData, "data/Shaders/Fragment/standard.shader")))
		{
			EAE6320_ASSERTF(false, "Failed Initializing GameObject");
			return result;
		}
	}

	{
		if (!(result = gameObjects[1].InitializeMeshEffect(index2, vertexData2, "data/Shaders/Fragment/special.shader")))
		{
			EAE6320_ASSERTF(false, "Failed Initializing GameObject");
			return result;
		}
	}
	//eae6320::Graphics::cEffect::Load(o_effect, "data/Shaders/Fragment/standard.shader");
	//eae6320::Graphics::cEffect::Load(o_effect2, "data/Shaders/Fragment/special.shader");

	//eae6320::Graphics::cMesh::Load(o_mesh, index, vertexData);
	//eae6320::Graphics::cMesh::Load(o_mesh2, index2, vertexData2);
	gameObjects[0].GetRigidBodyState()->position = eae6320::Math::sVector(0.0f, 0.0f, 0.0f);
	return result;
}

eae6320::cResult eae6320::cMyGame::CleanUp()
{
	eae6320::Logging::OutputMessage("Real Engine Tournament cleanup success");
	auto result = eae6320::Results::Success;

	for (int i = 0; i < 2; i++)
	{
		gameObjects[i].CleanUp();
	}

	return result;
}
