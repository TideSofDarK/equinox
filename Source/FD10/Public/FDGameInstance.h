// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "Serialization/Archive.h"
#include "Serialization/BufferArchive.h"
#include "Serialization/MemoryReader.h"
#include "Engine/GameInstance.h"
#include "FDGameInstance.generated.h"

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
	FTransform ActorTransform;
	FString ActorClass;
	TArray<uint8> ActorData;

	//void RightShiftImplementation(FArchive& Ar)
	//{
	//	Ar << ActorTransform;
	//	Ar << ActorClass;
	//	Ar << ActorData;
	//}

	friend FArchive& operator<<(FArchive& Ar, FActorSaveData& ActorData);
};

USTRUCT()
struct FItemSaveData : public FActorSaveData
{
	GENERATED_BODY()
	FIntVector Slot;

	//void RightShiftImplementation(FArchive& Ar)
	//{
	//	FActorSaveData::RightShiftImplementation(Ar);
	//	Ar << Slot;
	//}

	friend FArchive& operator<<(FArchive& Ar, FItemSaveData& ItemData);
};

USTRUCT()
struct FInventoryAndStashSaveData
{
	GENERATED_BODY()

	TArray<FItemSaveData> InventoryItems, StashItems;

	friend FArchive& operator<<(FArchive& Ar, FInventoryAndStashSaveData& InventoryAndStashSaveData);
};

/**
 * 
 */
UCLASS()
class FD10_API UFDGameInstance : public UGameInstance
{
	GENERATED_BODY()

private:
	static void SaveItemGrid(TMap<FIntVector, AActor*> ItemGrid, TArray<FItemSaveData>& SavedItems);
	static void LoadItemGrid(TArray<FItemSaveData> SavedItems, TMap<FIntVector, AActor*>& ItemGrid);

public:
	UFUNCTION(BlueprintCallable, Category = "Save Game")
	static int SaveInventoryAndStash(TMap<FIntVector, AActor*> InventoryGrid, TMap<FIntVector, AActor*> StashGrid);

	UFUNCTION(BlueprintCallable, Category = "Save Game")
	static int LoadInventoryAndStash(TMap<FIntVector, AActor*>& InventoryGrid, TMap<FIntVector, AActor*>& StashGrid);
};
