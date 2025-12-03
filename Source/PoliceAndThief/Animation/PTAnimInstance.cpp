#include "Animation/PTAnimInstance.h"
#include "Character/PTPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UPTAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwnerCharacter = Cast<APTPlayerCharacter>(GetOwningActor());
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

	bool bIsLocallyControlled = OwnerCharacter->IsLocallyControlled(); // AI ÆÇº°
	bShouldMove = (bIsLocallyControlled ? !OwnerCharacterMovementComponent->GetCurrentAcceleration().IsNearlyZero() : true) && (3.f < GroundSpeed);
}

void UPTAnimInstance::SetIsDeath(bool bInIsDeath)
{
	bIsDeath = bInIsDeath;
}
