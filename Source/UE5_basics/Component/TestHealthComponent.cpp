// Fill out your copyright notice in the Description page of Project Settings.


#include "TestHealthComponent.h"

#include "MathUtil.h"

// Sets default values for this component's properties
UTestHealthComponent::UTestHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	currentHealth = MaxHealth;
	currentStamina = MaxStamina;
	// ...
}

void UTestHealthComponent::GetDamage(int amount)
{
	if (amount != 0)
	{
		const auto ActualDamage = FMathf::Abs(amount);
		currentHealth = FMathf::Clamp(currentHealth - ActualDamage,0, MaxHealth);

		/*OnDamageTaken.Broadcast();

		if (currentHealth == 0)
		{
			OnHealthZero.Broadcast();
		}*/

		healthTimer = 0;

		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, FString::FromInt(currentHealth));
	}
}

void UTestHealthComponent::HealDamage(int amount)
{
	if (amount != 0)
	{
		const auto ActualDamage = FMathf::Abs(amount);
		currentHealth = FMathf::Clamp(currentHealth + ActualDamage,0, MaxHealth);

		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, FString::FromInt(currentHealth));
	}
}

void UTestHealthComponent::LoseStamina(int amount)
{
	if (amount != 0 && !WasStunned)
	{
		const auto ActualCost = FMathf::Abs(amount);
		currentStamina = FMathf::Clamp(currentStamina - ActualCost,0, MaxStamina);

		if (currentStamina == 0)
		{
			WasStunned = true;
		}
	}
}

void UTestHealthComponent::GainStamina(int amount)
{
	if (amount != 0)
	{
		const auto ActualCost = FMathf::Abs(amount);
		currentStamina = FMathf::Clamp(currentStamina + ActualCost,0, MaxStamina);
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
}


// Called every frame
void UTestHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CanRecover && currentHealth < MaxHealth)
	{
		if (healthTimer < RecoverTimer)
		{
			healthTimer = FMathf::Clamp(healthTimer + DeltaTime, 0, RecoverTimer);
			//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("%f"), timer));
		}
		else
		{
			HealDamage(HealthRecover);
			healthTimer = 0;
			//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::FromInt(currentHealth));
		}
	}

	if (currentStamina < MaxStamina)
	{
		if (staminaTimer < RecoverTimer)
		{
			staminaTimer = FMathf::Clamp(staminaTimer + DeltaTime, 0, RecoverTimer);
		}
		else
		{
			GainStamina(StaminaRecover);
			staminaTimer = 0;

			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::FromInt(currentStamina));
			
			if (currentStamina == MaxStamina)
			{
				WasStunned = false;
			}
		}
	}
}

