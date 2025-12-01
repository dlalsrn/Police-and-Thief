#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PTTitlePlayerController.generated.h"

class UUserWidget;

UCLASS()
class POLICEANDTHIEF_API APTTitlePlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	void JoinServer(const FString& ServerIP);

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess))
	TSubclassOf<UUserWidget> TitleWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess))
	TObjectPtr<UUserWidget> TitleWidgetInstance;
};
