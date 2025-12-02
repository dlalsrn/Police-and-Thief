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

public:
	APTGameStateBase();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	int32 GetAlivePlayerCount() const;

	void SetAlivePlayerCount(int32 NewAlivePlayerCount);

	EMatchState GetCurrentMatchState() const;

	void SetCurrentMatchState(EMatchState NewMatchState);

private:
	UPROPERTY(Replicated)
	int32 AlivePlayerCount;

	UPROPERTY(Replicated)
	EMatchState CurrentMatchState;
};
