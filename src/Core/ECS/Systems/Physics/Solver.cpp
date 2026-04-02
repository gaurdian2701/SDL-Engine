#include "Core/ECS/Systems/Physics/Solver.h"
#include "Components/Rigidbody2D.h"
#include "Components/Transform.h"

void Core::ECS::Systems::Physics::Solver::Solve(const std::vector<CollisionManifold> &manifolds)
{
	for (auto& manifold : manifolds)
	{
		if (manifold.RigidbodyA && manifold.RigidbodyB)
		{
			if (manifold.RigidbodyA->IsStatic() && manifold.RigidbodyB->IsStatic())
			{
				continue;
			}

			if (manifold.RigidbodyA->IsStatic())
			{
				manifold.TransformB->Position += manifold.PenetrationDepth * manifold.ContactNormal;
			}
			else if (manifold.RigidbodyB->IsStatic())
			{
				manifold.TransformA->Position -= manifold.PenetrationDepth * manifold.ContactNormal;
			}
			else
			{
				manifold.TransformA->Position -= manifold.PenetrationDepth * 0.5f * manifold.ContactNormal;
				manifold.TransformB->Position += manifold.PenetrationDepth * 0.5f * manifold.ContactNormal;
			}

			glm::vec2 relativeVelocity = manifold.RigidbodyB->LinearVelocity - manifold.RigidbodyA->LinearVelocity;
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
			float inverseMassA = manifold.RigidbodyA->GetInverseMass();
			float inverseMassB = manifold.RigidbodyB->GetInverseMass();

			impulseScalar /= inverseMassA + inverseMassB;
			glm::vec2 impulse = impulseScalar * manifold.ContactNormal;

			manifold.RigidbodyA->LinearVelocity -= inverseMassA * impulse;
			manifold.RigidbodyB->LinearVelocity += inverseMassB * impulse;

			//Do Positional correction to correct rigidbodies sinking into one another
			//This solves edge case 2
			const float positionalCorrectionPercentage = 1.0f;
			const float slop = 0.01f;

			//Move both objects along the collision normal by a % of penetration depth,
			//and only move/separate them if the penetrationDepth is above a threshold which is the slop value
			glm::vec2 correction = std::max(manifold.PenetrationDepth - slop, 0.0f) /
				(inverseMassA + inverseMassB) * positionalCorrectionPercentage * manifold.ContactNormal;
			manifold.TransformA->Position -= manifold.RigidbodyA->GetInverseMass() * correction;
			manifold.TransformB->Position += manifold.RigidbodyB->GetInverseMass() * correction;
		}
	}
}


