// Fill out your copyright notice in the Description page of Project Settings.


#include "Editor/Utility/C_StaticMeshToHISMConverter.h"
#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"


void UC_StaticMeshToHISMConverter::ConvertToHISM()
{
	UWorld* World = GetWorld();
	if (!World) return;

	// 새로운 Actor 생성 및 RootComponent 설정
	AActor* InstancingActor = World->SpawnActor<AActor>();
	if (!InstancingActor) return;

	InstancingActor->Rename(TEXT("HISM_Container"));
	USceneComponent* RootComp = NewObject<USceneComponent>(InstancingActor, TEXT("Root"));
	InstancingActor->SetRootComponent(RootComp);
	RootComp->RegisterComponent();
	InstancingActor->SetActorLocation(FVector::ZeroVector); // 위치 지정

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(World, AStaticMeshActor::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		AStaticMeshActor* SMActor = Cast<AStaticMeshActor>(Actor);
		if (!SMActor) continue;

		UStaticMeshComponent* SMC = SMActor->GetStaticMeshComponent();
		if (!SMC || !SMC->GetStaticMesh()) continue;

		UStaticMesh* Mesh = SMC->GetStaticMesh();

		if (!MeshToHISMMap.Contains(Mesh))
		{
			UHierarchicalInstancedStaticMeshComponent* HISM =
				NewObject<UHierarchicalInstancedStaticMeshComponent>(InstancingActor);

			HISM->RegisterComponent();
			HISM->SetStaticMesh(Mesh);
			HISM->AttachToComponent(RootComp, FAttachmentTransformRules::KeepRelativeTransform);
			MeshToHISMMap.Add(Mesh, HISM);
		}

		MeshToHISMMap[Mesh]->AddInstance(SMC->GetComponentTransform());

		SMActor->Destroy();
	}
}
