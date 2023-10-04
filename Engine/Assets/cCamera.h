#pragma once
#include<Engine/Math/cMatrix_transformation.h>
#include<Engine/Physics/sRigidBodyState.h>
#include<Engine/Math/sVector.h>

namespace eae6320
{
	namespace Assets
	{
		class Camera
		{
			Math::cMatrix_transformation localTransform;
			Math::cMatrix_transformation cameraToProjected;
			Physics::sRigidBodyState camRigidBody;

		public:
			Camera();
			Math::cMatrix_transformation GetLocalTransform();
			Math::cMatrix_transformation GetCameraToProjectedTransform();
			void SetPosition(Math::sVector position);
			void SetVelocity(Math::sVector velocity);
			void Update(float i_secondToIntegrate);
			Physics::sRigidBodyState* GetRigidBodyReference();
		};
	}
}
