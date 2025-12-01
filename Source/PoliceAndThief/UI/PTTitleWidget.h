#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PTTitleWidget.generated.h"

class UButton;
class UEditableText;

UCLASS()
class POLICEANDTHIEF_API UPTTitleWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void OnPlayButtonClicked();

	UFUNCTION()
	void OnQuitButtonClicked();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UButton> PlayButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UButton> QuitButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UEditableText> ServerIPEditableText;
};
