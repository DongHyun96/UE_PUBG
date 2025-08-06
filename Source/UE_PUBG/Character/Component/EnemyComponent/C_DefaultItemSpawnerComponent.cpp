// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/EnemyComponent/C_DefaultItemSpawnerComponent.h"

#include "Character/C_Enemy.h"
#include "Character/Component/C_EquippedComponent.h"
#include "Item/ConsumableItem/C_ConsumableItem.h"
#include "Item/Equipment/C_BackPack.h"
#include "Item/Equipment/C_Helmet.h"
#include "Item/Equipment/C_Vest.h"
#include "Item/ItemManager/C_ItemManager.h"
#include "Item/Weapon/MeleeWeapon/C_MeleeWeapon.h"
#include "Item/Weapon/C_Weapon.h"
#include "Item/Weapon/Gun/C_Gun.h"
#include "Item/Weapon/ThrowingWeapon/C_ThrowingWeapon.h"
#include "Singleton/C_GameInstance.h"
#include "Singleton/C_GameSceneManager.h"
#include "Utility/C_Util.h"

UC_DefaultItemSpawnerComponent::UC_DefaultItemSpawnerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	// /Game/Project_PUBG/Common/Item/Weapon/Gun/AR/BPC_AUG.uasset
}

void UC_DefaultItemSpawnerComponent::BeginPlay()
{
	Super::BeginPlay();

	// MovementTest와 SkyDivingTest 용 Enemy의 경우, Default Item들을 Spawn시키지 않음
	switch (OwnerEnemy->GetBehaviorType()) case EEnemyBehaviorType::MovementTest: case EEnemyBehaviorType::SkyDivingTest:
		return;
	
	SpawnDefaultWeaponsAndItems();

	if (GAMESCENE_MANAGER->GetItemManager())
		ToggleSpawnedItemsHiddenInGame(GAMESCENE_MANAGER->GetItemManager()->GetEnemyDefaultSpawnedItemHiddenInGame());
	else
		ToggleSpawnedItemsHiddenInGame(false);
}

void UC_DefaultItemSpawnerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UC_DefaultItemSpawnerComponent::SpawnDefaultWeaponsAndItems()
{
	switch (OwnerEnemy->GetBehaviorType()) { case EEnemyBehaviorType::MovementTest: case EEnemyBehaviorType::SkyDivingTest: return; }
	
	FActorSpawnParameters Param{};
	Param.Owner = OwnerEnemy;

	SpawnEquipableItems(Param);
	SpawnWeapons(Param);
	SpawnConsumableItems(Param);
	SpawnBullets(Param);
}

void UC_DefaultItemSpawnerComponent::ToggleSpawnedItemsHiddenInGame(bool InHiddenInGame)
{
	TArray<AActor*> AttachedItems{};
	OwnerEnemy->GetAttachedActors(AttachedItems);

	for (AActor* AttachedItem : AttachedItems) AttachedItem->SetActorHiddenInGame(InHiddenInGame);
}

void UC_DefaultItemSpawnerComponent::SpawnWeapons(const FActorSpawnParameters& Param)
{
	switch (OwnerEnemy->GetBehaviorType()) 
	{ case EEnemyBehaviorType::MovementTest: case EEnemyBehaviorType::SkyDivingTest: return; }
	
	if (WeaponClasses.IsEmpty())
	{
		UC_Util::Print("WeaponClasses empty!", FColor::Red, 10.f);
		return;
	}

	if (OwnerEnemy->GetBehaviorType() == EEnemyBehaviorType::StatCareTest)
	{
		// Smoke Grenade 3개 (이 개수가 ThrowingWeapon내의 MoveInvenToSlot에서 예외처리를 하기 간단해짐)
		AC_ThrowingWeapon* SmokeGrenade = GetWorld()->SpawnActor<AC_ThrowingWeapon>(ThrowableClasses[EThrowableType::SMOKE], Param);
		SmokeGrenade->SetItemStack(3);
		SmokeGrenade->MoveToInven(OwnerEnemy, SmokeGrenade->GetItemCurStack());
		return;
	}
	
	AC_MeleeWeapon* MeleeWeapon = GetWorld()->SpawnActor<AC_MeleeWeapon>(WeaponClasses[EWeaponSlot::MELEE_WEAPON], Param);
	OwnerEnemy->GetEquippedComponent()->SetSlotWeapon(EWeaponSlot::MELEE_WEAPON, MeleeWeapon);

	AC_Gun* MainGun = GetWorld()->SpawnActor<AC_Gun>(WeaponClasses[EWeaponSlot::MAIN_GUN], Param);
	MainGun->MoveToSlot(OwnerEnemy, MainGun->GetItemCurStack());
	MainGun->SetActorHiddenInGame(false); // 어디선가 Hidden처리가 되어 나옴
	MainGun->SetCurMagazineBulletCount(30);
	//EquippedComponent->SetSlotWeapon(EWeaponSlot::MAIN_GUN, MainGun);

	AC_Gun* SubGun = GetWorld()->SpawnActor<AC_Gun>(WeaponClasses[EWeaponSlot::SUB_GUN], Param);
	SubGun->MoveToSlot(OwnerEnemy, SubGun->GetItemCurStack());
	SubGun->SetActorHiddenInGame(false);
	SubGun->SetCurMagazineBulletCount(5);
	//EquippedComponent->SetSlotWeapon(EWeaponSlot::SUB_GUN, SubGun);

	// Throwable Weapon setting 하기

	// CombatTestType의 경우, 갯수 1개로 setting
	bool bIsCombatTestType = OwnerEnemy->GetBehaviorType() == EEnemyBehaviorType::CombatTest; 

	// Grenade 1~3개
	AC_ThrowingWeapon* Grenade = GetWorld()->SpawnActor<AC_ThrowingWeapon>(ThrowableClasses[EThrowableType::GRENADE], Param);
	Grenade->SetItemStack(bIsCombatTestType ? 1 : FMath::RandRange(1, 2));
	Grenade->MoveToInven(OwnerEnemy, Grenade->GetItemCurStack());

	// Smoke Grenade 1~2개
	AC_ThrowingWeapon* SmokeGrenade = GetWorld()->SpawnActor<AC_ThrowingWeapon>(ThrowableClasses[EThrowableType::SMOKE], Param);
	SmokeGrenade->SetItemStack(bIsCombatTestType ? 1 : FMath::RandRange(1, 2));
	SmokeGrenade->MoveToInven(OwnerEnemy, SmokeGrenade->GetItemCurStack());

	// FlashBang 1~2개
	AC_ThrowingWeapon* FlashBang = GetWorld()->SpawnActor<AC_ThrowingWeapon>(ThrowableClasses[EThrowableType::FLASH_BANG], Param);
	FlashBang->SetItemStack(bIsCombatTestType ? 1 : FMath::RandRange(1, 2));
	FlashBang->MoveToInven(OwnerEnemy, FlashBang->GetItemCurStack());

}

void UC_DefaultItemSpawnerComponent::SpawnEquipableItems(const FActorSpawnParameters& Param)
{
	switch (OwnerEnemy->GetBehaviorType())
	{
	case EEnemyBehaviorType::InGamePlayable:
	{
		// Body Armors and Backpack
		AC_Vest* Vest = GetWorld()->SpawnActor<AC_Vest>(VestClass, Param);

		// 30% 확률로 3조끼 장착
		if (FMath::RandRange(0.f, 1.f) > 0.7f)
			Vest->SetItemLevel(EEquipableItemLevel::LV3);
		else Vest->SetItemLevel(static_cast<EEquipableItemLevel>(FMath::RandRange(0, 1)));

		Vest->InitVestDatasAndStaticMesh();
		
		Vest->MoveToSlot(OwnerEnemy, Vest->GetItemCurStack());

		// UC_Util::Print("Setting Vest Lv : " + FString::FromInt(static_cast<uint8>(Vest->GetItemLevel()) + 1), FColor::MakeRandomColor(), 10.f);

		// 20% 확률로 3가방 장착 & 장착 못하면 1, 2랩 가방은 각각 50%
		EEquipableItemLevel BackPackLevel = (FMath::RandRange(0.f, 1.f) > 0.8f) ? EEquipableItemLevel::LV3 :
											static_cast<EEquipableItemLevel>(FMath::RandRange(0, 1));
		
		AC_BackPack* BackPack = GetWorld()->SpawnActor<AC_BackPack>(BackPackClasses[BackPackLevel], Param);
		BackPack->MoveToSlot(OwnerEnemy, BackPack->GetItemCurStack());

		// UC_Util::Print("Setting BackPack Lv : " + FString::FromInt(static_cast<uint8>(BackPack->GetItemLevel()) + 1), FColor::MakeRandomColor(), 10.f);

		// 10% 확률로 3헬멧 장착
		EEquipableItemLevel HelmetLevel = (FMath::RandRange(0.f, 1.f) > 0.9f) ? EEquipableItemLevel::LV3 :
											static_cast<EEquipableItemLevel>(FMath::RandRange(0, 1));
			
		AC_Helmet* Helmet = GetWorld()->SpawnActor<AC_Helmet>(HelmetClasses[HelmetLevel], Param);
		Helmet->MoveToSlot(OwnerEnemy, Helmet->GetItemCurStack());

		// UC_Util::Print("Setting Helmet Lv : " + FString::FromInt(static_cast<uint8>(Helmet->GetItemLevel()) + 1), FColor::MakeRandomColor(), 10.f);
	}
		return;
	case EEnemyBehaviorType::StatCareTest: // 3가방만 장착
	{
		AC_BackPack* BackPack = GetWorld()->SpawnActor<AC_BackPack>(BackPackClasses[EEquipableItemLevel::LV3], Param);
		BackPack->MoveToSlot(OwnerEnemy, BackPack->GetItemCurStack());
	}
		return;
	case EEnemyBehaviorType::CombatTest: // 모두 3 Level Equipable로 장착
	{
		AC_Vest* Vest = GetWorld()->SpawnActor<AC_Vest>(VestClass, Param);
		Vest->SetItemLevel(EEquipableItemLevel::LV3);
		Vest->InitVestDatasAndStaticMesh();
		Vest->MoveToSlot(OwnerEnemy, Vest->GetItemCurStack());

		AC_BackPack* BackPack = GetWorld()->SpawnActor<AC_BackPack>(BackPackClasses[EEquipableItemLevel::LV3], Param);
		BackPack->MoveToSlot(OwnerEnemy, BackPack->GetItemCurStack());

		AC_Helmet* Helmet = GetWorld()->SpawnActor<AC_Helmet>(HelmetClasses[EEquipableItemLevel::LV3], Param);
		Helmet->MoveToSlot(OwnerEnemy, Helmet->GetItemCurStack());
	}
	}
}

void UC_DefaultItemSpawnerComponent::SpawnConsumableItems(const FActorSpawnParameters& Param)
{
	switch (OwnerEnemy->GetBehaviorType())
	{
	case EEnemyBehaviorType::MovementTest: case EEnemyBehaviorType::SkyDivingTest: case EEnemyBehaviorType::CombatTest: return;
	}
	
	// Stat Care Test Enemy 용 Spawn Consumable 처리
	if (OwnerEnemy->GetBehaviorType() == EEnemyBehaviorType::StatCareTest)
	{
		// MedKit 1개 Spawn
		AC_ConsumableItem* MedKit = GetWorld()->SpawnActor<AC_ConsumableItem>(ConsumableItemClasses[EConsumableItemType::MEDKIT], Param);
		MedKit->SetItemStack(1);
		MedKit->MoveToInven(OwnerEnemy, MedKit->GetItemCurStack());
		
		// 구급 상자 1개 Spawn
		AC_ConsumableItem* FirstAidKit = GetWorld()->SpawnActor<AC_ConsumableItem>(ConsumableItemClasses[EConsumableItemType::FIRST_AID_KIT], Param);
		FirstAidKit->SetItemStack(1);
		FirstAidKit->MoveToInven(OwnerEnemy, FirstAidKit->GetItemCurStack());

		// 에너지 드링크 1개 Spawn
		AC_ConsumableItem* Drink = GetWorld()->SpawnActor<AC_ConsumableItem>(ConsumableItemClasses[EConsumableItemType::ENERGY_DRINK], Param);
		Drink->SetItemStack(1);
		Drink->MoveToInven(OwnerEnemy, Drink->GetItemCurStack());

		// 진통제 1개 spawn
		AC_ConsumableItem* PainKiller = GetWorld()->SpawnActor<AC_ConsumableItem>(ConsumableItemClasses[EConsumableItemType::PAIN_KILLER], Param);
		PainKiller->SetItemStack(1);
		PainKiller->MoveToInven(OwnerEnemy, PainKiller->GetItemCurStack());

		return;
	}

	// InGamePlayable Spawn 처리
	
	// 의료용 키트 0~1개 Random spawn
	int MedKitCount = FMath::RandRange(0, 1);
	if (MedKitCount == 1)
	{
		AC_ConsumableItem* MedKit = GetWorld()->SpawnActor<AC_ConsumableItem>(ConsumableItemClasses[EConsumableItemType::MEDKIT], Param);
		MedKit->MoveToInven(OwnerEnemy, MedKit->GetItemCurStack());
	}

	// 구급 상자 4~6개 Random spawn
	AC_ConsumableItem* FirstAidKit = GetWorld()->SpawnActor<AC_ConsumableItem>(ConsumableItemClasses[EConsumableItemType::FIRST_AID_KIT], Param);
	FirstAidKit->SetItemStack(FMath::RandRange(4, 6));
	FirstAidKit->MoveToInven(OwnerEnemy, FirstAidKit->GetItemCurStack());
	
	// 붕대 5개 spawn
	AC_ConsumableItem* Bandage = GetWorld()->SpawnActor<AC_ConsumableItem>(ConsumableItemClasses[EConsumableItemType::BANDAGE], Param);
	Bandage->MoveToInven(OwnerEnemy, Bandage->GetItemCurStack());

	// 에너지드링크 3~5개 Random spawn
	AC_ConsumableItem* Drink = GetWorld()->SpawnActor<AC_ConsumableItem>(ConsumableItemClasses[EConsumableItemType::ENERGY_DRINK], Param);
	Drink->SetItemStack(FMath::RandRange(3, 5));
	Drink->MoveToInven(OwnerEnemy, Drink->GetItemCurStack());

	// 진통제 2~3개 Random spawn
	AC_ConsumableItem* PainKiller = GetWorld()->SpawnActor<AC_ConsumableItem>(ConsumableItemClasses[EConsumableItemType::PAIN_KILLER], Param);
	PainKiller->SetItemStack(FMath::RandRange(2, 3));
	PainKiller->MoveToInven(OwnerEnemy, PainKiller->GetItemCurStack());

	/*for (int i = 0; i < static_cast<int>(EConsumableItemType::MAX); ++i)
	{
		FName ItemName = AC_ConsumableItem::GetConsumableItemName(static_cast<EConsumableItemType>(i));
		AC_Item* ConsumableItem = OwnerEnemy->GetInvenComponent()->FindMyItemByName(ItemName);

		FString Str{};
		
		if (!IsValid(ConsumableItem))
		{
			Str = ItemName.ToString() + " : does not exists.";
			UC_Util::Print(Str, FColor::Red, 10.f);
			continue;
		}
		
		Str = ItemName.ToString() + "'s Count : " + FString::FromInt(ConsumableItem->GetItemCurStack());
		UC_Util::Print(Str, FColor::Red, 10.f);
	}*/
}

void UC_DefaultItemSpawnerComponent::SpawnBullets(const FActorSpawnParameters& Param)
{
	// InGamePlayable만 탄알 spawn 처리
	if (OwnerEnemy->GetBehaviorType() != EEnemyBehaviorType::InGamePlayable) return;
	
	// 5.56mm 탄 200발, 7.62mm 탄 30발
	for (int i = 0; i < 2; ++i)
	{
		AC_Item_Bullet* FiveMMBullet = GetWorld()->SpawnActor<AC_Item_Bullet>(BulletClasses[EBulletType::FIVEMM], Param);
		FiveMMBullet->MoveToInven(OwnerEnemy, FiveMMBullet->GetItemCurStack());
	}
	
	AC_Item_Bullet* SevenMMBullet = GetWorld()->SpawnActor<AC_Item_Bullet>(BulletClasses[EBulletType::SEVENMM], Param);
	SevenMMBullet->MoveToInven(OwnerEnemy, SevenMMBullet->GetItemCurStack());

	FName FiveBulletName = AC_Item_Bullet::GetBulletTypeName(EBulletType::FIVEMM);
	FName SevenBulletName = AC_Item_Bullet::GetBulletTypeName(EBulletType::SEVENMM);

	AC_Item* FiveBulletItem		= OwnerEnemy->GetInvenComponent()->FindMyItemByName(FiveBulletName);
	AC_Item* SevenBulletItem	= OwnerEnemy->GetInvenComponent()->FindMyItemByName(SevenBulletName);

	if (!IsValid(FiveBulletItem)) return;
	
	FString Str = "Five Bullet Spawned Count : " + FString::FromInt(FiveBulletItem->GetItemCurStack());
	UC_Util::Print(Str, FColor::Red, 10.f);

	if (!IsValid(SevenBulletItem))
	{
		UC_Util::Print("No SevenBullet in Inven!", FColor::Red, 20.f);
		return;
	}

	Str = "Seven Bullet Spawned Count : " + FString::FromInt(SevenBulletItem->GetItemCurStack());
	UC_Util::Print(Str, FColor::Red, 10.f);
}
