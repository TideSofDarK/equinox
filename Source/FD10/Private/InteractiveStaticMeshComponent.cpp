// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractiveStaticMeshComponent.h"

UInteractiveStaticMeshComponent::UInteractiveStaticMeshComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetCollisionProfileName(TEXT("Survival_InteractiveScene"));
}

void UInteractiveStaticMeshComponent::DispatchOnClickedBlueprint(FKey ButtonClicked)
{
	DispatchOnClicked();
}

void UInteractiveStaticMeshComponent::DispatchOnReleasedBlueprint(FKey ButtonClicked)
{
	DispatchOnReleased();
}