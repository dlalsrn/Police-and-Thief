#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NotificationWidget.generated.h"

class UTextBlock;

UCLASS()
class POLICEANDTHIEF_API UNotificationWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetNotificationText(const FText& NewText);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> NotificationText;
};
