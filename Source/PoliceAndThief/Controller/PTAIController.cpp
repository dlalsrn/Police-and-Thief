#include "Controller/PTAIController.h"
#include "Character/PTPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BrainComponent.h"

void APTAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void APTAIController::BeginPlay()
{
	Super::BeginPlay();
}

void APTAIController::StartAIControl()
{
	GetWorld()->GetTimerManager().SetTimer(
		BTTimerHandle,
		this,
		&APTAIController::StartBehaviorTree,
		FMath::FRandRange(1.0f, 3.0f),
		false
	);
}

void APTAIController::StopAIControl()
{
	UBrainComponent* BrainComp = GetBrainComponent();
	if (IsValid(BrainComp))
	{
		BrainComp->StopLogic(TEXT("Stop AI Control"));
	}
}

void APTAIController::StartBehaviorTree()
{
	if (BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BehaviorTree is not assigned in PTAIController."));
	}
}
