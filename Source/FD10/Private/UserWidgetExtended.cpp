// Fill out your copyright notice in the Description page of Project Settings.

#include "UserWidgetExtended.h"

void UUserWidgetExtended::ListenForInputActionExtended(FName ActionName, TEnumAsByte<EInputEvent> EventType,
                                                       bool bConsume, bool bExecuteWhenPaused, FOnInputAction Callback)
{
    if (!InputComponent)
    {
        InitializeInputComponent();
    }

    if (InputComponent)
    {
        FInputActionBinding NewBinding(ActionName, EventType.GetValue());
        NewBinding.bConsumeInput = bConsume;
        NewBinding.bExecuteWhenPaused = bExecuteWhenPaused;
        NewBinding.ActionDelegate.GetDelegateForManualSet().BindUObject(this, &ThisClass::OnInputAction, Callback);

        InputComponent->AddActionBinding(NewBinding);
    }
}

UTexture2D* UUserWidgetExtended::CreateTransientTexture2D(const int Width, const int Height)
{
    UTexture2D* NewTexture = UTexture2D::CreateTransient(Width, Height, PF_B8G8R8A8);
    NewTexture->CompressionSettings = TC_VectorDisplacementmap;
    NewTexture->SRGB = false;
    NewTexture->UpdateResource();
    return NewTexture;
}
