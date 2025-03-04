// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "Serialization/Archive.h"
#include "Serialization/BufferArchive.h"
#include "Serialization/MemoryReader.h"
#include "FDSaveGame.generated.h"

struct FD10_API FSaveGameArchive : public FObjectAndNameAsStringProxyArchive
{
    FSaveGameArchive(FArchive& InInnerArchive)
        : FObjectAndNameAsStringProxyArchive(InInnerArchive, false)
    {
        ArIsSaveGame = true;
        ArNoDelta = true;
    }
};

USTRUCT()
struct FActorSaveData
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, Category = SaveGame)
    FTransform ActorTransform;

    UPROPERTY(VisibleAnywhere, Category = SaveGame)
    FString ActorClass;

    UPROPERTY(VisibleAnywhere, Category = SaveGame)
    TArray<uint8> ActorData;
};

USTRUCT()
struct FItemSaveData : public FActorSaveData
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, Category = SaveGame)
    FIntVector Slot;
};

USTRUCT()
struct FMapSaveData
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, Category = SaveGame)
    TSet<FString> ActorsToDestroy;

    UPROPERTY(VisibleAnywhere, Category = SaveGame)
    TMap<FString, FActorSaveData> SavedActors;
};

/**
 * 
 */
UCLASS()
class FD10_API UFDSaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    static const uint32 PersistentSlot = 0;

    UFDSaveGame();

    UPROPERTY(VisibleAnywhere, Category = SaveGame)
    FActorSaveData PlayerCharacterSaveData;

    UPROPERTY(VisibleAnywhere, Category = SaveGame)
    FName MapName;

    UPROPERTY(VisibleAnywhere, Category = SaveGame)
    TMap<FName, FMapSaveData> SavedMaps;

    UPROPERTY(VisibleAnywhere, Category = SaveGame)
    TArray<FItemSaveData> InventoryItems;

    UPROPERTY(VisibleAnywhere, Category = SaveGame)
    TArray<FItemSaveData> StashItems;

    UPROPERTY(VisibleAnywhere, Category = SaveGame)
    TArray<FColor> ScreenshotData;

    UPROPERTY(VisibleAnywhere, Category = SaveGame)
    FDateTime Timestamp;

    UPROPERTY(VisibleAnywhere, Category = SaveGame)
    float Progress;

    UFUNCTION(BlueprintCallable, Category = "Save Game")
    static void SaveGame(const UObject* WorldContextObject, TMap<FIntVector, AActor*> InventoryGrid,
                         TMap<FIntVector, AActor*> StashGrid, TArray<FString> ActorsToDestroy, int Slot,
                         USceneCaptureComponent2D* SceneCaptureComponent2D);

    UFUNCTION(BlueprintCallable, Category = "Save Game")
    static void LoadGame(const UObject* WorldContextObject, TMap<FIntVector, class ASurvivalItemBase*>& InventoryGrid,
                         TMap<FIntVector, class ASurvivalItemBase*>& StashGrid, int Slot, AActor* & EquippedItem);

    UFUNCTION(BlueprintCallable, Category = "Save Game")
    static bool LoadGamePreview(const UObject* WorldContextObject, int Slot, UTexture2D* ScreenshotTexture,
                                FName& MapNameOut, FDateTime& TimestampOut, float& ProgressOut);
};
