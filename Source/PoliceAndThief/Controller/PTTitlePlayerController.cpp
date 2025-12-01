#include "Controller/PTTitlePlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void APTTitlePlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() == false)
	{
		return;
	}

	if (IsValid(TitleWidgetClass) == true)
	{
		TitleWidgetInstance = CreateWidget<UUserWidget>(this, TitleWidgetClass);
		if (IsValid(TitleWidgetInstance) == true)
		{
			TitleWidgetInstance->AddToViewport();

			FInputModeUIOnly Mode;
			SetInputMode(Mode);

			bShowMouseCursor = true;
		}
	}
}

void APTTitlePlayerController::JoinServer(const FString& ServerIP)
{
	FName NextLevelName = FName(*ServerIP);
	UGameplayStatics::OpenLevel(GetWorld(), NextLevelName, true);
}
