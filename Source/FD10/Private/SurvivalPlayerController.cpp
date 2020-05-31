// Fill out your copyright notice in the Description page of Project Settings.

#include "SurvivalPlayerController.h"

void ASurvivalPlayerController::HideActor(AActor* Actor)
{
    HiddenActors.Add(Actor);
}

void ASurvivalPlayerController::ShowActor(AActor* Actor)
{
    HiddenActors.Remove(Actor);
}
