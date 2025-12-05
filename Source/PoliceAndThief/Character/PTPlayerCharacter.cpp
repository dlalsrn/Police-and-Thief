#include "Character/PTPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "AbilitySystemComponent.h"
#include "PlayerState/PTPlayerState.h"
#include "PoliceAndThief.h"
#include "Animation/PTAnimInstance.h"
#include "GameMode/PTGameModeBase.h"
#include "Controller/PTPlayerController.h"

APTPlayerCharacter::APTPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	ASC->SetIsReplicated(true);
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
}

void APTPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	//ASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Character.Block.Attack")));
}

void APTPlayerCharacter::NetMulticastRPCRemoveBlockAttackTag_Implementation()
{
	ASC->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Character.Block.Attack")));
}

void APTPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (IsValid(EnhancedInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APTPlayerCharacter::Move);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APTPlayerCharacter::Look);

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &APTPlayerCharacter::StartJump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &APTPlayerCharacter::StopJump);

		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &APTPlayerCharacter::Attack);
	}
}

void APTPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitializeASC();

	if (HasAuthority())
	{
		PT_LOG_NET(LogPTNet, Log, TEXT("Initializing Abilities for Player Character"));
		InitializeAbilities();
	}
}

void APTPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitializeASC();
}

void APTPlayerCharacter::Move(const FInputActionValue& Value)
{
	if (!IsLocallyControlled())
	{
		return;
	}

	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.X);
		AddMovementInput(RightDirection, MovementVector.Y);
	}
}

void APTPlayerCharacter::Look(const FInputActionValue& Value)
{
	if (!IsLocallyControlled())
	{
		return;
	}

	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APTPlayerCharacter::StartJump(const FInputActionValue& Value)
{
	if (!IsLocallyControlled())
	{
		return;
	}

	FGameplayTag JumpTag = FGameplayTag::RequestGameplayTag(FName("Character.Ability.Jump"));
	ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(JumpTag));
}

void APTPlayerCharacter::StopJump(const FInputActionValue& Value)
{
	if (!IsLocallyControlled())
	{
		return;
	}

	StopJumping();
}

void APTPlayerCharacter::Attack(const FInputActionValue& Value)
{
	if (!IsLocallyControlled())
	{
		return;
	}

	FGameplayTag AttackTag = FGameplayTag::RequestGameplayTag(FName("Character.Ability.Attack"));
	ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(AttackTag));
}

void APTPlayerCharacter::InitializeASC()
{
	ASC->InitAbilityActorInfo(this, this);
}

void APTPlayerCharacter::InitializeAbilities()
{
	for (const TSubclassOf<UGameplayAbility>& AbilityClass : AllAbilities)
	{
		if (IsValid(AbilityClass))
		{
			FGameplayAbilitySpec Spec(AbilityClass, 1, 0, this);
			ASC->GiveAbility(Spec);
		}
	}
}

UAbilitySystemComponent* APTPlayerCharacter::GetAbilitySystemComponent() const
{
	return ASC;
}

void APTPlayerCharacter::ServerRPCOnHit_Implementation()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();

	SetLifeSpan(5.0f);

	APTGameModeBase* PTGameMode = GetWorld()->GetAuthGameMode<APTGameModeBase>();
	if (IsValid(PTGameMode))
	{
		APTPlayerController* PC = Cast<APTPlayerController>(GetController());
		if (IsValid(PC))
		{
			PTGameMode->OnCharacterDead(PC);
		}
	}

	NetMulticastRPCOnDie();
}

void APTPlayerCharacter::NetMulticastRPCOnDie_Implementation()
{
	UPTAnimInstance* AnimInst = Cast<UPTAnimInstance>(GetMesh()->GetAnimInstance());
	if (IsValid(AnimInst))
	{
		AnimInst->SetIsDeath(true);
	}
}

void APTPlayerCharacter::DrawDebugAttackCollision_Implementation(const FColor& DrawColor, FVector TraceStart, FVector TraceEnd, FVector Forward)
{
	const float AttackRange = 100.f;
	const float AttackRadius = 50.f;
	FVector CapsuleOrigin = TraceStart + (TraceEnd - TraceStart) * 0.5f;
	float CapsuleHalfHeight = AttackRange * 0.5f;
	DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, AttackRange, FRotationMatrix::MakeFromZ(Forward).ToQuat(), DrawColor, false, 5.0f);
}