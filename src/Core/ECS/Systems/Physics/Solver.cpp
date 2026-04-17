#include "../../../../../include/Core/Physics/Solver.h"
#include "Components/Rigidbody2D.h"
#include "Components/Transform.h"
#include "Core/Physics/NaiveBroadPhase.h"

void Core::Physics::Solver::Solve(std::vector<PhysicsData::ContactManifold> &manifolds)
{
	for (std::uint8_t i = 0; i < m_solverIterations; i++)
	{
		for (auto& manifold : manifolds)
		{
			if (manifold.RigidbodyA && manifold.RigidbodyB)
			{
				if (manifold.RigidbodyA->IsStatic() && manifold.RigidbodyB->IsStatic())
				{
					continue;
				}

				float relativeVelocityAlongNormalScalar = glm::dot(manifold.RigidbodyB->LinearVelocity - manifold.RigidbodyA->LinearVelocity,
					manifold.ContactNormal);

				//If it is positive, the rigidbodies are already separated, which means that
				//they are not colliding. Example, when
				//1. Two rigidbodies are moving away or
				//2. They are moving in the same direction with separation.
				//NOTE: In case 2, two rigidbodies moving in the same direction will fail to collide
				//with impulse if one rigidbody somehow manages to catch up with the other. They will simply clip
				//into one another since relativeVelocityAlongNormalScalar would be 0.
				if (relativeVelocityAlongNormalScalar > 0.0f)
				{
					continue;
				}

				float restitutionConstant = std::min(manifold.RigidbodyA->Restitution, manifold.RigidbodyB->Restitution);

				float inverseMassA = manifold.RigidbodyA->GetInverseMass();
				float inverseMassB = manifold.RigidbodyB->GetInverseMass();

				float incrementedImpulse = -(1 + restitutionConstant) * relativeVelocityAlongNormalScalar;
				incrementedImpulse /= inverseMassA + inverseMassB;

				//Projected Gauss Seidel - Accumulate impulses over time instead of just instantly adding them in one frame
				float newAccumulatedImpulse = std::max(0.0f, manifold.AccumulatedImpulse + incrementedImpulse);
				float appliedImpulse = newAccumulatedImpulse - manifold.AccumulatedImpulse;
				manifold.AccumulatedImpulse = newAccumulatedImpulse;

				glm::vec2 impulse = appliedImpulse * manifold.ContactNormal;

				manifold.RigidbodyA->LinearVelocity -= inverseMassA * impulse;
				manifold.RigidbodyB->LinearVelocity += inverseMassB * impulse;

				//Do Positional correction to correct rigidbodies sinking into one another
				//Accumulated additions of gravity into the ground would keep sinking it into the ground
				//Then add positional correction
				//This solves edge case 2

				const float positionalCorrectionPercentage = 0.4f;
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
}


