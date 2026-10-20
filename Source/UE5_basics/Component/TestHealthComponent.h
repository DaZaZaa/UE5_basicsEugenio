// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TestHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHealthDelegate);
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE5_BASICS_API UTestHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTestHealthComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxHealth = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool CanRecover;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RecoverTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int RecoverRate;
	
	UFUNCTION(BlueprintCallable)
	void GetDamage(int amount);

	UFUNCTION(BlueprintCallable)
	void HealDamage(int amount);

	UPROPERTY(BlueprintAssignable)
	FHealthDelegate OnHealthZero;

	UPROPERTY(BlueprintAssignable)
	FHealthDelegate OnDamageTaken;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintPure)
	int GetCurrentHealth();
	
private:

	int currentHealth;
	float timer;
};
