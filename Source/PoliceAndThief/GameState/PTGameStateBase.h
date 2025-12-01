#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "PTGameStateBase.generated.h"

UENUM(BlueprintType)
enum class EMatchState : uint8
{
	None,
	Wait,
	Play,
	End
};

UCLASS()
class POLICEANDTHIEF_API APTGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
};
