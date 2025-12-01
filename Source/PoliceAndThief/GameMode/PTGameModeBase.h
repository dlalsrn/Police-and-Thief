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
	TArray<TObjectPtr<APTPlayerController>> AllPlayerControllers;
};
