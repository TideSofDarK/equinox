// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SurvivalSaveable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USurvivalSaveable : public UInterface
{
    GENERATED_BODY()
};

/**
 * 
 */
class FD10_API ISurvivalSaveable
{
    GENERATED_BODY()

    // Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "SaveGame")
    void OnActorPreSave();

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "SaveGame")
    void OnActorPostSave();

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "SaveGame")
    void OnActorPreLoad();

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "SaveGame")
    void OnActorPostLoad();
};
