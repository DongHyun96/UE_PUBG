// Fill out your copyright notice in the Description page of Project Settings.


#include "C_CarRespawnHandler.h"

#include "Components/BoxComponent.h"
#include "Singleton/C_GameSceneManager.h"
#include "Utility/C_Util.h"
#include "Vehicle/C_Vehicle.h"

const float AC_CarRespawnHandler::RespawnDelayMax = 30.f;

AC_CarRespawnHandler::AC_CarRespawnHandler()
{
	PrimaryActorTick.bCanEverTick = true;

	SpawnArea = CreateDefaultSubobject<UBoxComponent>("SpawnArea");
	RootComponent = SpawnArea;
}

void AC_CarRespawnHandler::BeginPlay()
{
	Super::BeginPlay();

	SpawnArea->OnComponentBeginOverlap.AddDynamic(this, &AC_CarRespawnHandler::OnSpawnAreaBeginOverlap);
	SpawnArea->OnComponentEndOverlap.AddDynamic(this, &AC_CarRespawnHandler::OnSpawnAreaEndOverlap);

	FTimerHandle& TimerHandle = GAMESCENE_MANAGER->GetTimerHandle();
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
	{
		TArray<AActor*> OverlappingVehicles{};

		SpawnArea->GetOverlappingActors(OverlappingVehicles, AC_Vehicle::StaticClass());

		for (AActor* OverlappingVehicle : OverlappingVehicles)
		{
			AC_Vehicle* Vehicle = Cast<AC_Vehicle>(OverlappingVehicle);
			if (!Vehicle) continue;

			++InitialCarCount; ++CurrentCarCount;

			InitialTransforms.Add(Vehicle->GetActorTransform());

			// Vehicle OuterBox와 같은 크기 및 위치의 새로운 BoxComponent를 생성 -> 추후 Respawn처리 시, 해당 영역에 물체가 없는지 조사할 때 사용
			UBoxComponent* OriginBoxComponent = Vehicle->GetMainOuterBoxComponent();
			UBoxComponent* NewBoxComponent = NewObject<UBoxComponent>(this);

			if (!NewBoxComponent || !OriginBoxComponent)
			{
				UC_Util::Print("From AC_CarRespawnHandler::BeginPlay : NewBox or OriginBox not valid!", FColor::Red, 10.f);
				continue;
			}

			NewBoxComponent->RegisterComponent();
			NewBoxComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);

			// Box Extent 동일히 Setting
			NewBoxComponent->SetBoxExtent(OriginBoxComponent->GetUnscaledBoxExtent());
			NewBoxComponent->SetWorldTransform(OriginBoxComponent->GetComponentTransform());

			// 충돌 설정은 모든 Actor Overlapped 되게끔 설정
			NewBoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			NewBoxComponent->SetCollisionObjectType(OriginBoxComponent->GetCollisionObjectType());
			NewBoxComponent->SetCollisionResponseToAllChannels(ECR_Overlap);

			NewBoxComponent->SetHiddenInGame(false);

			InitialCarOuterBoxes.Add(NewBoxComponent);
		}

		bInitializedCompletedFlag = true;
	}, 0.1f, false);
}

void AC_CarRespawnHandler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bRespawnTimerSet) return;
	RespawnTimer -= DeltaTime;

	if (RespawnTimer > 0.f) return;

	// Respawn Timer는 모두 돈 상황, Respawn 가능한 상황인지 체크
	// if (CurrentCarCount < InitialCarCount && )
	
}

void AC_CarRespawnHandler::OnSpawnAreaBeginOverlap
(
	UPrimitiveComponent*	OverlappedComponent,
	AActor*					OtherActor,
	UPrimitiveComponent*	OtherComp,
	int32					OtherBodyIndex,
	bool					bFromSweep,
	const FHitResult&		SweepResult
)
{
	if (!bInitializedCompletedFlag) return;
	AC_Vehicle* EnteredVehicle = Cast<AC_Vehicle>(OtherActor);
	if (!EnteredVehicle) return;

	++CurrentCarCount;

	if (++CurrentCarCount >= InitialCarCount)
	{
		// RespawnTimer 끄기
		bRespawnTimerSet = false;
		RespawnTimer = 0.f; 
	}
}

void AC_CarRespawnHandler::OnSpawnAreaEndOverlap
(
	UPrimitiveComponent* OverlappedComponent,
	AActor*				 OtherActor,
	UPrimitiveComponent* OtherComp,
	int32				 OtherBodyIndex
)
{
	if (!bInitializedCompletedFlag) return;

	AC_Vehicle* LeftVehicle = Cast<AC_Vehicle>(OtherActor);
	if (!LeftVehicle) return;

	// 새로이 Spawn 처리된 차량이 나갔으면, 해당 Spawn 자리 비워두기 
	// 자리를 비워둔다고 무조건 Respawn 처리되는 것은 아님
	for (int i = 0; i < Vehicles.Num(); ++i)
	{
		if (LeftVehicle == Vehicles[i])
		{
			Vehicles[i] = nullptr;
			break;
		}
	}

	// 차고지에 차량 전체 대수가 부족해진 상황
	if (--CurrentCarCount < InitialCarCount)
	{
		// 현재 Timer가 Setting되어있지 않았던 상황 RespawnTimer 최대로 맞춰놓기
		if (!bRespawnTimerSet) RespawnTimer = RespawnDelayMax;
		bRespawnTimerSet = true;
	}
}
