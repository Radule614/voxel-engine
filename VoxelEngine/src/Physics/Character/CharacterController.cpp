//
// Created by RadU on 7/22/2025.
//

#include "CharacterController.hpp"

using namespace JPH;

namespace VoxelEngine
{

CharacterController::CharacterController(std::unique_ptr<CharacterVirtual>&& character)
    : m_Character(std::move(character))
{
}

void CharacterController::HandleInput(Vec3Arg inMovementDirection, bool inJump, float inDeltaTime)
{
    PhysicsSystem& physicsSystem = PhysicsEngine::Instance().GetSystem();

    bool playerControlsHorizontalVelocity = m_Character->IsSupported();
    if (playerControlsHorizontalVelocity)
    {
        // Smooth the player input
        m_DesiredVelocity = m_InertiaEnabled
                                ? 0.25f * inMovementDirection * m_CharacterSpeed + 0.75f * m_DesiredVelocity
                                : inMovementDirection * m_CharacterSpeed;

        // True if the player intended to move
        m_AllowSliding = !inMovementDirection.IsNearZero();
    }
    else { m_AllowSliding = true; }

    Quat characterUpRotation = m_Character->GetRotation();

    // A cheaper way to update the character's ground velocity,
    // the platforms that the character is standing on may have changed velocity
    m_Character->UpdateGroundVelocity();

    // Determine new basic velocity
    Vec3 currentVerticalVelocity = m_Character->GetLinearVelocity().Dot(m_Character->GetUp()) * m_Character->GetUp();
    Vec3 groundVelocity = m_Character->GetGroundVelocity();
    Vec3 newVelocity;
    bool movingTowardsGround = (currentVerticalVelocity.GetY() - groundVelocity.GetY()) < 0.1f;
    if (m_Character->GetGroundState() == CharacterVirtual::EGroundState::OnGround // If on ground
        && (m_InertiaEnabled
                // Inertia enabled: And not moving away from ground
                ? movingTowardsGround
                // Inertia disabled: And not on a slope that is too steep
                : !m_Character->IsSlopeTooSteep(m_Character->GetGroundNormal())))
    {
        // Assume velocity of ground when on ground
        newVelocity = groundVelocity;

        // Jump
        if (inJump && movingTowardsGround)
            newVelocity += m_JumpSpeed * m_Character->GetUp();
    }
    else
        newVelocity = currentVerticalVelocity;

    // Gravity
    newVelocity += characterUpRotation * physicsSystem.GetGravity() * m_GravityStrength * inDeltaTime;

    if (playerControlsHorizontalVelocity)
    {
        // Player input
        newVelocity += characterUpRotation * m_DesiredVelocity;
    }
    else
    {
        // Preserve horizontal velocity
        Vec3 currentHorizontalVelocity = m_Character->GetLinearVelocity() - currentVerticalVelocity;
        newVelocity += currentHorizontalVelocity;
    }

    // Update character velocity
    m_Character->SetLinearVelocity(newVelocity);
}

CharacterVirtual& CharacterController::GetCharacter() const { return *m_Character; }

}
