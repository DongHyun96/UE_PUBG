// Fill out your copyright notice in the Description page of Project Settings.


#include "C_ShootingTargetWidgetsHolder.h"

#include "C_ShootingTargetDamageInfoWidget.h"
#include "Components/WidgetComponent.h"
#include "EntitySystem/MovieSceneEntitySystemRunner.h"
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

void AC_ShootingTargetWidgetsHolder::SpawnDamageInfoWidget(EShootingTargetDamageInfoType DamageInfoType, float DamageAmount, const FVector& WorldLocation)
{
	// Queue 처리
	UWidgetComponent* SpawnedWidgetComponent{};
	if (!DamageWidgetQueue.Dequeue(SpawnedWidgetComponent))
	{
		UC_Util::Print("From AC_ShootingTargetWidgetsHolder::SpawnDamageInfoWidget : DamageWidgetQueue Dequeue failed!", FColor::Red, 10.f);
		return;
	}
	DamageWidgetQueue.Enqueue(SpawnedWidgetComponent);

	SpawnedWidgetComponent->SetWorldLocation(WorldLocation);
	
	UC_ShootingTargetDamageInfoWidget* DamageInfoWidget = Cast<UC_ShootingTargetDamageInfoWidget>(SpawnedWidgetComponent->GetWidget());
	if (!DamageInfoWidget)
	{
		UC_Util::Print("From AC_ShootingTargetWidgetsHolder::SpawnDamageInfoWidget : DamageInfoWidget casting failed!", FColor::Red, 10.f);
		return;
	}
	DamageInfoWidget->Spawn(DamageInfoType, DamageAmount);
}
