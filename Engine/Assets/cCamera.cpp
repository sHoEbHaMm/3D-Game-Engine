#include<Engine/Assets/cCamera.h>

eae6320::Assets::Camera::Camera()
{
	SetPosition(Math::sVector(0, 0, 10));
	cameraToProjected = Math::cMatrix_transformation::CreateCameraToProjectedTransform_perspective(0.8f, 1, 0.1f, 1000.0f);
}

void eae6320::Assets::Camera::Update(float i_secondCountToIntegrate)
{
	localTransform = camRigidBody.PredictFutureTransform(i_secondCountToIntegrate);
	camRigidBody.Update(i_secondCountToIntegrate);
}

void eae6320::Assets::Camera::SetPosition(Math::sVector position)
{
	camRigidBody.position = position;
}

void eae6320::Assets::Camera::SetVelocity(Math::sVector velocity)
{
	camRigidBody.velocity = velocity;
}

eae6320::Math::cMatrix_transformation eae6320::Assets::Camera::GetLocalTransform()
{
	return localTransform;
}

eae6320::Math::cMatrix_transformation eae6320::Assets::Camera::GetCameraToProjectedTransform()
{
	return cameraToProjected;
}

eae6320::Physics::sRigidBodyState* eae6320::Assets::Camera::GetRigidBodyReference()
{
	return &camRigidBody;
}