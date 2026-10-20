// Fill out your copyright notice in the Description page of Project Settings.


#include "TestHealthComponent.h"

#include "MathUtil.h"

// Sets default values for this component's properties
UTestHealthComponent::UTestHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UTestHealthComponent::GetDamage(int amount)
{
	if (amount != 0)
	{
		const auto ActualDamage = FMathf::Abs(amount);
		currentHealth = FMathf::Clamp(currentHealth - ActualDamage,0, MaxHealth);

		OnDamageTaken.Broadcast();

		if (currentHealth == 0)
		{
			OnHealthZero.Broadcast();
		}
	}
}

void UTestHealthComponent::HealDamage(int amount)
{
	if (amount != 0)
	{
		const auto ActualDamage = FMathf::Abs(amount);
		currentHealth = FMathf::Clamp(currentHealth + ActualDamage,0, MaxHealth);
	}
}


int UTestHealthComponent::GetCurrentHealth()
{
	return currentHealth;
}


// Called when the game starts
void UTestHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	currentHealth = MaxHealth;
}


// Called every frame
void UTestHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CanRecover)
	{
		timer = FMathf::Clamp(timer + DeltaTime, 0, RecoverTimer);

		if (timer == RecoverTimer)
		{
			HealDamage(RecoverRate);
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, "Health: " + currentHealth);
		}
	}
}

