#include "GameState/PTGameStateBase.h"
#include "Net/UnrealNetwork.h"

APTGameStateBase::APTGameStateBase() :
	CurrentMatchState(EMatchState::Wait)
{
}

void APTGameStateBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, AlivePlayerCount);
	DOREPLIFETIME(ThisClass, CurrentMatchState);
}

void APTGameStateBase::SetAlivePlayerCount(int32 NewAlivePlayerCount)
{
	AlivePlayerCount = NewAlivePlayerCount;
}

int32 APTGameStateBase::GetAlivePlayerCount() const
{
	return AlivePlayerCount;
}

EMatchState APTGameStateBase::GetCurrentMatchState() const
{
	return CurrentMatchState;
}

void APTGameStateBase::SetCurrentMatchState(EMatchState NewMatchState)
{
	CurrentMatchState = NewMatchState;
}
