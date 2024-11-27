// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/C_ParkourComponent.h"

#include "Character/C_BasicCharacter.h"

#include "Utility/C_Util.h"

UC_ParkourComponent::UC_ParkourComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UC_ParkourComponent::BeginPlay()
{
	Super::BeginPlay();
	if (!RootedSkeletalMesh || !RootedAnimInstance)
	{
		UC_Util::Print("From UC_ParkourComponent : Var not inited!", FColor::Red, 10.f);
		return;
	}

	FTransform PrevTransform = OwnerCharacter->GetMesh()->GetRelativeTransform();

	OwnerCharacter->GetMesh()->SetSkeletalMesh(RootedSkeletalMesh, false);
	OwnerCharacter->GetMesh()->SetAnimInstanceClass(RootedAnimInstance);
	OwnerCharacter->GetMesh()->SetRelativeTransform(PrevTransform);
}


void UC_ParkourComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

