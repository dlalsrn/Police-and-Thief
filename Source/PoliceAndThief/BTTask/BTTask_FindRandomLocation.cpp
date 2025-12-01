#include "BTTask/BTTask_FindRandomLocation.h"
#include "NavigationSystem.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_FindRandomLocation::UBTTask_FindRandomLocation()
{
	NodeName = TEXT("Find Random Location");
}

EBTNodeResult::Type UBTTask_FindRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!IsValid(AIController))
	{
		return EBTNodeResult::Failed;
	}

	APawn* AIPawn = AIController->GetPawn();
	if (!IsValid(AIPawn))
	{
		return EBTNodeResult::Failed;
	}

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSystem)
	{
		return EBTNodeResult::Failed;
	}

	FNavLocation RandomLocation;
	bool bFound = NavSystem->GetRandomPointInNavigableRadius(
		AIPawn->GetActorLocation(),
		SearchRadius,
		RandomLocation
	);

	if (bFound)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found Random Location: %s"), *RandomLocation.Location.ToString());
		UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
		if (IsValid(BlackboardComp))
		{
			BlackboardComp->SetValueAsVector(FName("TargetLocation"), RandomLocation.Location);
			return EBTNodeResult::Succeeded;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to find random location"));
	}

	return EBTNodeResult::Failed;
}
