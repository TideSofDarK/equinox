// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UserWidgetExtended.generated.h"

DECLARE_DYNAMIC_DELEGATE(FOnInputAxis);

/**
 * 
 */
UCLASS()
class FD10_API UUserWidgetExtended : public UUserWidget
{
	GENERATED_BODY()
	

public:

	UFUNCTION(BlueprintCallable, Category = "Input", meta = (BlueprintProtected = "true"))
	void ListenForInputActionExtended(FName ActionName, TEnumAsByte< EInputEvent > EventType, bool bConsume, bool bExecuteWhenPaused, FOnInputAction Callback);
};
