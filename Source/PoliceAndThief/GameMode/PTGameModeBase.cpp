#include "GameMode/PTGameModeBase.h"
#include "Controller/PTPlayerController.h"
#include "GameState/PTGameStateBase.h"
#include "PoliceAndThief.h"
#include "EngineUtils.h"
#include "Controller/PTAIController.h"
#include "Character/PTPlayerCharacter.h"

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

	FindAllAIControllers();

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
			DestroyInitialCharacter();

			for (APTAIController* PTAIController : AllAIControllers)
			{
				if (IsValid(PTAIController))
				{
					PTAIController->StartAIControl();
				}
			}

			PTGameState->SetCurrentMatchState(EMatchState::Play);
		}

		NotifyAllPlayers(NotificationString);

		break;

	case EMatchState::Play:
		PTGameState->SetAlivePlayerCount(AlivePlayerControllers.Num());

		NotificationString = FString::Printf(TEXT("Find Another players. Alive Player Count: %d"), PTGameState->GetAlivePlayerCount());

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

void APTGameModeBase::FindAllAIControllers()
{
	AllAIControllers.Empty();
	AllAICharacters.Empty();

	for (TActorIterator<APTAIController> It(GetWorld()); It; ++It)
	{
		APTAIController* AIController = *It;
		if (IsValid(AIController))
		{
			AllAIControllers.Add(AIController);

			APTPlayerCharacter* PTCharacter = Cast<APTPlayerCharacter>(AIController->GetPawn());
			if (IsValid(PTCharacter))
			{
				AllAICharacters.Add(PTCharacter);
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("AIControllers: %d, AICharacters: %d"), AllAIControllers.Num(), AllAICharacters.Num());
}

void APTGameModeBase::PossessRandomCharacter(APlayerController* NewPlayer)
{
	if (AllAICharacters.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No AI Characters to possess."));
		return;
	}

	APTPlayerCharacter* SelectedCharacter = AllAICharacters[FMath::RandRange(0, AllAICharacters.Num() - 1)];
	if (!IsValid(SelectedCharacter))
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected AI Character is not valid."));
		return;
	}

	APTAIController* OldAIController = Cast<APTAIController>(SelectedCharacter->GetController());
	if (!IsValid(OldAIController))
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected AI Character has no valid AI Controller."));
		return;
	}

	OldAIController->UnPossess();

	if (AllAIControllers.Contains(OldAIController))
	{
		AllAIControllers.Remove(OldAIController);
	}

	if (AllAICharacters.Contains(SelectedCharacter))
	{
		AllAICharacters.Remove(SelectedCharacter);
	}

	NewPlayer->Possess(SelectedCharacter);
	SelectedCharacter->NetMulticastRPCRemoveBlockAttackTag();
}

void APTGameModeBase::DestroyInitialCharacter()
{
	for (APTPlayerController* PTPlayerController : AlivePlayerControllers)
	{
		if (!IsValid(PTPlayerController))
		{
			continue;
		}

		APawn* InitialPawn = PTPlayerController->GetPawn();
		if (IsValid(InitialPawn))
		{
			PTPlayerController->UnPossess();
			InitialPawn->Destroy();
		}

		PossessRandomCharacter(PTPlayerController);
	}
}
