// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/C_ConsumableUsageMeshComponent.h"

#include "Character/C_BasicCharacter.h"

#include "Utility/C_Util.h"

UC_ConsumableUsageMeshComponent::UC_ConsumableUsageMeshComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	//for (uint8 poseState = 0; poseState < static_cast<uint8>(EPoseState::POSE_MAX); poseState++)
	//{
	//	EPoseState currentPose = static_cast<EPoseState>(poseState);

	for (uint8 UsageMeshType = 0; UsageMeshType < static_cast<uint8>(EConsumableUsageMeshType::MAX); UsageMeshType++)
	{
		EConsumableUsageMeshType MeshType = static_cast<EConsumableUsageMeshType>(UsageMeshType);

		FName StaticMeshName = FName(GetMeshTypeName(MeshType) + "StaticMeshComp");

		UStaticMeshComponent* StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(StaticMeshName);
		UsageMeshMap.Emplace(MeshType, StaticMeshComponent);
		StaticMeshComponent->SetComponentTickEnabled(true);
		//StaticMeshComponent->PrimaryComponentTick.bCanEverTick = true;
	}
}


void UC_ConsumableUsageMeshComponent::BeginPlay()
{
	Super::BeginPlay();

	for (auto& Pair : UsageMeshMap)
	{
		//Pair.Value->SetVisibility(false);
		//Pair.Value->SetHiddenInGame(true);

		Pair.Value->AttachToComponent
		(
			OwnerCharacter->GetMesh(),
			FAttachmentTransformRules(EAttachmentRule::KeepRelative, true),
			SocketNames[Pair.Key]
		);
		Pair.Value->SetVisibility(false);
	}
}

// Called every frame
void UC_ConsumableUsageMeshComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UC_ConsumableUsageMeshComponent::ToggleMeshUsageVisible(EConsumableUsageMeshType MeshType, bool Visible)
{
	if (MeshType == EConsumableUsageMeshType::MAX)
	{
		UC_Util::Print("From UC_ConsumableUsageMeshComponent::ToggleMeshUsageVisible : Wrong MeshType");
		return;
	}
	
	//UsageMeshMap[MeshType]->bHiddenInGame = !Visible;
	UsageMeshMap[MeshType]->SetVisibility(Visible);
	//UsageMeshMap[MeshType]->SetHiddenInGame(!Visible);
}

FString UC_ConsumableUsageMeshComponent::GetMeshTypeName(EConsumableUsageMeshType Type)
{
	switch (Type)
	{
		case EConsumableUsageMeshType::BANDAGE:			return "Bandage";
		case EConsumableUsageMeshType::SYRINGE:			return "Syringe";
		case EConsumableUsageMeshType::DRINK:			return "Drink";
		case EConsumableUsageMeshType::PAIN_KILLER:		return "PainKiller";
		case EConsumableUsageMeshType::MAX: default:	return "";
	}
}

