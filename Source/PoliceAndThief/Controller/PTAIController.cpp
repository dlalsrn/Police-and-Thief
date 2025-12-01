#include "Controller/PTAIController.h"
#include "Character/PTPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PoliceAndThief.h"

void APTAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (!HasAuthority())
	{
		return;
	}

	PT_LOG_NET(LogPTNet, Log, TEXT("AI Controller Possessed Pawn: %s"), *InPawn->GetName());

	GetWorld()->GetTimerManager().SetTimer(
		BTTimerHandle,
		this,
		&APTAIController::StartBehaviorTree,
		FMath::FRandRange(1.0f, 3.0f),
		false
	);
}

void APTAIController::BeginPlay()
{
	Super::BeginPlay();
}

void APTAIController::StartBehaviorTree()
{
	if (BehaviorTree)
	{
		PT_LOG_NET(LogPTNet, Log, TEXT("AI Controller Start Behavior Tree"));
		RunBehaviorTree(BehaviorTree);
	}
}
