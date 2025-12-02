#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PTGameModeBase.generated.h"

class APTPlayerController;

UCLASS()
class POLICEANDTHIEF_API APTGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
protected:
	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void Logout(AController* Exiting) override;

	virtual void BeginPlay() override;

private:
	void ElapsedTimerForMain();

	void NotifyAllPlayers(const FString& Message);

protected:
	FTimerHandle MainTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 WaitingTime;

	int32 RemainWaitingTimeForPlaying;

	int32 MinimumPlayerCountForPlaying;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 EndingTime;

	int32 RemainWaitingTimeForEnding;

private:
	TArray<TObjectPtr<APTPlayerController>> AllPlayerControllers;
};
