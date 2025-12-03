#include "GameMode/PTGameModeBase.h"
#include "Controller/PTPlayerController.h"
#include "GameState/PTGameStateBase.h"
#include "PoliceAndThief.h"

void APTGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (!IsValid(NewPlayer))
	{
		return;
	}

	APTPlayerController* NewPTPlayer = Cast<APTPlayerController>(NewPlayer);
	if (IsValid(NewPTPlayer) && !AlivePlayerControllers.Contains(NewPTPlayer))
	{
		AlivePlayerControllers.Add(NewPTPlayer);
	}
}

void APTGameModeBase::Logout(AController* Exiting)
{
	APTPlayerController* ExitingPTPlayer = Cast<APTPlayerController>(Exiting);
	if (IsValid(ExitingPTPlayer) && AlivePlayerControllers.Contains(ExitingPTPlayer))
	{
		AlivePlayerControllers.Remove(ExitingPTPlayer);
		DeadPlayerControllers.Add(ExitingPTPlayer);
	}

	Super::Logout(Exiting);
}

void APTGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(
		MainTimerHandle,
		this,
		&APTGameModeBase::ElapsedTimerForMain,
		1.0f,
		true
	);

	MinimumPlayerCountForPlaying = 2;
	RemainWaitingTimeForPlaying = WaitingTime;
	RemainWaitingTimeForEnding = EndingTime;
}

void APTGameModeBase::OnCharacterDead(APTPlayerController* InController)
{
	if (IsValid(InController) && AlivePlayerControllers.Contains(InController))
	{
		AlivePlayerControllers.Remove(InController);
		DeadPlayerControllers.Add(InController);
	}
}

void APTGameModeBase::ElapsedTimerForMain()
{
	APTGameStateBase* PTGameState = GetGameState<APTGameStateBase>();
	if (!IsValid(PTGameState))
	{
		return;
	}

	FString NotificationString = TEXT("");
	switch (PTGameState->GetCurrentMatchState())
	{
		case EMatchState::None:
			break;

		case EMatchState::Wait:
			if (AlivePlayerControllers.Num() < MinimumPlayerCountForPlaying)
			{
				NotificationString = FString::Printf(TEXT("Wait another players for playing."));
				RemainWaitingTimeForPlaying = WaitingTime;
			}
			else
			{
				NotificationString = FString::Printf(TEXT("Wait %d seconds for playing."), RemainWaitingTimeForPlaying);
				--RemainWaitingTimeForPlaying;
			}

			if (RemainWaitingTimeForPlaying <= 0)
			{
				NotificationString = FString::Printf(TEXT(""));
				PTGameState->SetCurrentMatchState(EMatchState::Play);
			}

			NotifyAllPlayers(NotificationString);
			
			break;

		case EMatchState::Play:
			PTGameState->SetAlivePlayerCount(AlivePlayerControllers.Num());

			NotificationString = FString::Printf(TEXT("%d / %d"), PTGameState->GetAlivePlayerCount(), AlivePlayerControllers.Num() + DeadPlayerControllers.Num());

			NotifyAllPlayers(NotificationString);

			if (PTGameState->GetAlivePlayerCount() <= 1)
			{
				PTGameState->SetCurrentMatchState(EMatchState::End);
			}

			break;

		case EMatchState::End:
			NotificationString = FString::Printf(TEXT("Waiting %d for returning to title."), RemainWaitingTimeForEnding);

			NotifyAllPlayers(NotificationString);

			--RemainWaitingTimeForEnding;

			if (RemainWaitingTimeForEnding <= 0)
			{
				MainTimerHandle.Invalidate();
				return;
			}

			break;

		default:
			break;
	}
}

void APTGameModeBase::NotifyAllPlayers(const FString& Message)
{
	for (auto AlivePlayerController : AlivePlayerControllers)
	{
		AlivePlayerController->SetNotificationText(Message);
	}

	for (auto DeadPlayerController : DeadPlayerControllers)
	{
		DeadPlayerController->SetNotificationText(Message);
	}
}