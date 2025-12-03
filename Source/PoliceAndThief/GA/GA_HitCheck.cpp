#include "GA/GA_HitCheck.h"
#include "Character/PTPlayerCharacter.h"

UGA_HitCheck::UGA_HitCheck()
{
}

void UGA_HitCheck::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	if (!HasAuthority(&ActivationInfo))
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_HitCheck::ActivateAbility - No Authority"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_HitCheck::ActivateAbility - CommitAbility Failed"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	TArray<FHitResult> OutHitResults;
	TSet<APTPlayerCharacter*> HitCharacters;
	FCollisionQueryParams Params(NAME_None, false, ActorInfo->AvatarActor.Get());

	const float AttackRange = 100.f;
	const float AttackRadius = 50.f;
	FVector ForwardVec = ActorInfo->AvatarActor->GetActorForwardVector();
	FVector StartLocation = ActorInfo->AvatarActor->GetActorLocation();
	FVector EndLocation = StartLocation + ForwardVec * AttackRange;
	bool bHit = ActorInfo->AvatarActor->GetWorld()->SweepMultiByChannel(
		OutHitResults,
		StartLocation,
		EndLocation,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(AttackRadius),
		Params
	);

	for (const FHitResult& HitResult : OutHitResults)
	{
		APTPlayerCharacter* HitCharacter = Cast<APTPlayerCharacter>(HitResult.GetActor());
		if (IsValid(HitCharacter) && !HitCharacters.Contains(HitCharacter))
		{
			HitCharacters.Add(HitCharacter);
		}
	}

	for (APTPlayerCharacter* HitCharacter : HitCharacters)
	{
		HitCharacter->ServerRPCOnHit();
	}

	FColor DrawColor = bHit ? FColor::Green : FColor::Red;
	APTPlayerCharacter* PlayerCharacter = Cast<APTPlayerCharacter>(ActorInfo->AvatarActor.Get());
	if (IsValid(PlayerCharacter))
	{
		PlayerCharacter->DrawDebugAttackCollision(DrawColor, StartLocation, EndLocation, ForwardVec);
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UGA_HitCheck::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
