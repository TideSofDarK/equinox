// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivalItemBase.h"

// Sets default values
ASurvivalItemBase::ASurvivalItemBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ASurvivalItemBase::OnPicked_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("SurvivalItemBase OnPicked is not implemented"));
}

// Called when the game starts or when spawned
//void ASurvivalItemBase::BeginPlay()
//{
//	Super::BeginPlay();
//	
//}
//
//// Called every frame
//void ASurvivalItemBase::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}
//
