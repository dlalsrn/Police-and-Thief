#include "UI/PTTitleWidget.h"
#include "Controller/PTTitlePlayerController.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Kismet/KismetSystemLibrary.h"

void UPTTitleWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(PlayButton))
	{
		PlayButton->OnClicked.AddDynamic(this, &UPTTitleWidget::OnPlayButtonClicked);
	}

	if (IsValid(QuitButton))
	{
		QuitButton->OnClicked.AddDynamic(this, &UPTTitleWidget::OnQuitButtonClicked);
	}
}

void UPTTitleWidget::OnPlayButtonClicked()
{
	APTTitlePlayerController* PlayerController = GetOwningPlayer<APTTitlePlayerController>();
	if (IsValid(PlayerController))
	{
		FText ServerIP = ServerIPEditableText->GetText();
		PlayerController->JoinServer(ServerIP.ToString());
	}
}

void UPTTitleWidget::OnQuitButtonClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}
