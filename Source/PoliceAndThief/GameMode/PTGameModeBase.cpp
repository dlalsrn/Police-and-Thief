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
			if (AllPlayerControllers.Num() < MinimumPlayerCountForPlaying)
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
			PTGameState->SetAlivePlayerCount(AllPlayerControllers.Num());

			NotificationString = FString::Printf(TEXT("%d / %d"), PTGameState->GetAlivePlayerCount(), AllPlayerControllers.Num());

			NotifyAllPlayers(NotificationString);

			if (PTGameState->GetAlivePlayerCount() <= 1)
			{
				PTGameState->SetCurrentMatchState(EMatchState::End);
			}

			break;
		case EMatchState::End:
			break;
		default:
			break;
	}
}

void APTGameModeBase::NotifyAllPlayers(const FString& Message)
{
	for (auto AlivePlayerController : AllPlayerControllers)
	{
		AlivePlayerController->ClientRPCNotificationMessage(Message);
	}
}