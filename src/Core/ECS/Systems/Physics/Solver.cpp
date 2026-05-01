#include "../../../../../include/Core/Physics/Solver.h"
#include <array>
#include "Components/Rigidbody2D.h"
#include "Components/Transform.h"
#include "Core/Physics/NaiveBroadPhase.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/exterior_product.hpp>

#include "Core/MathHelpers.h"

void Core::Physics::Solver::Solve(std::vector<PhysicsData::ContactManifold> &manifolds, const float physicsTimeStep)
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

				float restitutionConstant = std::min(manifold.RigidbodyA->Restitution, manifold.RigidbodyB->Restitution);
				float inverseMassA = manifold.RigidbodyA->GetInverseMass();
				float inverseMassB = manifold.RigidbodyB->GetInverseMass();
				float inverseInertiaA = manifold.RigidbodyA->GetInverseMomentOfInertia();
				float inverseInertiaB = manifold.RigidbodyB->GetInverseMomentOfInertia();

				const float positionalCorrectionPercentage = 0.4f;
				const float slop = 0.01f;
				static float normalImpulses[2] = {0.0f};

				//Solve Normal Impulses
				for (std::uint8_t index = 0; index < manifold.Contacts.NumberOfContactPoints; index++)
				{
					glm::vec2 aVecToContactPoint = manifold.Contacts.Points[index] - manifold.TransformA->Position;
					glm::vec2 bVecToContactPoint = manifold.Contacts.Points[index] - manifold.TransformB->Position;

					glm::vec2 aVecToContactPointPerp = glm::vec2(-aVecToContactPoint.y, aVecToContactPoint.x);
					glm::vec2 bVecToContactPointPerp = glm::vec2(-bVecToContactPoint.y, bVecToContactPoint.x);

					glm::vec2 angularVelocityA = aVecToContactPointPerp * manifold.RigidbodyA->AngularVelocity;
					glm::vec2 angularVelocityB = bVecToContactPointPerp * manifold.RigidbodyB->AngularVelocity;

					glm::vec2 relativeVelocity = (manifold.RigidbodyB->LinearVelocity + angularVelocityB) -
												(manifold.RigidbodyA->LinearVelocity + angularVelocityA);

					float contactVelocityMagnitude = glm::dot(relativeVelocity, manifold.ContactNormal);

					//If it is positive, the rigidbodies are already separated, which means that
					//they are not colliding. Example, when
					//1. Two rigidbodies are moving away or
					//2. They are moving in the same direction with separation.
					//NOTE: In case 2, two rigidbodies moving in the same direction will fail to collide
					//with impulse if one rigidbody somehow manages to catch up with the other. They will simply clip
					//into one another since relativeVelocityAlongNormalScalar would be 0.
					if (contactVelocityMagnitude > 0.0f)
					{
						continue;
					}

					//nullify restitution at small speeds
					if (std::abs(contactVelocityMagnitude) < 1.0f)
					{
						restitutionConstant = 0.0f;
					}

					float aVecToContactPerpDotNormal = glm::dot(aVecToContactPointPerp, manifold.ContactNormal);
					float bVecToContactPerpDotNormal = glm::dot(bVecToContactPointPerp, manifold.ContactNormal);

					float impulseScalar = -(1 + restitutionConstant) * contactVelocityMagnitude;

					impulseScalar /= inverseMassA + inverseMassB +
						(aVecToContactPerpDotNormal * aVecToContactPerpDotNormal) * inverseInertiaA +
							(bVecToContactPerpDotNormal * bVecToContactPerpDotNormal) * inverseInertiaB;
					impulseScalar /= manifold.Contacts.NumberOfContactPoints;

					normalImpulses[index] = impulseScalar;

					glm::vec2 finalImpulse = impulseScalar * manifold.ContactNormal;

					//Apply linear impulses
					manifold.RigidbodyA->LinearVelocity -= finalImpulse * inverseMassA;
					manifold.RigidbodyB->LinearVelocity += finalImpulse * inverseMassB;

					//Apply angular impulses
					manifold.RigidbodyA->AngularVelocity -= MathHelpers::VectorCross(aVecToContactPoint, finalImpulse) *
						manifold.RigidbodyA->GetInverseMomentOfInertia();
					manifold.RigidbodyB->AngularVelocity += MathHelpers::VectorCross(bVecToContactPoint, finalImpulse) *
						manifold.RigidbodyB->GetInverseMomentOfInertia();

					//Do Positional correction to correct rigidbodies sinking into one another
					//Accumulated additions of gravity into the ground would keep sinking it into the ground
					//Then add positional correction
					//This solves edge case 2
					//Move both objects along the collision normal by a % of penetration depth,
					//and only move/separate them if the penetrationDepth is above a threshold which is the slop value
					glm::vec2 correction = std::max(manifold.PenetrationDepth - slop, 0.0f) /
						(inverseMassA + inverseMassB) * positionalCorrectionPercentage * manifold.ContactNormal;

					manifold.TransformA->Position -= manifold.RigidbodyA->GetInverseMass() * correction;
					manifold.TransformB->Position += manifold.RigidbodyB->GetInverseMass() * correction;
				}

				float staticFriction = (manifold.RigidbodyA->StaticFriction + manifold.RigidbodyB->StaticFriction) * 0.5f;
				float dynamicFriction = (manifold.RigidbodyA->DynamicFriction + manifold.RigidbodyB->DynamicFriction) * 0.5f;

				//Solve Tangential/Frictional impulses
				for (std::uint8_t index = 0; index < manifold.Contacts.NumberOfContactPoints; index++)
				{
					glm::vec2 aVecToContactPoint = manifold.Contacts.Points[index] - manifold.TransformA->Position;
					glm::vec2 bVecToContactPoint = manifold.Contacts.Points[index] - manifold.TransformB->Position;

					glm::vec2 aVecToContactPointPerp = glm::vec2(-aVecToContactPoint.y, aVecToContactPoint.x);
					glm::vec2 bVecToContactPointPerp = glm::vec2(-bVecToContactPoint.y, bVecToContactPoint.x);

					glm::vec2 angularVelocityA = aVecToContactPointPerp * manifold.RigidbodyA->AngularVelocity;
					glm::vec2 angularVelocityB = bVecToContactPointPerp * manifold.RigidbodyB->AngularVelocity;

					glm::vec2 relativeVelocity = (manifold.RigidbodyB->LinearVelocity + angularVelocityB) -
												(manifold.RigidbodyA->LinearVelocity + angularVelocityA);

					//Gives the tangent velocity in the direction of the relative velocity
					glm::vec2 tangentVelocity =
						relativeVelocity - glm::dot(relativeVelocity, manifold.ContactNormal) * manifold.ContactNormal;

					if (glm::length(tangentVelocity) < 0.01f)
					{
						continue;
					}

					tangentVelocity = glm::normalize(tangentVelocity);

					float aVecToContactPerpDotTangent = glm::dot(aVecToContactPointPerp, tangentVelocity);
					float bVecToContactPerpDotTangent = glm::dot(bVecToContactPointPerp, tangentVelocity);

					//This is the frictional impulse to be applied in the opposite direction of the relative velocity
					float frictionalImpulseScalar = -glm::dot(relativeVelocity, tangentVelocity);

					frictionalImpulseScalar /= inverseMassA + inverseMassB +
						(aVecToContactPerpDotTangent * aVecToContactPerpDotTangent) * inverseInertiaA +
							(bVecToContactPerpDotTangent * bVecToContactPerpDotTangent) * inverseInertiaB;
					frictionalImpulseScalar /= manifold.Contacts.NumberOfContactPoints;

					glm::vec2 finalFrictionalImpulse = glm::vec2(0.0f);
					float normalImpulse = normalImpulses[index];

					//Adjust frictional forces according to Coulomb's Law:
					//If frictionalImpulse < normalImpulse * static friction, keep the original calculation,
					//else scale it by dynamic friction in the opposite direction

					if (std::abs(frictionalImpulseScalar) < normalImpulse * staticFriction)
					{
						finalFrictionalImpulse = frictionalImpulseScalar * tangentVelocity;
					}
					else
					{
						finalFrictionalImpulse = -normalImpulse * dynamicFriction * tangentVelocity;
					}

					//Apply linear impulses
					manifold.RigidbodyA->LinearVelocity -= finalFrictionalImpulse * inverseMassA;
					manifold.RigidbodyB->LinearVelocity += finalFrictionalImpulse * inverseMassB;

					//Apply angular impulses
					manifold.RigidbodyA->AngularVelocity -= MathHelpers::VectorCross(aVecToContactPoint, finalFrictionalImpulse) *
						manifold.RigidbodyA->GetInverseMomentOfInertia();
					manifold.RigidbodyB->AngularVelocity += MathHelpers::VectorCross(bVecToContactPoint, finalFrictionalImpulse) *
						manifold.RigidbodyB->GetInverseMomentOfInertia();
				}
			}
		}
	}
}


