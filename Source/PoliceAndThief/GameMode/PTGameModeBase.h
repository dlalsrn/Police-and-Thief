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

	void StartGame();

	void ResetGame();

	void DestroyAICharacters();

	void DestroyInitialPlayerCharacters();

	void SpawnAICharacters();

	void SpawnPlayerCharacter(APTPlayerController* NewPlayer);

	void CollectSpawnPoints();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 WaitingTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 EndingTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 SpawnAICharacterCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<ACharacter> AICharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<APTAIController> AIControllerClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<APTPlayerCharacter> PlayerCharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FVector> SpawnPoints;

	FTimerHandle MainTimerHandle;

	int32 RemainWaitingTimeForPlaying;

	int32 MinimumPlayerCountForPlaying;

	int32 RemainWaitingTimeForEnding;

private:
	UPROPERTY()
	TArray<TObjectPtr<APTAIController>> AllAIControllers;

	UPROPERTY()
	TArray<TObjectPtr<APTPlayerController>> AlivePlayerControllers;

	UPROPERTY()
	TArray<TObjectPtr<APTPlayerController>> DeadPlayerControllers;
};
