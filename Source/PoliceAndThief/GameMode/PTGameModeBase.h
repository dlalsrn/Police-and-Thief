#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PTGameModeBase.generated.h"

class APTPlayerController;
class APTAIController;
class APTPlayerCharacter;

UCLASS()
class POLICEANDTHIEF_API APTGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	void OnCharacterDead(APTPlayerController* InController);

protected:
	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void Logout(AController* Exiting) override;

	virtual void BeginPlay() override;

private:
	void ElapsedTimerForMain();

	void NotifyAllPlayers(const FString& Message);

	void FindAllAIControllers();

	void PossessRandomCharacter(APlayerController* NewPlayer);

	void DestroyInitialCharacter();

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
	UPROPERTY()
	TArray<TObjectPtr<APTAIController>> AllAIControllers;

	UPROPERTY()
	TArray<TObjectPtr<APTPlayerCharacter>> AllAICharacters;

	UPROPERTY()
	TArray<TObjectPtr<APTPlayerController>> AlivePlayerControllers;

	UPROPERTY()
	TArray<TObjectPtr<APTPlayerController>> DeadPlayerControllers;
};
