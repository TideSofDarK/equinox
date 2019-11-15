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
	}
};

USTRUCT()
struct FActorSaveData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = Basic)
	FTransform ActorTransform;

	UPROPERTY(VisibleAnywhere, Category = Basic)
	FString ActorClass;

	UPROPERTY(VisibleAnywhere, Category = Basic)
	TArray<uint8> ActorData;
};

USTRUCT()
struct FItemSaveData : public FActorSaveData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = Basic)
	FIntVector Slot;
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

	UPROPERTY(VisibleAnywhere, Category = Basic)
	TArray<FItemSaveData> InventoryItems;

	UPROPERTY(VisibleAnywhere, Category = Basic)
	TArray<FItemSaveData> StashItems;

	UPROPERTY(VisibleAnywhere, Category = Basic)
	FDateTime Timestamp;

	UFUNCTION(BlueprintCallable, Category = "Save Game")
	static void SaveGame(TMap<FIntVector, AActor*> InventoryGrid, TMap<FIntVector, AActor*> StashGrid, int Slot);

	UFUNCTION(BlueprintCallable, Category = "Save Game")
	static void LoadGame(TMap<FIntVector, class ASurvivalItemBase*>& InventoryGrid, TMap<FIntVector, class ASurvivalItemBase*>& StashGrid, int Slot);
};
