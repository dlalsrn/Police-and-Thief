#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PTPlayerController.generated.h"

class UInputMappingContext;
class UNotificationWidget;

UCLASS()
class POLICEANDTHIEF_API APTPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	APTPlayerController();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Client, Reliable)
	void ClientRPCNotificationMessage(const FString& Message);

	void SetNotificationText(const FString& Message);
	
protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

private:
	UFUNCTION()
	void OnRep_NotificationText();

protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputMappingContext> DefaultMappingContexts;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UNotificationWidget> NotificationWidgetClass;

private:
	UPROPERTY(ReplicatedUsing = OnRep_NotificationText)
	FText NotifactionText;

	UPROPERTY()
	TObjectPtr<UNotificationWidget> NotificationWidgetInstance;

	FVector OriginalLocation;
};
