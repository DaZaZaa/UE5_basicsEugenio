// Fill out your copyright notice in the Description page of Project Settings.


#include "TP_TestCharacter2.h"

#include "EnhancedInputComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "UE5_basics/Component/TestHealthComponent.h"
#include "UE5_basics/Enemy/Enemy.h"
#include "UE5_basics/Other/TargetActor.h"

// Sets default values
ATP_TestCharacter2::ATP_TestCharacter2()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	
	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false;

	// configure health component
	HealthComponent = CreateDefaultSubobject<UTestHealthComponent>(TEXT("HealthComponent"));
}

// Called when the game starts or when spawned
void ATP_TestCharacter2::BeginPlay()
{
	Super::BeginPlay();

	if(MovementCurve)
	{
		FOnTimelineFloat ProgressFunctionLength;
		ProgressFunctionLength.BindUFunction(this, "HandleProgressArmLength");
		AimTimeline.AddInterpFloat(MovementCurve, ProgressFunctionLength);
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Silver, "Bind the timeline!");
	}

	if (OffsetCurve)
	{
		FOnTimelineVector OffsetFunctionLength;
		OffsetFunctionLength.BindUFunction(this, "HandleProgressCameraOffset");
		AimTimeline.AddInterpVector(OffsetCurve, OffsetFunctionLength);
	}
}

// Called every frame
void ATP_TestCharacter2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AimTimeline.TickTimeline(DeltaTime);
}

// Called to bind functionality to input
void ATP_TestCharacter2::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATP_TestCharacter2::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ATP_TestCharacter2::Look);

		// Aim
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &ATP_TestCharacter2::AimIn);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ATP_TestCharacter2::AimOut);
		
		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATP_TestCharacter2::Look);

		//Shooting
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Started, this, &ATP_TestCharacter2::FireFromWeapon);
	}
}

void ATP_TestCharacter2::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void ATP_TestCharacter2::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void ATP_TestCharacter2::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void ATP_TestCharacter2::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void ATP_TestCharacter2::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void ATP_TestCharacter2::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}

void ATP_TestCharacter2::AimIn()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
	AimTimeline.Play();
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Purple, "Aim IN");
}

void ATP_TestCharacter2::AimOut()
{
	GetCharacterMovement()->bOrientRotationToMovement = false;
	AimTimeline.Reverse();
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, "Aim OUT");
}

void ATP_TestCharacter2::HandleProgressArmLength(float Length)
{
	CameraBoom->TargetArmLength = Length;
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, FString::Printf(TEXT("New camer boom val -> %f"), Length));
}

void ATP_TestCharacter2::HandleProgressCameraOffset(FVector Offset) {
	CameraBoom->SocketOffset = Offset;
}

void ATP_TestCharacter2::FireFromWeapon()
{
	FCollisionQueryParams Params;

	const auto MyWorld = GetWorld();
	if (!IsValid(MyWorld))
	{
		return;
	}

	Params.AddIgnoredActor(UGameplayStatics::GetPlayerPawn(MyWorld, 0));

	FHitResult Hit;
	float WeaponRange = 2000;

	FVector Start = FollowCamera->GetComponentLocation();
	FVector End = Start + (FollowCamera->GetForwardVector() * WeaponRange);

	bool bhit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Camera, Params);
	
	DrawDebugLine(GetWorld(), Start, End, FColor::Red, true, 100, 1, 4);
	
	if (bhit)
	{
		/*ATargetActor* HitActor = Cast<ATargetActor>(Hit.GetActor());

		if (IsValid(HitActor))
		{
			HitActor->IncreaseCounter();
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Magenta, "Missed");
		}*/

		UTestHealthComponent* healthComponent = Cast<UTestHealthComponent>(Hit.GetActor());

		if (IsValid(healthComponent))
		{
			healthComponent->LoseStamina(StaminaDamage);
		}
	}
}

void ATP_TestCharacter2::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	if (!DefaultMappingContext)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, "DefaultMappingContext e' NULL!");
		return;
	}

	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, "IMC aggiunto");
		}
	}
}

