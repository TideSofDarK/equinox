#include "FDSaveGame.h"

#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "FDGameInstance.h"
#include "SurvivalItemBase.h"

UFDSaveGame::UFDSaveGame()
{
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

void LoadItemGrid(TArray<FItemSaveData> SavedItems, TMap<FIntVector, ASurvivalItemBase*>& ItemGrid)
{
	for (FItemSaveData ItemRecord : SavedItems)
	{
		UClass* Result = FindObject<UClass>(ANY_PACKAGE, *ItemRecord.ActorClass);

		if (Result)
		{
			ASurvivalItemBase* NewItem = Cast<ASurvivalItemBase>(GWorld->SpawnActor(Result, &ItemRecord.ActorTransform));
			FMemoryReader MemoryReader(ItemRecord.ActorData, true);
			FSaveGameArchive Ar(MemoryReader);
			NewItem->Serialize(Ar);
			NewItem->SetActorScale3D(ItemRecord.ActorTransform.GetScale3D());

			NewItem->OnPicked();

			ItemGrid.Add(ItemRecord.Slot, NewItem);
		}
	}
}

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

void UFDSaveGame::LoadGame(TMap<FIntVector, ASurvivalItemBase*>& InventoryGrid, TMap<FIntVector, ASurvivalItemBase*>& StashGrid, int Slot)
{
	if (UFDSaveGame* LoadedGameInstance = Cast<UFDSaveGame>(UGameplayStatics::LoadGameFromSlot(FString::FromInt(Slot), 0)))
	{
		LoadItemGrid(LoadedGameInstance->InventoryItems, InventoryGrid);
		LoadItemGrid(LoadedGameInstance->StashItems, StashGrid);
	}
}
