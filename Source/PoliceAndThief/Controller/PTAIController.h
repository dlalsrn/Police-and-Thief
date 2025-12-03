#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "PTAIController.generated.h"

class UBehaviorTree;

UCLASS()
class POLICEANDTHIEF_API APTAIController : public AAIController
{
	GENERATED_BODY()

public:
	void StartAIControl();

	void StopAIControl();

protected:
	virtual void OnPossess(APawn* InPawn) override;

	virtual void BeginPlay() override;

private:
	void StartBehaviorTree();

private:
	UPROPERTY(EditAnywhere, Category = "AI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBehaviorTree> BehaviorTree;

	FTimerHandle BTTimerHandle;
};
