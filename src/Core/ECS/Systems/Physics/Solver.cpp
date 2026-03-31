#include "Core/ECS/Systems/Physics/Solver.h"

#include "Components/Rigidbody2D.h"
#include "Components/Transform.h"

void Core::ECS::Systems::Physics::Solver::Solve(const std::vector<CollisionManifold> &manifolds)
{
	for (auto& manifold : manifolds)
	{
		if (manifold.RigidbodyA && manifold.RigidbodyB)
		{
			glm::vec2 relativeVelocity = manifold.RigidbodyB->Velocity - manifold.RigidbodyA->Velocity;

			float relativeVelocityAlongNormalScalar = glm::dot(relativeVelocity, manifold.ContactNormal);

			//If it is positive, the rigidbodies are already separated, which means that
			//they are not colliding. Example, when
			//1. Two rigidbodies are moving away or
			//2. They are moving in the same direction with separation.
			//NOTE: In case 2, two rigidbodies moving in the same direction will fail to collide
			//with impulse if one rigidbody somehow manages to catch up with the other. They will simply clip
			//into one another since relativeVelocityAlongNormalScalar would be 0.
			if (relativeVelocityAlongNormalScalar > 0.0f)
			{
				return;
			}

			float restitutionConstant = std::min(manifold.RigidbodyA->Restitution, manifold.RigidbodyB->Restitution);
			float impulseScalar = -(1 + restitutionConstant) * relativeVelocityAlongNormalScalar;

			impulseScalar /= manifold.RigidbodyA->GetInverseMass() + manifold.RigidbodyB->GetInverseMass();
			glm::vec2 impulse = impulseScalar * manifold.ContactNormal;

			float inverseMassSum = 1 / (manifold.RigidbodyA->GetMass() + manifold.RigidbodyB->GetMass());
			float ratio = 1 - manifold.RigidbodyA->GetMass() * inverseMassSum;
			manifold.RigidbodyA->Velocity -= ratio * impulse;

			ratio = 1 - manifold.RigidbodyB->GetMass() * inverseMassSum;
			manifold.RigidbodyB->Velocity += ratio * impulse;

			//Do Positional correction to correct rigidbodies sinking into one another
			//This solves edge case 2
			const float positionalCorrectionPercentage = 0.8f;
			const float slop = 0.01f;

			//Move both objects along the collision normal by a % of penetration depth,
			//and only move/separate them if the penetrationDepth is above a threshold which is the slop value
			glm::vec2 correction = std::max(manifold.PenetrationDepth - slop, 0.0f) * inverseMassSum * positionalCorrectionPercentage * manifold.ContactNormal;
			manifold.TransformA->Position -= manifold.RigidbodyA->GetInverseMass() * correction;
			manifold.TransformB->Position += manifold.RigidbodyB->GetInverseMass() * correction;
		}
		else
		{
			if (!manifold.TransformA->Static)
			{
				manifold.TransformA->Position -= manifold.PenetrationDepth * 0.5f * manifold.ContactNormal;
			}
			if (!manifold.TransformB->Static)
			{
				manifold.TransformB->Position += manifold.PenetrationDepth * 0.5f * manifold.ContactNormal;
			}
		}
	}
}

