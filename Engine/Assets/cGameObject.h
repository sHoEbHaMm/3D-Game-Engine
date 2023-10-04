#pragma once

#include<Engine/Physics/sRigidBodyState.h>
#include<Engine/Math/cMatrix_transformation.h>
#include<Engine/Math/sVector.h>
#include<Engine/Graphics/cMesh.h>
#include<Engine/Graphics/cEffect.h>
namespace eae6320
{
	namespace Assets
	{
		class cGameObject
		{
			eae6320::Graphics::cMesh* mesh = nullptr;
			eae6320::Graphics::cEffect* effect = nullptr;
			eae6320::Physics::sRigidBodyState gameObjectRigidbody;
			eae6320::Math::cMatrix_transformation transform;
			eae6320::Math::cMatrix_transformation position;

		public:
			eae6320::cResult InitializeMeshEffect(uint16_t _indexData[], eae6320::Graphics::VertexFormats::sVertex_mesh _vertexData[],
				std::string fragmentShaderPath);

			void Update(float i_secondCountToIntegrate);

			eae6320::Graphics::cMesh* GetMesh() { return mesh; }
			eae6320::Graphics::cEffect* GetEffect() { return effect; }
			void SetMesh(eae6320::Graphics::cMesh* newMesh);

			eae6320::Math::cMatrix_transformation GetTransform() { return transform; }
			eae6320::Physics::sRigidBodyState* GetRigidBodyState();
			void SetVelocity(Math::sVector velocity);

			void CleanUp();
			~cGameObject();

		};
	}
}