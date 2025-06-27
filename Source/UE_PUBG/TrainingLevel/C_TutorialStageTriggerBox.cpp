// Fill out your copyright notice in the Description page of Project Settings.


#include "C_TutorialStageTriggerBox.h"

#include "Blueprint/UserWidget.h"
#include "Components/ShapeComponent.h"


AC_TutorialStageTriggerBox::AC_TutorialStageTriggerBox()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AC_TutorialStageTriggerBox::BeginPlay()
{
	Super::BeginPlay();
	
}

void AC_TutorialStageTriggerBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AC_TutorialStageTriggerBox::ToggleTriggerBox(bool InIsEnabled)
{
	if (InIsEnabled)
	{
		GetCollisionComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		if (IsValid(TriangleWidget)) TriangleWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		GetCollisionComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if (IsValid(TriangleWidget)) TriangleWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}
