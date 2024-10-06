// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/C_SwimmingComponent.h"

#include "Components/ShapeComponent.h"
#include "Components/CapsuleComponent.h"

#include "Character/C_BasicCharacter.h"

#include "Utility/C_Util.h"

#include "WaterBodyActor.h"

UC_SwimmingComponent::UC_SwimmingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	WaterDetectionCollider = CreateDefaultSubobject<UCapsuleComponent>("WaterDetectionCollider");
	//WaterDetectionCollider->SetupAttachment(OwnerCharacter->GetRootComponent());
}


void UC_SwimmingComponent::BeginPlay()
{
	Super::BeginPlay();

	WaterDetectionCollider->OnComponentBeginOverlap.AddDynamic(this, &UC_SwimmingComponent::OnWaterDetectionColliderBeginOverlap);
	WaterDetectionCollider->OnComponentEndOverlap.AddDynamic  (this, &UC_SwimmingComponent::OnWaterDetectionColliderEndOverlap);
}


void UC_SwimmingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UC_SwimmingComponent::SetOwnerCharacter(AC_BasicCharacter* InOwnerCharacter)
{
	OwnerCharacter = InOwnerCharacter;
	WaterDetectionCollider->AttachToComponent(OwnerCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
}

void UC_SwimmingComponent::OnWaterDetectionColliderBeginOverlap
(
	UPrimitiveComponent*	OverlappedComponent,
	AActor*					OtherActor,
	UPrimitiveComponent*	OtherComp,
	int32					OtherBodyIndex, 
	bool					bFromSweep,
	const FHitResult&		SweepResult
)
{
	AWaterBody* WaterBody = Cast<AWaterBody>(OtherActor);
	if (IsValid(WaterBody)) UC_Util::Print("On Water Begin Overlap", FColor::Red);
}

void UC_SwimmingComponent::OnWaterDetectionColliderEndOverlap
(
	UPrimitiveComponent*	OverlappedComponent, 
	AActor*					OtherActor,
	UPrimitiveComponent*	OtherComp,
	int32					OtherBodyIndex
)
{
	AWaterBody* WaterBody = Cast<AWaterBody>(OtherActor);
	if (IsValid(WaterBody)) UC_Util::Print("On Water Begin End Overlap", FColor::Red);
}

