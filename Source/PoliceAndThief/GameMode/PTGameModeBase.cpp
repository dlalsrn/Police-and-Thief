#include "GameMode/PTGameModeBase.h"
#include "Controller/PTPlayerController.h"

void APTGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	if (!IsValid(NewPlayer))
	{
		return;
	}

	Super::PostLogin(NewPlayer);

	APTPlayerController* NewPTPlayer = Cast<APTPlayerController>(NewPlayer);
	if (IsValid(NewPTPlayer) && !AllPlayerControllers.Contains(NewPTPlayer))
	{
		AllPlayerControllers.Add(NewPTPlayer);
	}
}

void APTGameModeBase::Logout(AController* Exiting)
{
	APTPlayerController* ExitingPTPlayer = Cast<APTPlayerController>(Exiting);
	if (IsValid(ExitingPTPlayer) && AllPlayerControllers.Contains(ExitingPTPlayer))
	{
		AllPlayerControllers.Remove(ExitingPTPlayer);
	}

	Super::Logout(Exiting);
}

void APTGameModeBase::BeginPlay()
{
	Super::BeginPlay();
}

