// Fill out your copyright notice in the Description page of Project Settings.


#include "C_ItemRespawnHandler.h"

#include "Components/BoxComponent.h"
#include "Item/C_Item.h"
#include "Singleton/C_GameSceneManager.h"
#include "Utility/C_Util.h"

const float AC_ItemRespawnHandler::RespawnDelayMax = 5.f;

AC_ItemRespawnHandler::AC_ItemRespawnHandler()
{
	PrimaryActorTick.bCanEverTick = true;

	SpawnArea = CreateDefaultSubobject<UBoxComponent>("SpawnArea");
	RootComponent = SpawnArea;
}

void AC_ItemRespawnHandler::BeginPlay()
{
	Super::BeginPlay();

	// RespawnArea에 잡힌 아이템들 조사 & 첫 setting
	InitialTransforms.Empty();
	SpawnedItems.Empty();

	// 아직 모든 Collision이 완전히 업데이트 되기 전 호출이 될 수 있음 -> 실제로 개수가 안맞았음
	// 특히 레벨에 배치된 액터들끼리의 Overlap은, 월드에서 모든 액터의 컴포넌트가 등록되고 난 후에야 안정적으로 처리됨
	// 따라서 살짝의 Latent로 밀어줌

	FTimerHandle& TimerHandle = GAMESCENE_MANAGER->GetTimerHandle();
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
	{
		TArray<AActor*> OverlappingItems{};
		
		SpawnArea->GetOverlappingActors(OverlappingItems, AC_Item::StaticClass());

		// UC_Util::Print("Total Overlapped Items : " + FString::FromInt(OverlappingItems.Num()), FColor::Red, 10.f);

		for (AActor* OverlappingItem : OverlappingItems)
		{
			AC_Item* Item = Cast<AC_Item>(OverlappingItem);
			if (!Item) continue;
			
			// 파밍 시에 실행할 Delegate bind 처리
			Item->OnRespawnableItemPickedUp.BindUObject(this, &AC_ItemRespawnHandler::OnItemPickedUp);
			
			if (!InitialTransforms.Contains(Item->GetClass())) // 첫 Class detected
			{
				InitialTransforms.Add(Item->GetClass(), {Item->GetActorTransform()});
				SpawnedItems.Add(Item->GetClass(), {Item});
				continue;
			}

			InitialTransforms[Item->GetClass()].Add(Item->GetActorTransform());
			SpawnedItems[Item->GetClass()].Add(Item);
		}

		// 첫 조사 이 후, SpawnArea collision 끄기
		SpawnArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// For testing
		/*for (const TPair<TSubclassOf<AC_Item>, TArray<FTransform>>& Pair : InitialTransforms)
		{
			FString Str = Pair.Key->GetName() + "'s Transform count : " + FString::FromInt(Pair.Value.Num());
			UC_Util::Print(Str, FColor::Blue, 30.f);
		}

		for (const TPair<TSubclassOf<AC_Item>, TArray<AC_Item*>>& Pair : SpawnedItems)
		{
			FString Str = Pair.Key->GetName() + "'s object count : " + FString::FromInt(Pair.Value.Num());
			UC_Util::Print(Str, FColor::Red, 30.f);
		}*/
		
	}, 0.1f, false);
}

void AC_ItemRespawnHandler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bRespawnTimerSet) return;
	RespawnTimer += DeltaTime;

	if (RespawnTimer < RespawnDelayMax) return;

	// Time to handle respawn items
	for (TPair<TSubclassOf<AC_Item>, TArray<AC_Item*>>& Pair : SpawnedItems)
	{
		for (int i = 0; i < Pair.Value.Num(); ++i)
		{
			if (Pair.Value[i])
				continue; // 해당 자리에 이미 아이템이 존재함

			// 해당 자리에 아이템이 존재하지 않음 (초기 아이템 Transform을 적용하여 Spawn 처리)
			AC_Item* NewItem = GetWorld()->SpawnActor<AC_Item>(Pair.Key, InitialTransforms[Pair.Key][i]);
			Pair.Value[i] = NewItem;

			// Bind Delegate
			NewItem->OnRespawnableItemPickedUp.BindUObject(this, &AC_ItemRespawnHandler::OnItemPickedUp);
		}
	}
	
	bRespawnTimerSet = false;
	RespawnTimer = 0.f;
}

void AC_ItemRespawnHandler::OnItemPickedUp(AC_Item* PickedItem)
{
	UC_Util::Print("OnItemPickedUp", FColor::Red, 10.f);

	PickedItem->OnRespawnableItemPickedUp.Unbind();

	if (!SpawnedItems.Contains(PickedItem->GetClass()))
	{
		UC_Util::Print
		(
			"From AC_ItemRespawnHandler::OnItemPickedUp : Delegate called but the item is not managed by this ItemRespawnHandler!",
			FColor::MakeRandomColor(), 10.f
		);
		return;
	}

	// Set respawn timer
	bRespawnTimerSet = true;
	RespawnTimer = 0.f;
	
	// SpawnedItem에서 주운 Item 빼기 (해당 위치(ActorLocation X) index nullptr로 초기화)
	for (AC_Item*& Item : SpawnedItems[PickedItem->GetClass()])
	{
		if (Item == PickedItem)
		{
			Item = nullptr;
			break;
		}
	}
	
}
