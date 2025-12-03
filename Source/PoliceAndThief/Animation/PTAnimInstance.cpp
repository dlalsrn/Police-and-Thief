#include "Animation/PTAnimInstance.h"
#include "Character/PTPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UPTAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwnerCharacter = Cast<ACharacter>(GetOwningActor());
	if (IsValid(OwnerCharacter))
	{
		OwnerCharacterMovementComponent = OwnerCharacter->GetCharacterMovement();
	}
}

void UPTAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!IsValid(OwnerCharacter) || !IsValid(OwnerCharacterMovementComponent))
	{
		return;
	}

	Velocity = OwnerCharacterMovementComponent->Velocity;
	GroundSpeed = FVector(Velocity.X, Velocity.Y, 0.f).Size();
	bIsFalling = OwnerCharacterMovementComponent->IsFalling();

	bool bIsPlayerControlled = TryGetPawnOwner()->IsPlayerControlled(); // AI ÆÇº°
	bShouldMove = (bIsPlayerControlled ? !OwnerCharacterMovementComponent->GetCurrentAcceleration().IsNearlyZero() : true) && (0.01f < GroundSpeed);
}

void UPTAnimInstance::SetIsDeath(bool bInIsDeath)
{
	bIsDeath = bInIsDeath;
}
