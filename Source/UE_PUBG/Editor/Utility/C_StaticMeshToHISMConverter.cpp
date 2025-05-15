// Fill out your copyright notice in the Description page of Project Settings.


#include "Editor/Utility/C_StaticMeshToHISMConverter.h"
#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"


void UC_StaticMeshToHISMConverter::ConvertToHISM()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AStaticMeshActor::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		AStaticMeshActor* SMActor = Cast<AStaticMeshActor>(Actor);
		if (!SMActor) continue;

		UStaticMeshComponent* SMC = SMActor->GetStaticMeshComponent();
		if (!SMC || !SMC->GetStaticMesh()) continue;

		UStaticMesh* Mesh = SMC->GetStaticMesh();

		if (!MeshToHISMMap.Contains(Mesh))
		{
			auto* HISM = NewObject<UHierarchicalInstancedStaticMeshComponent>(this);
			HISM->RegisterComponent();
			HISM->SetStaticMesh(Mesh);
			HISM->AttachToComponent(SMActor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
			MeshToHISMMap.Add(Mesh, HISM);
		}

		MeshToHISMMap[Mesh]->AddInstance(SMC->GetComponentTransform());
		SMActor->Destroy();
	}
}
