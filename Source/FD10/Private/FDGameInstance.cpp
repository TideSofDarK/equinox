// Fill out your copyright notice in the Description page of Project Settings.


#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Engine/World.h"

#include "FDGameInstance.h"

FArchive& operator<<(FArchive& Ar, FActorSaveData& ActorData)
{
	Ar << ActorData.ActorTransform;
	Ar << ActorData.ActorClass;
	Ar << ActorData.ActorData;

	return Ar;
}

FArchive& operator<<(FArchive& Ar, FItemSaveData& ItemData)
{
	Ar << (FActorSaveData&)ItemData;
	Ar << ItemData.Slot;

	return Ar;
}

FArchive& operator<<(FArchive& Ar, FInventoryAndStashSaveData& InventoryAndStashSaveData)
{
	Ar << InventoryAndStashSaveData.InventoryItems;
	Ar << InventoryAndStashSaveData.StashItems;

	return Ar;
}

void UFDGameInstance::SaveItemGrid(TMap<FIntVector, AActor*> ItemGrid, TArray<FItemSaveData>& SavedItems)
{
	for (auto Pair : ItemGrid)
	{
		AActor* Actor = Pair.Value;

		if (!IsValid(Actor))
		{
			continue;
		}

		FItemSaveData ItemRecord;
		ItemRecord.ActorClass = Actor->GetClass()->GetPathName();
		ItemRecord.ActorTransform = Actor->GetTransform();
		ItemRecord.Slot = Pair.Key;

		FMemoryWriter MemoryWriter(ItemRecord.ActorData, true);
		FSaveGameArchive Ar(MemoryWriter);

		Actor->Serialize(Ar);
		SavedItems.Add(ItemRecord);
	}
}

void UFDGameInstance::LoadItemGrid(TArray<FItemSaveData> SavedItems, TMap<FIntVector, AActor*>& ItemGrid)
{
	for (FItemSaveData ItemRecord : SavedItems)
	{
		FVector SpawnPos = ItemRecord.ActorTransform.GetLocation();
		FRotator SpawnRot = ItemRecord.ActorTransform.Rotator();
		UClass* Result = FindObject<UClass>(ANY_PACKAGE, *ItemRecord.ActorClass);
		if (Result)
		{
			AActor* NewActor = GWorld->SpawnActor(Result, &SpawnPos, &SpawnRot);
			FMemoryReader MemoryReader(ItemRecord.ActorData, true);
			FSaveGameArchive Ar(MemoryReader);
			NewActor->Serialize(Ar);
			NewActor->SetActorTransform(ItemRecord.ActorTransform);

			ItemGrid.Add(ItemRecord.Slot, NewActor);
		}
	}
}

int UFDGameInstance::SaveInventoryAndStash(TMap<FIntVector, AActor*> InventoryGrid, TMap<FIntVector, AActor*> StashGrid)
{
	FInventoryAndStashSaveData InventoryAndStashSaveData;

	UFDGameInstance::SaveItemGrid(InventoryGrid, InventoryAndStashSaveData.InventoryItems);
	UFDGameInstance::SaveItemGrid(StashGrid, InventoryAndStashSaveData.StashItems);

	FBufferArchive ToBinary;

	ToBinary << InventoryAndStashSaveData;

	if (ToBinary.Num() <= 0)
	{
		return -1;
	}
	
	if (FFileHelper::SaveArrayToFile(ToBinary, *FString::Printf(TEXT("%sSaveGames/InventoryAndStash.ersave"), *FPaths::ProjectSavedDir())))
	{
		UE_LOG(LogTemp, Warning, TEXT("Save Success!"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Save Failed!"));
	}

	ToBinary.FlushCache();
	ToBinary.Empty();

	return InventoryAndStashSaveData.InventoryItems.Num() + InventoryAndStashSaveData.StashItems.Num();
}

int UFDGameInstance::LoadInventoryAndStash(TMap<FIntVector, AActor*>& InventoryGrid, TMap<FIntVector, AActor*>& StashGrid)
{
	TArray<uint8> LoadArray;
	
	if (!FFileHelper::LoadFileToArray(LoadArray, *FString::Printf(TEXT("%sSaveGames/InventoryAndStash.ersave"), *FPaths::ProjectSavedDir())))
	{
		UE_LOG(LogTemp, Warning, TEXT("Load Failed!"));
		return 0;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Load Succeeded!"));
	}

	if (LoadArray.Num() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Loaded file empty!"));
		return 0;
	}

	UE_LOG(LogTemp, Warning, TEXT("Save File Size: %d"), LoadArray.Num());

	FMemoryReader FromBinary = FMemoryReader(LoadArray, true);
	FromBinary.Seek(0);

	FInventoryAndStashSaveData InventoryAndStashSaveData;

	FromBinary << InventoryAndStashSaveData;

	FromBinary.FlushCache();
	LoadArray.Empty();
	FromBinary.Close();

	// TMap<FIntVector, AActor*> InventoryGrid, StashGrid;

	LoadItemGrid(InventoryAndStashSaveData.InventoryItems, InventoryGrid);
	LoadItemGrid(InventoryAndStashSaveData.StashItems, StashGrid);

	return InventoryGrid.Num() + StashGrid.Num();
}
