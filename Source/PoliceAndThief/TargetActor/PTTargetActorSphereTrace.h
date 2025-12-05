#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "PTTargetActorSphereTrace.generated.h"

UCLASS()
class POLICEANDTHIEF_API APTTargetActorSphereTrace : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()
	
public:
	APTTargetActorSphereTrace();

	virtual void StartTargeting(UGameplayAbility* Ability) override;

	virtual void ConfirmTargetingAndContinue() override;

private:
	bool GetSphereTraceHitResult(AActor* InSourceActor, TArray<FHitResult>& OutHitResults) const;
};
