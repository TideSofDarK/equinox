#include "FDSaveGame.h"

#include "Misc/FileHelper.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "FDGameInstance.h"
#include "SurvivalItemBase.h"
#include "SurvivalSaveable.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/SceneCaptureComponent2D.h"

void ApplyActorRecord(AActor* Actor, FActorSaveData ActorRecord, bool bSkipTransform = false)
{
    FMemoryReader MemoryReader(ActorRecord.ActorData, true);
    FSaveGameArchive Ar(MemoryReader);
    Actor->Serialize(Ar);
    if (!bSkipTransform)
    {
        Actor->SetActorTransform(ActorRecord.ActorTransform);
        Actor->SetActorScale3D(ActorRecord.ActorTransform.GetScale3D());
    }
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

ASurvivalItemBase* LoadItemGrid(TArray<FItemSaveData> SavedItems, TMap<FIntVector, ASurvivalItemBase*>& ItemGrid)
{
    ASurvivalItemBase* EquippedItem = nullptr;
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

                if (NewItem->bIsEquipped)
                {
                    EquippedItem = NewItem;
                }
            }
        }
    }
    return EquippedItem;
}

UFDSaveGame::UFDSaveGame()
{
}

void UFDSaveGame::SaveGame(const UObject* WorldContextObject, TMap<FIntVector, AActor*> InventoryGrid,
                           TMap<FIntVector, AActor*> StashGrid, TArray
                           <FString> ActorsToDestroy, int Slot, USceneCaptureComponent2D* SceneCaptureComponent2D)
{
    UFDGameInstance* FDGameInstance = Cast<UFDGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
    UFDSaveGame* LoadedGameInstance = Cast<UFDSaveGame>(
        UGameplayStatics::LoadGameFromMemory(FDGameInstance->PersistentSaveData));

    UFDSaveGame* SaveGameInstance = IsValid(LoadedGameInstance)
                                        ? LoadedGameInstance
                                        : Cast<UFDSaveGame>(
                                            UGameplayStatics::CreateSaveGameObject(StaticClass()));
    SaveGameInstance->Timestamp = FDateTime::Now();

    if (SaveGameInstance)
    {
        UE_LOG(LogTemp, Warning, TEXT("SaveGame class created"));

        // Save screenshot if available
        if (SceneCaptureComponent2D != nullptr)
        {
            UTexture2D* ScreenshotTexture = SceneCaptureComponent2D->TextureTarget->ConstructTexture2D(
                SaveGameInstance, FString("Screenshot"),
                RF_NoFlags);
            ScreenshotTexture->CompressionSettings = TC_VectorDisplacementmap;
            ScreenshotTexture->MipGenSettings = TMGS_NoMipmaps;
            ScreenshotTexture->SRGB = false;
            ScreenshotTexture->UpdateResource();

            const FColor* FormattedImageData = StaticCast<const FColor*>(
                ScreenshotTexture->PlatformData->Mips[0].BulkData.LockReadOnly());

            for (int32 X = 0; X < 256; X++)
            {
                for (int32 Y = 0; Y < 256; Y++)
                {
                    FColor Color = FormattedImageData[X * 256 + Y];
                    SaveGameInstance->ScreenshotData.Add(Color);
                }
            }

            ScreenshotTexture->PlatformData->Mips[0].BulkData.Unlock();
        }

        SaveGameInstance->MapName = FName(*WorldContextObject->GetWorld()->GetName());

        // Save player character state

        SaveActor(UGameplayStatics::GetPlayerPawn(WorldContextObject, 0), SaveGameInstance->PlayerCharacterSaveData);

        // Find existing map record

        FMapSaveData MapSaveData;
        if (SaveGameInstance->SavedMaps.Contains(SaveGameInstance->MapName))
        {
            MapSaveData = SaveGameInstance->SavedMaps[SaveGameInstance->MapName];
        }

        // Save destroyed actors with SaveDestroyed tag

        MapSaveData.ActorsToDestroy.Append(ActorsToDestroy);

        // Save actors with saveable interface

        TArray<AActor*> SaveableActors;
        UGameplayStatics::GetAllActorsWithInterface(WorldContextObject, USurvivalSaveable::StaticClass(),
                                                    SaveableActors);

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

        if (Slot == PersistentSlot)
        {
            if (UGameplayStatics::SaveGameToMemory(SaveGameInstance, FDGameInstance->PersistentSaveData))
            {
            }
        }
        else if (UGameplayStatics::SaveGameToSlot(SaveGameInstance, FString::FromInt(Slot), 0))
        {
        }
    }
}

void UFDSaveGame::LoadGame(const UObject* WorldContextObject, TMap<FIntVector, ASurvivalItemBase*>& InventoryGrid,
                           TMap<FIntVector, ASurvivalItemBase*>& StashGrid, int Slot, AActor* & EquippedItem)
{
    UFDSaveGame* LoadedGameInstance = nullptr;
    const bool bIsPersistentSlot = Slot == PersistentSlot;

    if (bIsPersistentSlot)
    {
        UFDGameInstance* FDGameInstance = Cast<UFDGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
        LoadedGameInstance = Cast<UFDSaveGame
        >(UGameplayStatics::LoadGameFromMemory(FDGameInstance->PersistentSaveData));
    }
    else
    {
        LoadedGameInstance = Cast<UFDSaveGame>(UGameplayStatics::LoadGameFromSlot(FString::FromInt(Slot), 0));
    }

    if (IsValid(LoadedGameInstance))
    {
        UE_LOG(LogTemp, Warning, TEXT("Loaded map name: %s"), *LoadedGameInstance->MapName.ToString());

        const FName CurrentMapName = FName(*WorldContextObject->GetWorld()->GetName());

        // Load player character state

        ApplyActorRecord(UGameplayStatics::GetPlayerPawn(WorldContextObject, 0),
                         LoadedGameInstance->PlayerCharacterSaveData, bIsPersistentSlot);

        if (LoadedGameInstance->SavedMaps.Contains(CurrentMapName))
        {
            // Select needed map record

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
            UGameplayStatics::GetAllActorsWithInterface(WorldContextObject, USurvivalSaveable::StaticClass(),
                                                        SaveableActors);

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

        EquippedItem = LoadItemGrid(LoadedGameInstance->InventoryItems, InventoryGrid);
        LoadItemGrid(LoadedGameInstance->StashItems, StashGrid);
    }
}

bool UFDSaveGame::LoadGamePreview(const UObject* WorldContextObject, int Slot, UTexture2D* & ScreenshotTextureOut,
                                  FName& MapNameOut, FDateTime& TimestampOut, float& ProgressOut)
{
    UFDSaveGame* LoadedGameInstance = nullptr;
    const bool bIsPersistentSlot = Slot == PersistentSlot;

    if (bIsPersistentSlot)
    {
        UFDGameInstance* FDGameInstance = Cast<UFDGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
        LoadedGameInstance = Cast<UFDSaveGame
        >(UGameplayStatics::LoadGameFromMemory(FDGameInstance->PersistentSaveData));
    }
    else
    {
        LoadedGameInstance = Cast<UFDSaveGame>(UGameplayStatics::LoadGameFromSlot(FString::FromInt(Slot), 0));
    }

    if (!IsValid(LoadedGameInstance)) return false;

    ScreenshotTextureOut = UTexture2D::CreateTransient(256, 256, PF_B8G8R8A8);
    ScreenshotTextureOut->CompressionSettings = TC_VectorDisplacementmap;
    ScreenshotTextureOut->MipGenSettings = TMGS_NoMipmaps;
    ScreenshotTextureOut->SRGB = false;
    ScreenshotTextureOut->UpdateResource();
    FTexture2DMipMap& Mip = ScreenshotTextureOut->PlatformData->Mips[0];
    void* Data = Mip.BulkData.Lock(LOCK_READ_WRITE);
    FMemory::Memcpy(Data, LoadedGameInstance->ScreenshotData.GetData(), 256 * 256 * 4);
    Mip.BulkData.Unlock();
    ScreenshotTextureOut->UpdateResource();

    MapNameOut = LoadedGameInstance->MapName;
    TimestampOut = LoadedGameInstance->Timestamp;
    ProgressOut = LoadedGameInstance->Progress;

    return true;
}
