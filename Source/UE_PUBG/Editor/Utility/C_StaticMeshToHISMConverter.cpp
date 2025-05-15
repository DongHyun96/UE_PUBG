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

	// HISM들을 붙일 새로운 Actor 생성
	AActor* InstancingActor = World->SpawnActor<AActor>();
	if (!InstancingActor) return;

	// 원하는 이름 부여 (에디터에서 찾기 쉽게)
	InstancingActor->Rename(TEXT("HISM_Container"));

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
			// HISM 생성 및 설정
			UHierarchicalInstancedStaticMeshComponent* HISM =
				NewObject<UHierarchicalInstancedStaticMeshComponent>(InstancingActor);

			HISM->RegisterComponent();
			HISM->SetStaticMesh(Mesh);
			HISM->AttachToComponent(InstancingActor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

			MeshToHISMMap.Add(Mesh, HISM);
		}

		MeshToHISMMap[Mesh]->AddInstance(SMC->GetComponentTransform());

		// 원본 액터 제거
		SMActor->Destroy();
	}
}
