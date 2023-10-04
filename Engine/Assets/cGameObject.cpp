#include<Engine/Assets/cGameObject.h>

eae6320::Assets::cGameObject::~cGameObject()
{
	CleanUp();
}

void eae6320::Assets::cGameObject::SetVelocity(Math::sVector velocity)
{
	gameObjectRigidbody.velocity = velocity;
}

eae6320::Physics::sRigidBodyState* eae6320::Assets::cGameObject::GetRigidBodyState()
{
	return &gameObjectRigidbody;
}

void eae6320::Assets::cGameObject::Update(float i_secondCountToIntegrate)
{
	transform = gameObjectRigidbody.PredictFutureTransform(i_secondCountToIntegrate);
	gameObjectRigidbody.Update(i_secondCountToIntegrate);
}

void eae6320::Assets::cGameObject::CleanUp()
{
	if (effect != nullptr)
	{
		effect->DecrementReferenceCount();
		effect = nullptr;
	}

	if (mesh != nullptr)
	{
		mesh->DecrementReferenceCount();
		mesh = nullptr;
	}
}

void eae6320::Assets::cGameObject::SetMesh(eae6320::Graphics::cMesh* newMesh)
{
	mesh->DecrementReferenceCount();
	mesh = newMesh;
	mesh->IncrementReferenceCount();
}

eae6320::cResult eae6320::Assets::cGameObject::InitializeMeshEffect(uint16_t _indexData[], eae6320::Graphics::VertexFormats::sVertex_mesh _vertexData[],
	std::string fragmentShaderPath)
{
	auto result = Results::Success;

	//load effect
	{
		if (!(result = eae6320::Graphics::cEffect::Load(effect, fragmentShaderPath)))
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without the shading data");
			return result;
		}
	}

	//load mesh
	{
		if (!(result = eae6320::Graphics::cMesh::Load(mesh, _indexData, _vertexData)))
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without the geometry data");
			return result;
		}
	}

	return result;
}