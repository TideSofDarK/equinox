// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RHI.h"
#include "GameplayUtilities.generated.h"

UENUM(BlueprintType)
enum class EDirection : uint8
{
	E_UP 		UMETA(DisplayName = "Up"),
	E_LEFT 		UMETA(DisplayName = "Left"),
	E_DOWN 		UMETA(DisplayName = "Down"),
	E_RIGHT 	UMETA(DisplayName = "Right")
};

/**
 *
 */
UCLASS()
class FD10_API UGameplayUtilities : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	static UUserWidget* InventoryNextItem(TMap<FIntVector, UUserWidget*> Inventory, INT Width, INT Height, FIntVector Start, EDirection Direction);

	UFUNCTION(Category = "Victory BP Library|SceneCapture", BlueprintPure, Meta = (DefaultToSelf = "Target"))
	static void CaptureComponent2D_DeProject(class USceneCaptureComponent2D* Target, const FVector2D& ScreenPos, FVector& OutWorldOrigin, FVector& OutWorldDirection);

	UFUNCTION(Category = "Victory BP Library|SceneCapture", BlueprintPure, Meta = (DefaultToSelf = "Target"))
	static void Capture2D_DeProject(class ASceneCapture2D* Target, const FVector2D& ScreenPos, FVector& OutWorldOrigin, FVector& OutWorldDirection);

	UFUNCTION(Category = "Victory BP Library|SceneCapture", BlueprintPure)
	static bool CaptureComponent2D_Project(class USceneCaptureComponent2D* Target, FVector Location, FVector2D& OutPixelLocation);

	UFUNCTION(Category = "Victory BP Library|SceneCapture", BlueprintPure, Meta = (DefaultToSelf = "Target"))
	static bool Capture2D_Project(class ASceneCapture2D* Target, FVector Location, FVector2D& OutPixelLocation);

	UFUNCTION(BlueprintPure)
	static bool WasSpawnedInEditor(class AActor* Actor);
};
