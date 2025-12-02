#include "PlayerState/PTPlayerState.h"
#include "AbilitySystemComponent.h"

APTPlayerState::APTPlayerState()
{
    AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
}

UAbilitySystemComponent* APTPlayerState::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}
