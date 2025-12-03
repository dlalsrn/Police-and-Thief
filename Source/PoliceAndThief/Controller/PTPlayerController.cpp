#include "Controller/PTPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Net/UnrealNetwork.h"
#include "Blueprint/UserWidget.h"
#include "UI/NotificationWidget.h"

APTPlayerController::APTPlayerController()
{
}

void APTPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APTPlayerController, NotifactionText);
}

void APTPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController())
	{
		return;
	}

	if (IsValid(NotificationWidgetClass) || IsLocalController())
	{
		NotificationWidgetInstance = Cast<UNotificationWidget>(CreateWidget<UUserWidget>(this, NotificationWidgetClass));
		if (IsValid(NotificationWidgetInstance))
		{
			NotificationWidgetInstance->AddToViewport();
		}
	}

	OriginalLocation = GetPawn()->GetActorLocation();

	FInputModeGameOnly Mode;
	SetInputMode(Mode);

	bShowMouseCursor = false;
}

void APTPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	if (IsLocalPlayerController())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContexts, 0);
		}
	}
}

void APTPlayerController::ClientRPCNotificationMessage_Implementation(const FString& Message)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Message);
}

void APTPlayerController::SetNotificationText(const FString& Message)
{
	NotifactionText = FText::FromString(Message);
}

void APTPlayerController::OnRep_NotificationText()
{
	NotificationWidgetInstance->SetNotificationText(NotifactionText);
}