#include "GameMode/PTGameModeBase.h"
#include "Controller/PTPlayerController.h"
#include "GameState/PTGameStateBase.h"
#include "PoliceAndThief.h"
#include "EngineUtils.h"
#include "Controller/PTAIController.h"
#include "Character/PTPlayerCharacter.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "Kismet/KismetMathLibrary.h"

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

	CollectSpawnPoints();

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
			StartGame();

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

			for (APTAIController* PTAIController : AllAIControllers)
			{
				if (IsValid(PTAIController))
				{
					PTAIController->StopAIControl();
				}
			}
		}

		break;

	case EMatchState::End:
		NotificationString = FString::Printf(TEXT("Waiting %d for returning to title."), RemainWaitingTimeForEnding);

		NotifyAllPlayers(NotificationString);

		--RemainWaitingTimeForEnding;

		if (RemainWaitingTimeForEnding <= 0)
		{
			for (APTPlayerController* AliveController : AlivePlayerControllers)
			{
				AliveController->ClientRPCReturnToTitle();
			}

			for (APTPlayerController* DeadController : DeadPlayerControllers)
			{
				DeadController->ClientRPCReturnToTitle();
			}

			MainTimerHandle.Invalidate();

			ResetGame();

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

void APTGameModeBase::StartGame()
{
	SpawnAICharacters();

	DestroyInitialPlayerCharacters();
	for (APTPlayerController* AliveController : AlivePlayerControllers)
	{
		SpawnPlayerCharacter(AliveController);
	}
}

void APTGameModeBase::ResetGame()
{
	APTGameStateBase* PTGameState = GetGameState<APTGameStateBase>();
	if (!IsValid(PTGameState))
	{
		return;
	}

	PTGameState->SetCurrentMatchState(EMatchState::Wait);

	AlivePlayerControllers.Empty();
	DeadPlayerControllers.Empty();

	RemainWaitingTimeForPlaying = WaitingTime;
	RemainWaitingTimeForEnding = EndingTime;

	DestroyAICharacters();
}

void APTGameModeBase::DestroyAICharacters()
{
	for (APTAIController* PTAIController : AllAIControllers)
	{
		if (!IsValid(PTAIController))
		{
			UE_LOG(LogTemp, Warning, TEXT("Invalid AIController in DestroyInitialCharacter."));
			continue;
		}

		ACharacter* AICharacter = Cast<ACharacter>(PTAIController->GetPawn());
		if (IsValid(AICharacter))
		{
			PTAIController->UnPossess();
			AICharacter->Destroy();
		}

		PTAIController->Destroy();
	}

	AllAIControllers.Empty();
}

void APTGameModeBase::DestroyInitialPlayerCharacters()
{
	for (APTPlayerController* PTPlayerController : AlivePlayerControllers)
	{
		if (!IsValid(PTPlayerController))
		{
			UE_LOG(LogTemp, Warning, TEXT("Invalid PlayerController in DestroyInitialCharacter."));
			continue;
		}

		APawn* InitialPawn = PTPlayerController->GetPawn();
		if (IsValid(InitialPawn))
		{
			PTPlayerController->UnPossess();
			InitialPawn->Destroy();
		}
	}
}

void APTGameModeBase::SpawnAICharacters()
{
	AllAIControllers.Empty();

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!IsValid(NavSys))
	{
		UE_LOG(LogTemp, Warning, TEXT("Navigation System is not valid."));
		return;
	}

	if (SpawnPoints.Num() == 0)
	{
		return;
	}

	const float SpawnRadius = 5000.f;

	for (int32 i = 0; i < SpawnAICharacterCount; ++i)
	{

		FVector Base = SpawnPoints[FMath::RandRange(0, SpawnPoints.Num() - 1)];
		FVector Offset = UKismetMathLibrary::RandomUnitVector() * 200.f;
		Offset.Z = 0.f;
		FVector SpawnLoc = Base + Offset;

		FNavLocation Projected;
		if (NavSys->ProjectPointToNavigation(SpawnLoc, Projected))
		{
			SpawnLoc = Projected.Location;
		}

		FRotator SpawnRot = FRotator::ZeroRotator;
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		ACharacter* NewAICharacter = GetWorld()->SpawnActor<ACharacter>(AICharacterClass, SpawnLoc, SpawnRot, Params);
		if (!IsValid(NewAICharacter))
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to spawn AI Character."));
			continue;
		}

		APTAIController* NewAIController = GetWorld()->SpawnActor<APTAIController>(AIControllerClass);
		if (!IsValid(NewAIController))
		{
			UE_LOG(LogTemp, Warning, TEXT("Spawned AI Character has no valid AI Controller."));
			continue;
		}

		NewAIController->Possess(NewAICharacter);

		AllAIControllers.Add(NewAIController);
	}
}

void APTGameModeBase::SpawnPlayerCharacter(APTPlayerController* NewPlayer)
{
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!IsValid(NavSys))
	{
		UE_LOG(LogTemp, Warning, TEXT("Navigation System is not valid."));
		return;
	}

	if (SpawnPoints.Num() == 0)
	{
		return;
	}

	FVector Base = SpawnPoints[FMath::RandRange(0, SpawnPoints.Num() - 1)];
	FVector Offset = UKismetMathLibrary::RandomUnitVector() * 200.f;
	Offset.Z = 0.f;
	FVector SpawnLoc = Base + Offset;

	FNavLocation Projected;
	if (NavSys->ProjectPointToNavigation(SpawnLoc, Projected))
	{
		SpawnLoc = Projected.Location;
	}

	FRotator SpawnRot = FRotator::ZeroRotator;
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	APTPlayerCharacter* NewPlayerCharacter = GetWorld()->SpawnActor<APTPlayerCharacter>(PlayerCharacterClass, SpawnLoc, SpawnRot, Params);
	if (!IsValid(NewPlayerCharacter))
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to spawn AI Character."));
		return;
	}

	NewPlayer->Possess(NewPlayerCharacter);
	NewPlayerCharacter->NetMulticastRPCRemoveBlockAttackTag();
}

void APTGameModeBase::CollectSpawnPoints()
{
	SpawnPoints.Empty();

	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		if (Actor->ActorHasTag("SpawnPoint"))
		{
			SpawnPoints.Add(Actor->GetActorLocation());
		}
	}
}
