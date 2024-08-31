// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/C_HUDComponent.h"

//#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"

// Sets default values for this component's properties
UC_HUDComponent::UC_HUDComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UC_HUDComponent::BeginPlay()
{
	Super::BeginPlay();

	//HUDWidgetComponent->GetWidget()
	
	if (HUDWidget) HUDWidget->AddToViewport();
}


// Called every frame
void UC_HUDComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

