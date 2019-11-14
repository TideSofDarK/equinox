// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivalCharacterBase.h"

// Sets default values
ASurvivalCharacterBase::ASurvivalCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	RootBoneName = TEXT("pelvis");
}

// Called when the game starts or when spawned
void ASurvivalCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

float ASurvivalCharacterBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	ProcessDamage(DamageAmount);

	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

// Called to bind functionality to input
void ASurvivalCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ASurvivalCharacterBase::ProcessDamage_Implementation(float DamageAmount)
{
	Health -= DamageAmount;

	if (Health <= 0.0f)
	{
		bIsDead = true;

		Die();
	}
}

void ASurvivalCharacterBase::Die_Implementation()
{
	
}

