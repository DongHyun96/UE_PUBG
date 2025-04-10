// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/EnemyComponent/C_DefaultItemSpawnerComponent.h"

#include "Character/C_Enemy.h"
#include "Character/Component/C_EquippedComponent.h"
#include "Item/Equipment/C_BackPack.h"
#include "Item/Equipment/C_Helmet.h"
#include "Item/Equipment/C_Vest.h"
#include "Item/Weapon/MeleeWeapon/C_MeleeWeapon.h"
#include "Item/Weapon/C_Weapon.h"
#include "Item/Weapon/Gun/C_Gun.h"
#include "Item/Weapon/ThrowingWeapon/C_ThrowingWeapon.h"
#include "Utility/C_Util.h"

UC_DefaultItemSpawnerComponent::UC_DefaultItemSpawnerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UC_DefaultItemSpawnerComponent::BeginPlay()
{
	Super::BeginPlay();
	SpawnDefaultWeaponsAndItems(); // TODO : 이 라인 지우기
}


void UC_DefaultItemSpawnerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UC_DefaultItemSpawnerComponent::SpawnDefaultWeaponsAndItems()
{
	FActorSpawnParameters Param{};
	Param.Owner = OwnerEnemy;

	SpawnWeapons(Param);
	SpawnEquipableItems(Param);
	SpawnConsumableItems(Param);
	SpawnBullets(Param);
	// TODO : 다른 Item들 (탄, Consumable item 등등 inven에 넣어두기)
	
	
}

void UC_DefaultItemSpawnerComponent::SpawnWeapons(const FActorSpawnParameters& Param)
{
	if (WeaponClasses.IsEmpty())
	{
		UC_Util::Print("WeaponClasses empty!", FColor::Red, 10.f);
		return;
	}
	
	AC_MeleeWeapon* MeleeWeapon = GetWorld()->SpawnActor<AC_MeleeWeapon>(WeaponClasses[EWeaponSlot::MELEE_WEAPON], Param);
	OwnerEnemy->GetEquippedComponent()->SetSlotWeapon(EWeaponSlot::MELEE_WEAPON, MeleeWeapon);

	AC_Gun* MainGun = GetWorld()->SpawnActor<AC_Gun>(WeaponClasses[EWeaponSlot::MAIN_GUN], Param);
	MainGun->MoveToSlot(OwnerEnemy, MainGun->GetItemCurStack());
	MainGun->SetActorHiddenInGame(false); // 어디선가 Hidden처리가 되어 나옴
	//EquippedComponent->SetSlotWeapon(EWeaponSlot::MAIN_GUN, MainGun);

	AC_Gun* SubGun = GetWorld()->SpawnActor<AC_Gun>(WeaponClasses[EWeaponSlot::SUB_GUN], Param);
	SubGun->MoveToSlot(OwnerEnemy, SubGun->GetItemCurStack());
	SubGun->SetActorHiddenInGame(false);
	//EquippedComponent->SetSlotWeapon(EWeaponSlot::SUB_GUN, SubGun);

	// Throwable Weapon setting 하기
	for (auto& pair : ThrowableClasses)
	{
		AC_ThrowingWeapon* ThrowWeapon = GetWorld()->SpawnActor<AC_ThrowingWeapon>(pair.Value, Param);
		ThrowWeapon->MoveToSlot(OwnerEnemy, ThrowWeapon->GetItemCurStack());
	}

	// "FlashBang"
	// "Grenade"
	FName GrenadeItemName = AC_ThrowingWeapon::GetThrowableItemNameMap()[EThrowableType::GRENADE];
	AC_ThrowingWeapon* Grenade = Cast<AC_ThrowingWeapon>(OwnerEnemy->GetInvenComponent()->FindMyItemByName(GrenadeItemName));
	if (!IsValid(Grenade))
		UC_Util::Print("From SpawnDefaultWeaponForEnemy : Grenade nullptr", FColor::Red, 10.f);
		
	if (Grenade) if (!Grenade->MoveToSlot(OwnerEnemy, Grenade->GetItemCurStack()))
		UC_Util::Print("From SpawnDefaultWeaponForEnemy : Grenade MoveToSlot Failed!", FColor::Red, 10.f);
}

void UC_DefaultItemSpawnerComponent::SpawnEquipableItems(const FActorSpawnParameters& Param)
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
	
	AC_BackPack* BackPack = GetWorld()->SpawnActor<AC_BackPack>(BackPackClasses[BackPackLevel]);
	BackPack->MoveToSlot(OwnerEnemy, BackPack->GetItemCurStack());

	// UC_Util::Print("Setting BackPack Lv : " + FString::FromInt(static_cast<uint8>(BackPack->GetItemLevel()) + 1), FColor::MakeRandomColor(), 10.f);

	// 10% 확률로 3헬멧 장착
	EEquipableItemLevel HelmetLevel = (FMath::RandRange(0.f, 1.f) > 0.9f) ? EEquipableItemLevel::LV3 :
										static_cast<EEquipableItemLevel>(FMath::RandRange(0, 1));
		
	AC_Helmet* Helmet = GetWorld()->SpawnActor<AC_Helmet>(HelmetClasses[HelmetLevel]);
	Helmet->MoveToSlot(OwnerEnemy, Helmet->GetItemCurStack());

	// UC_Util::Print("Setting Helmet Lv : " + FString::FromInt(static_cast<uint8>(Helmet->GetItemLevel()) + 1), FColor::MakeRandomColor(), 10.f);
}

void UC_DefaultItemSpawnerComponent::SpawnConsumableItems(const FActorSpawnParameters& Param)
{
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

	// 제대로 spawn 처리 안되고 있는 중
	for (int i = 0; i < static_cast<int>(EConsumableItemType::MAX); ++i)
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
	}
}

void UC_DefaultItemSpawnerComponent::SpawnBullets(const FActorSpawnParameters& Param)
{
	// TODO : 5.56 7.62 탄 spawn 시켜주기
}
