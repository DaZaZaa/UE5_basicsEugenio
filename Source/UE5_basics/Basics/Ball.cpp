// Fill out your copyright notice in the Description page of Project Settings.


#include "Ball.h"

#include "UE5_basics/TP_Character/TP_TestCharacter.h"

// Sets default values
ABall::ABall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
}

// Called when the game starts or when spawned
void ABall::BeginPlay()
{
	Super::BeginPlay();

	OnActorBeginOverlap.AddDynamic(this, &ABall::OnBeginOverlap);
}

void ABall::OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if(!IsValid(OtherActor))
	{
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, "Ball");
	
	UClass* OtherActorClass = OtherActor->GetClass();
	for(auto& SensedClass : SensedCharacterClasses)
	{
		if(SensedClass == OtherActorClass || OtherActorClass->IsChildOf(SensedClass))
		{
			USceneComponent* SensedHolder = Cast<ATP_TestCharacter>(OtherActor)->BallHolder;
			
			this->AttachToComponent(SensedHolder, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SensedHolder->GetAttachSocketName());
			
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, "Sphere Acquired.");
			//OnSomethingEnter();
			break;
		}
	}
}

// Called every frame
void ABall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

