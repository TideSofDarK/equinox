#include "FDSaveGame.h"

#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
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

UFDSaveGame::UFDSaveGame()
{
	//SaveSlotName = TEXT("TestSaveSlot");
	//UserIndex = 0;
	Timestamp = FDateTime::Now();
}

void SaveItemGrid(TMap<FIntVector, AActor*> ItemGrid, TArray<FItemSaveData>& SavedItems)
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

void LoadItemGrid(TArray<FItemSaveData> SavedItems, TMap<FIntVector, AActor*>& ItemGrid)
{
	for (FItemSaveData ItemRecord : SavedItems)
	{
		FVector SpawnPos = ItemRecord.ActorTransform.GetLocation();
		FRotator SpawnRot = ItemRecord.ActorTransform.Rotator();
		UClass* Result = FindObject<UClass>(ANY_PACKAGE, *ItemRecord.ActorClass);
		UE_LOG(LogTemp, Warning, TEXT("Loaded actor: %s"), *ItemRecord.ActorClass);
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
//
//int UFDSaveGame::SaveInventoryAndStash(TMap<FIntVector, AActor*> InventoryGrid, TMap<FIntVector, AActor*> StashGrid)
//{
//	FInventoryAndStashSaveData InventoryAndStashSaveData;
//
//	UFDSaveGame::SaveItemGrid(InventoryGrid, InventoryAndStashSaveData.InventoryItems);
//	UFDSaveGame::SaveItemGrid(StashGrid, InventoryAndStashSaveData.StashItems);
//
//	// UFDGameInstance* GameInstance = Cast<UFDGameInstance>(UGameplayStatics::GetGameInstance());
//
//	// 
//	UE_LOG(LogTemp, Warning, TEXT("SaveGame class creation begins..."));
//	if (UFDSaveGame* SaveGameInstance = Cast<UFDSaveGame>(UGameplayStatics::CreateSaveGameObject(UFDSaveGame::StaticClass())))
//	{
//		UE_LOG(LogTemp, Warning, TEXT("SaveGame class created"));
//
//		UFDSaveGame::SaveItemGrid(InventoryGrid, SaveGameInstance->InventoryAndStashSaveData.InventoryItems);
//		UFDSaveGame::SaveItemGrid(StashGrid, SaveGameInstance->InventoryAndStashSaveData.StashItems);
//
//		if (UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->SaveSlotName, SaveGameInstance->UserIndex))
//		{
//			UE_LOG(LogTemp, Warning, TEXT("SaveGame API Success"));
//		}
//		else
//		{
//			UE_LOG(LogTemp, Warning, TEXT("SaveGame API Failure"));
//		}
//	}
//	else
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Couldn't create SaveGame class"));
//	}
//	//
//
//	//FBufferArchive ToBinary;
//
//	//ToBinary << InventoryAndStashSaveData;
//
//	//if (ToBinary.Num() <= 0)
//	//{
//	//	return -1;
//	//}
//
//	//if (FFileHelper::SaveArrayToFile(ToBinary, *FString::Printf(TEXT("%sSaveGames/InventoryAndStash.ersave"), *FPaths::ProjectSavedDir())))
//	//{
//	//	UE_LOG(LogTemp, Warning, TEXT("Save Success!"));
//	//}
//	//else
//	//{
//	//	UE_LOG(LogTemp, Warning, TEXT("Save Failed!"));
//	//}
//
//	//ToBinary.FlushCache();
//	//ToBinary.Empty();
//
//	return InventoryAndStashSaveData.InventoryItems.Num() + InventoryAndStashSaveData.StashItems.Num();
//}
//
//int UFDSaveGame::LoadInventoryAndStash(TMap<FIntVector, AActor*>& InventoryGrid, TMap<FIntVector, AActor*>& StashGrid)
//{
//	TArray<uint8> LoadArray;
//
//	if (!FFileHelper::LoadFileToArray(LoadArray, *FString::Printf(TEXT("%sSaveGames/InventoryAndStash.ersave"), *FPaths::ProjectSavedDir())))
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Load Failed!"));
//		return 0;
//	}
//	else
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Load Succeeded!"));
//	}
//
//	if (LoadArray.Num() <= 0)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Loaded file empty!"));
//		return 0;
//	}
//
//	UE_LOG(LogTemp, Warning, TEXT("Save File Size: %d"), LoadArray.Num());
//
//	FMemoryReader FromBinary = FMemoryReader(LoadArray, true);
//	FromBinary.Seek(0);
//
//	FInventoryAndStashSaveData InventoryAndStashSaveData;
//
//	FromBinary << InventoryAndStashSaveData;
//
//	FromBinary.FlushCache();
//	LoadArray.Empty();
//	FromBinary.Close();
//
//	// TMap<FIntVector, AActor*> InventoryGrid, StashGrid;
//
//	LoadItemGrid(InventoryAndStashSaveData.InventoryItems, InventoryGrid);
//	LoadItemGrid(InventoryAndStashSaveData.StashItems, StashGrid);
//
//	return InventoryGrid.Num() + StashGrid.Num();
//}

void UFDSaveGame::SaveGame(TMap<FIntVector, AActor*> InventoryGrid, TMap<FIntVector, AActor*> StashGrid, int Slot)
{
	UFDSaveGame* SaveGameInstance = Cast<UFDSaveGame>(UGameplayStatics::CreateSaveGameObject(UFDSaveGame::StaticClass()));
	if (SaveGameInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("SaveGame class created"));

		TArray<FItemSaveData> InventoryItems;
		TArray<FItemSaveData> StashItems;

		SaveItemGrid(InventoryGrid, InventoryItems);
		SaveItemGrid(StashGrid, StashItems);

		SaveGameInstance->InventoryItems.Append(InventoryItems);
		SaveGameInstance->StashItems.Append(StashItems);

		UE_LOG(LogTemp, Warning, TEXT("Inventory size: %d"), SaveGameInstance->InventoryItems.Num());
		UE_LOG(LogTemp, Warning, TEXT("Stash size: %d"), SaveGameInstance->StashItems.Num());

		if (UGameplayStatics::SaveGameToSlot(SaveGameInstance, FString::FromInt(Slot), 0))
		{

		}
	}
}

void UFDSaveGame::LoadGame(TMap<FIntVector, AActor*>& InventoryGrid, TMap<FIntVector, AActor*>& StashGrid, int Slot)
{
	if (UFDSaveGame* LoadedGameInstance = Cast<UFDSaveGame>(UGameplayStatics::LoadGameFromSlot(FString::FromInt(Slot), 0)))
	{
		LoadItemGrid(LoadedGameInstance->InventoryItems, InventoryGrid);
		LoadItemGrid(LoadedGameInstance->StashItems, StashGrid);
	}
}
