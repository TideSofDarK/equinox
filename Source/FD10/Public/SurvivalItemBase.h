// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SurvivalItemBase.generated.h"

UCLASS()
class FD10_API ASurvivalItemBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASurvivalItemBase();

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, SaveGame, Category = "Inventory")
	bool bIsEquipped;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
	void OnPicked();
};
