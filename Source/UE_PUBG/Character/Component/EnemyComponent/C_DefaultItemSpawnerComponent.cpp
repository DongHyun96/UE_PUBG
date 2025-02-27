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
}


void UC_DefaultItemSpawnerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UC_DefaultItemSpawnerComponent::SpawnDefaultWeaponsAndItems()
{
	FActorSpawnParameters Param{};
	Param.Owner = OwnerEnemy;
	
	AC_MeleeWeapon* MeleeWeapon = GetWorld()->SpawnActor<AC_MeleeWeapon>(WeaponClasses[EWeaponSlot::MELEE_WEAPON], Param);
	OwnerEnemy->GetEquippedComponent()->SetSlotWeapon(EWeaponSlot::MELEE_WEAPON, MeleeWeapon);

	AC_Gun* MainGun = GetWorld()->SpawnActor<AC_Gun>(WeaponClasses[EWeaponSlot::MAIN_GUN], Param);
	MainGun->MoveToSlot(OwnerEnemy);
	//EquippedComponent->SetSlotWeapon(EWeaponSlot::MAIN_GUN, MainGun);

	AC_Gun* SubGun = GetWorld()->SpawnActor<AC_Gun>(WeaponClasses[EWeaponSlot::SUB_GUN], Param);
	SubGun->MoveToSlot(OwnerEnemy);
	//EquippedComponent->SetSlotWeapon(EWeaponSlot::SUB_GUN, SubGun);

	// Throwable Weapon setting 하기
	for (auto& pair : ThrowableClasses)
	{
		AC_ThrowingWeapon* ThrowWeapon = GetWorld()->SpawnActor<AC_ThrowingWeapon>(pair.Value, Param);
		ThrowWeapon->MoveToSlot(OwnerEnemy);
	}

	// "FlashBang"
	// "Grenade"
	AC_ThrowingWeapon* Grenade = Cast<AC_ThrowingWeapon>(OwnerEnemy->GetInvenComponent()->FindMyItem("Grenade"));
	if (!IsValid(Grenade))
	{
		UC_Util::Print("From SpawnDefaultWeaponForEnemy : Grenade nullptr", FColor::Red, 10.f);
		return;
	}
	
	if (!Grenade->MoveToSlot(OwnerEnemy))
		UC_Util::Print("From SpawnDefaultWeaponForEnemy : Grenade MoveToSlot Failed!", FColor::Red, 10.f);


	// Body Armors and Backpack
	AC_Vest* Vest = GetWorld()->SpawnActor<AC_Vest>(VestClass, Param);

	// 30% 확률로 3조끼 장착
	if (FMath::RandRange(0.f, 1.f) > 0.7f)
		Vest->SetItemLevel(EEquipableItemLevel::LV3);
	else Vest->SetItemLevel(static_cast<EEquipableItemLevel>(FMath::RandRange(0, 1)));

	Vest->InitVestDatasAndStaticMesh();
	
	Vest->MoveToSlot(OwnerEnemy);

	// UC_Util::Print("Setting Vest Lv : " + FString::FromInt(static_cast<uint8>(Vest->GetItemLevel()) + 1), FColor::MakeRandomColor(), 10.f);

	// 20% 확률로 3가방 장착 & 장착 못하면 1, 2랩 가방은 각각 50%
	EEquipableItemLevel BackPackLevel = (FMath::RandRange(0.f, 1.f) > 0.8f) ? EEquipableItemLevel::LV3 :
										static_cast<EEquipableItemLevel>(FMath::RandRange(0, 1));
	
	AC_BackPack* BackPack = GetWorld()->SpawnActor<AC_BackPack>(BackPackClasses[BackPackLevel]);
	BackPack->MoveToSlot(OwnerEnemy);

	// UC_Util::Print("Setting BackPack Lv : " + FString::FromInt(static_cast<uint8>(BackPack->GetItemLevel()) + 1), FColor::MakeRandomColor(), 10.f);

	// 10% 확률로 3헬멧 장착
	EEquipableItemLevel HelmetLevel = (FMath::RandRange(0.f, 1.f) > 0.9f) ? EEquipableItemLevel::LV3 :
										static_cast<EEquipableItemLevel>(FMath::RandRange(0, 1));
		
	AC_Helmet* Helmet = GetWorld()->SpawnActor<AC_Helmet>(HelmetClasses[HelmetLevel]);
	Helmet->MoveToSlot(OwnerEnemy);

	// UC_Util::Print("Setting Helmet Lv : " + FString::FromInt(static_cast<uint8>(Helmet->GetItemLevel()) + 1), FColor::MakeRandomColor(), 10.f);
	
	// TODO : 다른 Item들 (탄, Consumable item 등등 inven에 넣어두기)
}

