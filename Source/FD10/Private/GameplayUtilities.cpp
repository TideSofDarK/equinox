// Fill out your copyright notice in the Description page of Project Settings.

#include "GameplayUtilities.h"

#if WITH_EDITOR
#include "AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "IContentBrowserSingleton.h"
#include "PackageTools.h"

#define LOCTEXT_NAMESPACE "KismetRenderingLibrary"
#endif

UUserWidget* UGameplayUtilities::InventoryNextItem(TMap<FIntVector, UUserWidget*> Inventory, int Width, int Height, FIntVector Start, EDirection Direction)
{
	// UE_LOG(LogTemp, Warning, TEXT("Your message %s"), *Start.ToString());
	if (Direction == EDirection::E_DOWN)
	{
		INT NewStartY = Start.Y + 1;

		for (INT I = NewStartY; I < Height; I++)
		{
			UUserWidget** Widget = Inventory.Find(FIntVector(Start.X, I, 0));
			if (Widget == nullptr)
			{

			}
			else
			{
				if (Inventory.Find(Start) != nullptr && *Widget == *Inventory.Find(Start))
				{
					NewStartY = I + 1;
					continue;
				}
				else
				{
					return *Widget;
				}
			}
		}
		if (NewStartY >= Height)
		{
			return nullptr;
		}
		for (INT Offset = 1; Offset < Width; Offset++)
		{
			for (INT Y = NewStartY; Y < Height; Y++)
			{
				UUserWidget** Widget = Inventory.Find(FIntVector(Start.X + Offset, Y, 0));
				if (Widget != nullptr)
				{
					return *Widget;
				}

				Widget = Inventory.Find(FIntVector(Start.X - Offset, Y, 0));
				if (Widget != nullptr)
				{
					return *Widget;
				}
			}
		}
	}

	if (Direction == EDirection::E_UP)
	{
		INT NewStartY = Start.Y - 1;

		for (INT I = NewStartY; I >= 0; I--)
		{
			UUserWidget** Widget = Inventory.Find(FIntVector(Start.X, I, 0));
			if (Widget == nullptr)
			{

			}
			else
			{
				if (Inventory.Find(Start) != nullptr && *Widget == *Inventory.Find(Start))
				{
					NewStartY = I - 1;
					continue;
				}
				else
				{
					return *Widget;
				}
			}
		}
		if (NewStartY < 0)
		{
			return nullptr;
		}
		for (INT Offset = 1; Offset < Width; Offset++)
		{
			for (INT Y = NewStartY; Y >= 0; Y--)
			{
				UUserWidget** Widget = Inventory.Find(FIntVector(Start.X + Offset, Y, 0));
				if (Widget != nullptr)
				{
					return *Widget;
				}

				Widget = Inventory.Find(FIntVector(Start.X - Offset, Y, 0));
				if (Widget != nullptr)
				{
					return *Widget;
				}
			}
		}
	}

	if (Direction == EDirection::E_LEFT)
	{
		INT NewStartX = Start.X - 1;

		for (INT I = NewStartX; I >= 0; I--)
		{
			UUserWidget** Widget = Inventory.Find(FIntVector(I, Start.Y, 0));
			if (Widget == nullptr)
			{
			}
			else
			{
				if (Inventory.Find(Start) != nullptr && *Widget == *Inventory.Find(Start))
				{
					NewStartX = I - 1;
					continue;
				}
				else
				{
					return *Widget;
				}
			}
		}
		if (NewStartX < 0)
		{
			return nullptr;
		}
		for (INT Offset = 1; Offset < Height; Offset++)
		{
			for (INT X = NewStartX; X >= 0; X--)
			{
				UUserWidget** Widget = Inventory.Find(FIntVector(X, Start.Y + Offset, 0));
				if (Widget != nullptr)
				{
					return *Widget;
				}

				Widget = Inventory.Find(FIntVector(X, Start.Y - Offset, 0));
				if (Widget != nullptr)
				{
					return *Widget;
				}
			}
		}
	}

	if (Direction == EDirection::E_RIGHT)
	{
		INT NewStartX = Start.X + 1;

		for (INT I = NewStartX; I < Width; I++)
		{
			UUserWidget** Widget = Inventory.Find(FIntVector(I, Start.Y, 0));
			if (Widget == nullptr)
			{

			}
			else
			{
				if (Inventory.Find(Start) != nullptr && *Widget == *Inventory.Find(Start))
				{
					NewStartX = I + 1;
					continue;
				}
				else
				{
					return *Widget;
				}
			}
		}
		if (NewStartX >= Width)
		{
			return nullptr;
		}
		for (INT Offset = 1; Offset < Height; Offset++)
		{
			for (INT X = NewStartX; X < Width; X++)
			{
				UUserWidget** Widget = Inventory.Find(FIntVector(X, Start.Y + Offset, 0));
				if (Widget != nullptr)
				{
					return *Widget;
				}

				Widget = Inventory.Find(FIntVector(X, Start.Y - Offset, 0));
				if (Widget != nullptr)
				{
					return *Widget;
				}
			}
		}
	}

	return nullptr;
}

void UGameplayUtilities::CaptureComponent2D_DeProject(class USceneCaptureComponent2D* Target, const FVector2D& ScreenPos, FVector& OutWorldOrigin, FVector& OutWorldDirection)
{
	if ((Target == nullptr) || (Target->TextureTarget == nullptr))
	{
		return;
	}

	const FTransform& Transform = Target->GetComponentToWorld();
	FMatrix ViewMatrix = Transform.ToInverseMatrixWithScale();
	FVector ViewLocation = Transform.GetTranslation();

	// swap axis st. x=z,y=x,z=y (unreal coord space) so that z is up
	ViewMatrix = ViewMatrix * FMatrix(
		FPlane(0, 0, 1, 0),
		FPlane(1, 0, 0, 0),
		FPlane(0, 1, 0, 0),
		FPlane(0, 0, 0, 1));

	const float FOV = Target->FOVAngle * (float)PI / 360.0f;

	FIntPoint CaptureSize(Target->TextureTarget->GetSurfaceWidth(), Target->TextureTarget->GetSurfaceHeight());

	float XAxisMultiplier;
	float YAxisMultiplier;

	if (CaptureSize.X > CaptureSize.Y)
	{
		// if the viewport is wider than it is tall
		XAxisMultiplier = 1.0f;
		YAxisMultiplier = CaptureSize.X / (float)CaptureSize.Y;
	}
	else
	{
		// if the viewport is taller than it is wide
		XAxisMultiplier = CaptureSize.Y / (float)CaptureSize.X;
		YAxisMultiplier = 1.0f;
	}

	FMatrix    ProjectionMatrix = FReversedZPerspectiveMatrix(
		FOV,
		FOV,
		XAxisMultiplier,
		YAxisMultiplier,
		GNearClippingPlane,
		GNearClippingPlane
	);

	const FMatrix InverseViewMatrix = ViewMatrix.InverseFast();
	const FMatrix InvProjectionMatrix = ProjectionMatrix.Inverse();

	const FIntRect ViewRect = FIntRect(0, 0, CaptureSize.X, CaptureSize.Y);

	FSceneView::DeprojectScreenToWorld(ScreenPos, ViewRect, InverseViewMatrix, InvProjectionMatrix, OutWorldOrigin, OutWorldDirection);
}

void UGameplayUtilities::Capture2D_DeProject(class ASceneCapture2D* Target, const FVector2D& ScreenPos, FVector& OutWorldOrigin, FVector& OutWorldDirection)
{
	if (Target)
	{
		CaptureComponent2D_DeProject(Target->GetCaptureComponent2D(), ScreenPos, OutWorldOrigin, OutWorldDirection);
	}
}

bool UGameplayUtilities::CaptureComponent2D_Project(class USceneCaptureComponent2D* Target, FVector Location, FVector2D& OutPixelLocation)
{
	if ((Target == nullptr) || (Target->TextureTarget == nullptr))
	{
		return false;
	}

	const FTransform& Transform = Target->GetComponentToWorld();
	FMatrix ViewMatrix = Transform.ToInverseMatrixWithScale();
	FVector ViewLocation = Transform.GetTranslation();

	// swap axis st. x=z,y=x,z=y (unreal coord space) so that z is up
	ViewMatrix = ViewMatrix * FMatrix(
		FPlane(0, 0, 1, 0),
		FPlane(1, 0, 0, 0),
		FPlane(0, 1, 0, 0),
		FPlane(0, 0, 0, 1));

	const float FOV = Target->FOVAngle * (float)PI / 360.0f;

	FIntPoint CaptureSize(Target->TextureTarget->GetSurfaceWidth(), Target->TextureTarget->GetSurfaceHeight());

	float XAxisMultiplier;
	float YAxisMultiplier;

	if (CaptureSize.X > CaptureSize.Y)
	{
		// if the viewport is wider than it is tall
		XAxisMultiplier = 1.0f;
		YAxisMultiplier = CaptureSize.X / (float)CaptureSize.Y;
	}
	else
	{
		// if the viewport is taller than it is wide
		XAxisMultiplier = CaptureSize.Y / (float)CaptureSize.X;
		YAxisMultiplier = 1.0f;
	}

	FMatrix    ProjectionMatrix = FReversedZPerspectiveMatrix(
		FOV,
		FOV,
		XAxisMultiplier,
		YAxisMultiplier,
		GNearClippingPlane,
		GNearClippingPlane
	);

	FMatrix ViewProjectionMatrix = ViewMatrix * ProjectionMatrix;

	FVector4 ScreenPoint = ViewProjectionMatrix.TransformFVector4(FVector4(Location, 1));

	if (ScreenPoint.W > 0.0f)
	{
		float InvW = 1.0f / ScreenPoint.W;
		float Y = (GProjectionSignY > 0.0f) ? ScreenPoint.Y : 1.0f - ScreenPoint.Y;
		FIntRect ViewRect = FIntRect(0, 0, CaptureSize.X, CaptureSize.Y);
		OutPixelLocation = FVector2D(
			ViewRect.Min.X + (0.5f + ScreenPoint.X * 0.5f * InvW) * ViewRect.Width(),
			ViewRect.Min.Y + (0.5f - Y * 0.5f * InvW) * ViewRect.Height()
		);
		return true;
	}

	return false;
}

bool UGameplayUtilities::Capture2D_Project(class ASceneCapture2D* Target, FVector Location, FVector2D& OutPixelLocation)
{
	return (Target) ? CaptureComponent2D_Project(Target->GetCaptureComponent2D(), Location, OutPixelLocation) : false;
}

bool UGameplayUtilities::WasSpawnedInEditor(class AActor* Actor)
{
	return Actor->HasAnyFlags(RF_WasLoaded);
}

UTexture2D* UGameplayUtilities::CreateAutomapTextureAsset(UTextureRenderTarget2D* RenderTarget, FString InName)
{
#if WITH_EDITOR
	if (RenderTarget && RenderTarget->Resource)
	{
		FString CurrentLevelPath = GWorld->GetCurrentLevel()->GetPathName();
		CurrentLevelPath.RemoveFromEnd(TEXT(":PersistentLevel"));
		CurrentLevelPath.RemoveFromEnd(GWorld->GetWorld()->GetName());
		CurrentLevelPath.RemoveFromEnd(TEXT("."));
		CurrentLevelPath.RemoveFromEnd(GWorld->GetWorld()->GetName());

		CurrentLevelPath.Append(TEXT("T_"));
		CurrentLevelPath.Append(GWorld->GetWorld()->GetName());
		CurrentLevelPath.Append(TEXT("_Automap_"));
		CurrentLevelPath.Append(InName);
		CurrentLevelPath.Append(TEXT("_D"));

		FString Name;
		FString PackageName;
		IAssetTools& AssetTools = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

		// Remove existing
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath(FName(*CurrentLevelPath));
		UObject* AssetToDelete = AssetData.GetAsset();
		if (IsValid(AssetToDelete))
		{
			UE_LOG(LogTemp, Warning, TEXT("stuff: %s"), *AssetToDelete->GetName());
			FAssetRegistryModule::AssetDeleted(AssetToDelete);
			AssetToDelete->ClearFlags(RF_Standalone | RF_Public);
			AssetToDelete->RemoveFromRoot();
			AssetToDelete->MarkPendingKill();
		}

		CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);

		AssetTools.CreateUniqueAssetName(CurrentLevelPath, TEXT(""), PackageName, Name);
		UTexture2D* NewTexture = NULL;
		if (RenderTarget)
		{
			NewTexture = RenderTarget->ConstructTexture2D(CreatePackage(NULL, *PackageName), Name, RenderTarget->GetMaskedFlags() | RF_Public | RF_Standalone, CTF_Default, NULL);
		}

		if (NewTexture)
		{
			NewTexture->MarkPackageDirty();

			FAssetRegistryModule::AssetCreated(NewTexture);

			NewTexture->CompressionSettings = TextureCompressionSettings::TC_EditorIcon;
			NewTexture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
			NewTexture->Filter = TextureFilter::TF_Nearest;
			NewTexture->SRGB = true;

			NewTexture->PostEditChange();
		}

		return NewTexture;
	}
#endif
	return nullptr;
}