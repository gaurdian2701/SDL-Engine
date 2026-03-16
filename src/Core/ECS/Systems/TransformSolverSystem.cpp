#include "Core/ECS/Systems/TransformSolverSystem.h"
#include "Assets/Components/Transform.h"
#include "Core/ECS/ECSManager.h"

void Core::ECS::Systems::TransformSolverSystem::RegisterInterestedComponents()
{
	Core::ECS::ECSManager::GetInstance().RegisterInterestedComponentsForSystem<Assets::Components::Transform>(this);
}

void Core::ECS::Systems::TransformSolverSystem::UpdateSystem(const float deltaTime)
{
	System::UpdateSystem(deltaTime);

    ECSManager::GetInstance().ForEachUsingComponents<Assets::Components::Transform>(
        [&, this](Assets::Components::Transform* transform)
        {
            this->SolveTransform(transform);
        });
}

inline void Core::ECS::Systems::TransformSolverSystem::SolveTransform(Assets::Components::Transform* someTransform)
{
	if (someTransform->ParentEntity != INVALID_ENTITY_ID)
	{
		auto parentTransform = ECSManager::GetInstance().GetComponent<Assets::Components::Transform>(someTransform->ParentEntity);
		//Solve Scaling
		someTransform->WorldScale = parentTransform->WorldScale * someTransform->LocalScale;

		//Solve Rotation
		someTransform->WorldRotation = parentTransform->WorldRotation + someTransform->LocalRotation;

		//Solve Position:
		//1. Scale local position by parent scale
		glm::vec2 scaledPositionVector = someTransform->LocalPosition * parentTransform->WorldScale;

		//2. Rotate position vector by parent rotation:
		//(xrot, yrot) = (xcost - ysint, xsint + ycost) t is in radians

		float cosAngle = std::cos(parentTransform->WorldRotation);
		float sinAngle = std::sin(parentTransform->WorldRotation);

		glm::vec2 rotatedPositionVector = glm::vec2(
			scaledPositionVector.x * cosAngle - scaledPositionVector.y * sinAngle,
			scaledPositionVector.x * sinAngle + scaledPositionVector.y * cosAngle);

		//3. Translate position using parent position
		someTransform->WorldPosition = rotatedPositionVector + parentTransform->WorldPosition;
	}
	else
	{
		someTransform->WorldPosition = someTransform->LocalPosition;
		someTransform->WorldRotation = someTransform->LocalRotation;
		someTransform->WorldScale = someTransform->LocalScale;
	}

	someTransform->Up = glm::vec2(-std::sin(someTransform->WorldRotation), std::cos(someTransform->WorldRotation));
	someTransform->Right = glm::vec2(std::cos(someTransform->WorldRotation), std::sin(someTransform->WorldRotation));
}

