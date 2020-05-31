// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SurvivalCharacterBase.generated.h"

UCLASS()
class FD10_API ASurvivalCharacterBase : public ACharacter
{
    GENERATED_BODY()

public:
    // Sets default values for this character's properties
    ASurvivalCharacterBase();

    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, SaveGame, Category = "Character")
    bool bIsDead;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, SaveGame, Category = "Character")
    float Health = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Interp, Category = "Character")
    float CutsceneSlotAlpha = 0.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character")
    FName RootBoneName;

protected:
    // Called when the game starts or when spawned
    void BeginPlay() override;

    float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
                     class AController* EventInstigator, AActor* DamageCauser) override;

public:

    // Called to bind functionality to input
    void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    UFUNCTION(BlueprintNativeEvent, Category = "Character")
    void ProcessDamage(float DamageAmount);

    UFUNCTION(BlueprintNativeEvent, Category = "Character")
    void Die();
};
