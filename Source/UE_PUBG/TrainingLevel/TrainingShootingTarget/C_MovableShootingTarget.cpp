// Fill out your copyright notice in the Description page of Project Settings.


#include "C_MovableShootingTarget.h"


AC_MovableShootingTarget::AC_MovableShootingTarget()
{
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* RootComp = CreateDefaultSubobject<USceneComponent>(FName("Root"));
	SetRootComponent(RootComp);

	Anchor0 = CreateDefaultSubobject<USceneComponent>(TEXT("AnchorOne"));
	Anchor1 = CreateDefaultSubobject<USceneComponent>(TEXT("AnchorTwo"));
	Anchor0->SetupAttachment(RootComponent);
	Anchor1->SetupAttachment(RootComponent);
}

void AC_MovableShootingTarget::BeginPlay()
{
	Super::BeginPlay();

	LeftLocation  = Anchor0->GetComponentLocation();
	RightLocation = Anchor1->GetComponentLocation();
}

void AC_MovableShootingTarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ElapsedTime += DeltaTime;
	
	float Alpha = (FMath::Sin(ElapsedTime * MoveSpeed) + 1.0f) * 0.5f;

	FVector NewLocation = FMath::Lerp(LeftLocation, RightLocation, Alpha);
	this->SetActorLocation(NewLocation);
}
