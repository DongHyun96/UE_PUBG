// Fill out your copyright notice in the Description page of Project Settings.


#include "C_ShootingTargetWidgetsHolder.h"

#include "Components/WidgetComponent.h"
#include "Utility/C_Util.h"

AC_ShootingTargetWidgetsHolder::AC_ShootingTargetWidgetsHolder()
{
	PrimaryActorTick.bCanEverTick = true;

	for (int i = 0; i < WidgetPoolCount; ++i)
	{
		FString WidgetStr = "DamageWidget" + FString::FromInt(i);
		FName WidgetName(*WidgetStr);		
		UWidgetComponent* DamageWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(WidgetName);
		DamageWidgetComponents.Add(DamageWidgetComponent);
	}
}

void AC_ShootingTargetWidgetsHolder::BeginPlay()
{
	Super::BeginPlay();

	// Init Queue
	DamageWidgetQueue.Empty();
	for (UWidgetComponent* Widget : DamageWidgetComponents)
		DamageWidgetQueue.Enqueue(Widget);
}

void AC_ShootingTargetWidgetsHolder::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AC_ShootingTargetWidgetsHolder::SpawnDamageInfoWidget(bool bIsHeadShot, float DamageAmount, const FVector& WorldLocation)
{
	
}