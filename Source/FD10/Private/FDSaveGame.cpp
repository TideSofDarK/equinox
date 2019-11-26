#include "FDSaveGame.h"

#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "FDGameInstance.h"
#include "SurvivalItemBase.h"
#include "SurvivalSaveable.h"

void ApplyActorRecord(AActor* Actor, FActorSaveData ActorRecord)
{
	FMemoryReader MemoryReader(ActorRecord.ActorData, true);
	FSaveGameArchive Ar(MemoryReader);
	Actor->Serialize(Ar);
	Actor->SetActorTransform(ActorRecord.ActorTransform);
	Actor->SetActorScale3D(ActorRecord.ActorTransform.GetScale3D());
}

void SaveActor(AActor* Actor, FActorSaveData& ActorRecord)
{
	ActorRecord.ActorClass = Actor->GetClass()->GetPathName();
	ActorRecord.ActorTransform = Actor->GetTransform();

	FMemoryWriter MemoryWriter(ActorRecord.ActorData, true);
	FSaveGameArchive Ar(MemoryWriter);
	Actor->Serialize(Ar);
}

AActor* LoadActor(FActorSaveData ActorRecord)
{
	UClass* Result = FindObject<UClass>(ANY_PACKAGE, *ActorRecord.ActorClass);

	if (Result)
	{
		AActor* LoadedActor = Cast<AActor>(GWorld->SpawnActor(Result, &ActorRecord.ActorTransform));
		ApplyActorRecord(LoadedActor, ActorRecord);

		return LoadedActor;
	}

	return nullptr;
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

		SaveActor(Actor, ItemRecord);
		ItemRecord.Slot = Pair.Key;

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
			AActor* LoadedActor = LoadActor(ItemRecord);
			if (IsValid(LoadedActor))
			{
				ASurvivalItemBase* NewItem = Cast<ASurvivalItemBase>(LoadedActor);
				FMemoryReader MemoryReader(ItemRecord.ActorData, true);
				FSaveGameArchive Ar(MemoryReader);
				NewItem->Serialize(Ar);
				NewItem->SetActorScale3D(ItemRecord.ActorTransform.GetScale3D());

				NewItem->OnPicked();

				ItemGrid.Add(ItemRecord.Slot, NewItem);
			}
		}
	}
}

UFDSaveGame::UFDSaveGame()
{
	Timestamp = FDateTime::Now();
}

void UFDSaveGame::SaveGame(const UObject * WorldContextObject, TMap<FIntVector, AActor*> InventoryGrid, TMap<FIntVector, AActor*> StashGrid, TArray<FString> ActorsToDestroy, int Slot)
{
	UFDGameInstance* FDGameInstance = Cast<UFDGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
	UFDSaveGame* LoadedGameInstance = Cast<UFDSaveGame>(UGameplayStatics::LoadGameFromMemory(FDGameInstance->PersistentSaveData));

	UFDSaveGame* SaveGameInstance = IsValid(LoadedGameInstance) ? LoadedGameInstance : Cast<UFDSaveGame>(UGameplayStatics::CreateSaveGameObject(UFDSaveGame::StaticClass()));

	if (SaveGameInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("SaveGame class created"));

		SaveGameInstance->MapName = FName(*WorldContextObject->GetWorld()->GetName());

		FMapSaveData MapSaveData;
		if (SaveGameInstance->SavedMaps.Contains(SaveGameInstance->MapName))
		{
			MapSaveData = SaveGameInstance->SavedMaps[SaveGameInstance->MapName];
		}

		// Save destroyed actors with SaveDestroyed tag

		MapSaveData.ActorsToDestroy.Append(ActorsToDestroy);

		// Save actors with saveable interface
		
		TArray<AActor*> SaveableActors;
		UGameplayStatics::GetAllActorsWithInterface(WorldContextObject, USurvivalSaveable::StaticClass(), SaveableActors);

		for (AActor* ActorToSave : SaveableActors)
		{
			FActorSaveData ActorSaveData;
			ISurvivalSaveable::Execute_OnActorPreSave(ActorToSave);
			SaveActor(ActorToSave, ActorSaveData);
			ISurvivalSaveable::Execute_OnActorPostSave(ActorToSave);
			MapSaveData.SavedActors.Add(ActorToSave->GetName(), ActorSaveData);
		}

		SaveGameInstance->SavedMaps.Add(SaveGameInstance->MapName, MapSaveData);

		// Save inventory and stash

		SaveGameInstance->InventoryItems.Empty();
		SaveGameInstance->StashItems.Empty();

		TArray<FItemSaveData> InventoryItems;
		TArray<FItemSaveData> StashItems;

		SaveItemGrid(InventoryGrid, InventoryItems);
		SaveItemGrid(StashGrid, StashItems);

		SaveGameInstance->InventoryItems.Append(InventoryItems);
		SaveGameInstance->StashItems.Append(StashItems);

		// Write data to memory or disk

		if (Slot == UFDSaveGame::PersistentSlot)
		{
			UFDGameInstance* FDGameInstance = Cast<UFDGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
			if (UGameplayStatics::SaveGameToMemory(SaveGameInstance, FDGameInstance->PersistentSaveData))
			{
				
			}
		}
		else if (UGameplayStatics::SaveGameToSlot(SaveGameInstance, FString::FromInt(Slot), 0))
		{

		}
	}
}

void UFDSaveGame::LoadGame(const UObject * WorldContextObject, TMap<FIntVector, ASurvivalItemBase*>& InventoryGrid, TMap<FIntVector, ASurvivalItemBase*>& StashGrid, int Slot)
{
	UFDSaveGame* LoadedGameInstance = nullptr;
	if (Slot == UFDSaveGame::PersistentSlot)
	{
		UFDGameInstance* FDGameInstance = Cast<UFDGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
		LoadedGameInstance = Cast<UFDSaveGame>(UGameplayStatics::LoadGameFromMemory(FDGameInstance->PersistentSaveData));
	}
	else
	{
		LoadedGameInstance = Cast<UFDSaveGame>(UGameplayStatics::LoadGameFromSlot(FString::FromInt(Slot), 0));
	}

	if (IsValid(LoadedGameInstance))
	{
		UE_LOG(LogTemp, Warning, TEXT("Loaded map name: %s"), *LoadedGameInstance->MapName.ToString());

		FName CurrentMapName = FName(*WorldContextObject->GetWorld()->GetName());

		if (LoadedGameInstance->SavedMaps.Contains(CurrentMapName))
		{
			FMapSaveData MapSaveData = LoadedGameInstance->SavedMaps[CurrentMapName];

			// Destroy level actors

			TArray<AActor*> ActorsToDestroy;
			UGameplayStatics::GetAllActorsOfClass(WorldContextObject, AActor::StaticClass(), ActorsToDestroy);
			for (auto ActorToDestroy : ActorsToDestroy)
			{
				if (MapSaveData.ActorsToDestroy.Contains(ActorToDestroy->GetName()))
				{
					GWorld->DestroyActor(ActorToDestroy);
				}
			}

			// Load saveable actors

			TArray<AActor*> SaveableActors;
			UGameplayStatics::GetAllActorsWithInterface(WorldContextObject, USurvivalSaveable::StaticClass(), SaveableActors);

			for (AActor* ActorToLoad : SaveableActors)
			{
				if (MapSaveData.SavedActors.Contains(ActorToLoad->GetName()))
				{
					ISurvivalSaveable::Execute_OnActorPreLoad(ActorToLoad);
					ApplyActorRecord(ActorToLoad, MapSaveData.SavedActors[ActorToLoad->GetName()]);
					ISurvivalSaveable::Execute_OnActorPostLoad(ActorToLoad);
					UE_LOG(LogTemp, Warning, TEXT("Loading actor: %s"), *ActorToLoad->GetName());
				}
			}
		}	

		// Load inventory and stash

		LoadItemGrid(LoadedGameInstance->InventoryItems, InventoryGrid);
		LoadItemGrid(LoadedGameInstance->StashItems, StashGrid);
	}
}
