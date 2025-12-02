#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PTPlayerController.generated.h"

class UInputMappingContext;

UCLASS()
class POLICEANDTHIEF_API APTPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	APTPlayerController();

	UFUNCTION(Client, Reliable)
	void ClientRPCNotificationMessage(const FString& Message);

protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputMappingContext> DefaultMappingContexts;
};
