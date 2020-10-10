// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivalPlayerState.h"


#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"

void ASurvivalPlayerState::AddPostProcessMaterial(UMaterial* Material) const
{
    const auto CameraComponent = Cast<UCameraComponent>(UGameplayStatics::GetPlayerController(this, 0)->GetViewTarget()->FindComponentByClass(UCameraComponent::StaticClass()));

    if (IsValid(CameraComponent))
    {
        CameraComponent->PostProcessSettings.AddBlendable(Material, 1.0f);
    }
}

void ASurvivalPlayerState::RemovePostProcessMaterial(UMaterial* Material) const
{
    const auto CameraComponent = Cast<UCameraComponent>(UGameplayStatics::GetPlayerController(this, 0)->GetViewTarget()->FindComponentByClass(UCameraComponent::StaticClass()));

    if (IsValid(CameraComponent))
    {
        CameraComponent->PostProcessSettings.RemoveBlendable(Material);
    }
}
