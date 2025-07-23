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

			// 충돌 세부 설정
			NewBoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			NewBoxComponent->SetCollisionObjectType(InitialOuterBoxObjectTypeChannel);
			NewBoxComponent->SetCollisionResponseToAllChannels(ECR_Overlap);
			NewBoxComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
			NewBoxComponent->SetCollisionResponseToChannel(ECC_WorldStatic,  ECR_Ignore);

			for (TEnumAsByte<ECollisionChannel> TraceCollisionChannel : AllTraceChannelUsed)
				NewBoxComponent->SetCollisionResponseToChannel(TraceCollisionChannel, ECR_Ignore);

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
	RespawnTimer = 0.f;
	
	// Respawn Timer End, Respawn 가능한 상황인지 체크
	
	TArray<uint8> PossibleSpaceIndices{};
	GetPossibleSpaceIndicesForRespawn(PossibleSpaceIndices);

	if (CurrentCarCount >= InitialCarCount)
	{
		UC_Util::Print("RespawnTimer End and tried respawn, but CurrentCar Count satisfied initial carCount or more.", FColor::Red, 10.f);
		bRespawnTimerSet = false;
		return;
	}

	// 가능한 자리에 한해서 Respawn 처리
	uint8 CarCount = CurrentCarCount;
	for (uint8 PossibleSpaceIndex : PossibleSpaceIndices)
	{
		GetWorld()->SpawnActor<AC_Vehicle>(VehicleClass, InitialTransforms[PossibleSpaceIndex]);
		// CurrentCarCount를 여기서 증가시키지 않음 -> Spawn처리되면서 자연스럽게 BeginOverlap 호출에서 차 개수가 더해짐
		if (++CarCount >= InitialCarCount) break;
	}

	// Spawn 처리한 이후, 차량의 개수가 아직 초기 개수와 맞지 않다면 기본 Timer보다 더 짧은 Timer를 두어 Spawn 시도
	if (CarCount < InitialCarCount)
	{
		// RespawnTimer = 10.f;
		UC_Util::Print("Tried respawn, but some Parking lot not vacant", FColor::Red, 10.f);
		UC_Util::Print(CarCount, FColor::MakeRandomColor(), 10.f);
		RespawnTimer = 3.f;
		return;
	}

	// 첫 차량 Count 대수 만큼 Spawn처리가 끝난 상황 / Timer 해제 
	bRespawnTimerSet = false;
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

	UC_Util::Print("OnSpawnAreaBeginOverlap", FColor::MakeRandomColor(), 10.f);
	
	if (++CurrentCarCount >= InitialCarCount)
	{
		// RespawnTimer 끄기
		bRespawnTimerSet = false;
		RespawnTimer = 0.f; 
	}
	FString Str = "Current Car Count : " + FString::FromInt(CurrentCarCount);
	UC_Util::Print(Str, FColor::MakeRandomColor(), 10.f);
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

	UC_Util::Print("OnSpawnAreaEndOverlap", FColor::MakeRandomColor(), 10.f);

	// 차고지에 차량 전체 대수가 부족해진 상황
	if (--CurrentCarCount < InitialCarCount)
	{
		// 현재 Timer가 Setting되어있지 않았던 상황 RespawnTimer 최대로 맞춰놓기
		// if (!bRespawnTimerSet) RespawnTimer = RespawnDelayMax;
		if (!bRespawnTimerSet) RespawnTimer = 5.f;
		bRespawnTimerSet = true;
	}
	FString Str = "Current Car Count : " + FString::FromInt(CurrentCarCount);
	UC_Util::Print(Str, FColor::MakeRandomColor(), 10.f);
}

void AC_CarRespawnHandler::GetPossibleSpaceIndicesForRespawn(TArray<uint8>& PossibleIndices)
{
	PossibleIndices.Empty();

	for (int i = 0; i < InitialCarOuterBoxes.Num(); ++i)
	{
		TArray<AActor*> OverlappingActors{};
		InitialCarOuterBoxes[i]->GetOverlappingActors(OverlappingActors);

		if (OverlappingActors.IsEmpty()) PossibleIndices.Add(i);
	}
}
