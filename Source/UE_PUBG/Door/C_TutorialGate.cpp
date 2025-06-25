// Fill out your copyright notice in the Description page of Project Settings.


#include "C_TutorialGate.h"

#include "Character/C_Player.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/TriggerBox.h"
#include "Utility/C_Util.h"


AC_TutorialGate::AC_TutorialGate()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AC_TutorialGate::BeginPlay()
{
	Super::BeginPlay();

	GateOpeningBox = Cast<UBoxComponent>(GetDefaultSubobjectByName("OpeningCollisionBox"));

	if (!GateOpeningBox) UC_Util::Print("From AC_TutorialGate::BeginPlay : Cannot find OpeningCollisionBox!", FColor::Red, 10.f);
	else				 GateOpeningBox->OnComponentBeginOverlap.AddDynamic(this, &AC_TutorialGate::OnGateOpeningBoxBeginOverlap);

	TriangleWidgetComponent = Cast<UWidgetComponent>(GetDefaultSubobjectByName("TriangleWidget"));

	if (!TriangleWidgetComponent) UC_Util::Print("From AC_TutorialGate::BeginPlay : Cannot find TriangleWidget!", FColor::Red, 10.f);

	ToggleOpeningBoxTriggerEnabled(false);
}

void AC_TutorialGate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!TriangleWidgetComponent->bHiddenInGame)
	{
		// TriangleWidgetComponent의 Z축 기준 계속 회전 시키기
		static const float RotationSpeed = 45.f;

		FRotator Rotation = TriangleWidgetComponent->GetRelativeRotation();
		Rotation.Yaw += 120.f * DeltaTime;
		TriangleWidgetComponent->SetRelativeRotation(Rotation);
	}
}

void AC_TutorialGate::ToggleOpeningBoxTriggerEnabled(bool Enabled)
{
	TriangleWidgetComponent->SetHiddenInGame(!Enabled);
	GateOpeningBox->SetCollisionEnabled(Enabled ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
}

void AC_TutorialGate::OnGateOpeningBoxBeginOverlap
(
	UPrimitiveComponent*	OverlappedComp,
	AActor*					OtherActor,
	UPrimitiveComponent*	OtherComp,
	int32					OtherBodyIndex,
	bool					bFromSweep,
	const FHitResult&		SweepResult
)
{
	if (!Cast<AC_Player>(OtherActor)) return;
	
	// Player entered
	OpenGate();
}
