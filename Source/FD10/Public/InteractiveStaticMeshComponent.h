// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "InteractiveStaticMeshComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = Rendering, meta = (BlueprintSpawnableComponent), Blueprintable)
class FD10_API UInteractiveStaticMeshComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

	UInteractiveStaticMeshComponent(const FObjectInitializer& ObjectInitializer);

public:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	bool bCanBeClicked;

	UFUNCTION(BlueprintCallable)
	void DispatchOnClickedBlueprint(FKey ButtonClicked);

	UFUNCTION(BlueprintCallable)
	void DispatchOnReleasedBlueprint(FKey ButtonClicked);
};
