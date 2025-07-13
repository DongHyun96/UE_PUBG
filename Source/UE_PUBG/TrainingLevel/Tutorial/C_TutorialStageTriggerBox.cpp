// Fill out your copyright notice in the Description page of Project Settings.


#include "C_TutorialStageTriggerBox.h"

#include "Blueprint/UserWidget.h"
#include "Character/C_Player.h"
#include "Components/ShapeComponent.h"
#include "Components/WidgetComponent.h"
#include "Utility/C_Util.h"


AC_TutorialStageTriggerBox::AC_TutorialStageTriggerBox()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AC_TutorialStageTriggerBox::BeginPlay()
{
	Super::BeginPlay();

	TriangleWidgetComp = Cast<UWidgetComponent>(GetDefaultSubobjectByName("TriangleWidgetComponent"));
	if (!TriangleWidgetComp)
	{
		UC_Util::Print("From AC_TutorialStageTriggerBox::BeginPlay : Cannot find TriangleWidgetComponent", FColor::MakeRandomColor(), 10.f);
		return;
	}

	TriangleWidgetComp->SetHiddenInGame(true);

	this->OnActorBeginOverlap.AddDynamic(this, &AC_TutorialStageTriggerBox::OnTriggerBoxBeginOverlap);
}

void AC_TutorialStageTriggerBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!TriangleWidgetComp->bHiddenInGame)
	{
		FRotator Rotation = TriangleWidgetComp->GetRelativeRotation();
		Rotation.Yaw += 120.f * DeltaTime;
		TriangleWidgetComp->SetRelativeRotation(Rotation);
	}
}

void AC_TutorialStageTriggerBox::ToggleTriggerBox(bool InIsEnabled)
{
	if (InIsEnabled)
	{
		GetCollisionComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		if (IsValid(TriangleWidgetComp)) TriangleWidgetComp->SetHiddenInGame(false);
	}
	else
	{
		GetCollisionComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if (IsValid(TriangleWidgetComp)) TriangleWidgetComp->SetHiddenInGame(true);
	}
}

void OnTriggerBoxBeginOverlap
(
	UPrimitiveComponent*	OverlappedComponent,
	AActor*					OtherActor,
	UPrimitiveComponent*	OtherComp,
	int32					OtherBodyIndex,
	bool					bFromSweep,
	const FHitResult&		SweepResult
)
{
	
}

void AC_TutorialStageTriggerBox::OnTriggerBoxBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	// Overlapped된 Actor가 Player가 아닌 경우
	if (!Cast<AC_Player>(OtherActor)) return;

	ToggleTriggerBox(false);

	if (AreaArrivedDelegate.IsBound()) AreaArrivedDelegate.Execute(DelegateParam.Key, DelegateParam.Value);
}
