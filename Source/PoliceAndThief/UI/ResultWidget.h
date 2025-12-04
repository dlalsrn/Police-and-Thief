#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ResultWidget.generated.h"

class UTextBlock;

UCLASS()
class POLICEANDTHIEF_API UResultWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetResultText(const FText& NewText);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ResultText;
};
