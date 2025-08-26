// Fill out your copyright notice in the Description page of Project Settings.


#include "C_PlayerCombatFieldManager.h"

#include "C_PlayerCombatFieldWidget.h"
#include "Character/C_Enemy.h"
#include "Character/C_Player.h"
#include "Character/Component/C_EquippedComponent.h"
#include "Character/Component/C_InputComponent.h"
#include "Character/Component/C_InvenComponent.h"
#include "Character/Component/C_InvenSystem.h"
#include "Character/Component/EnemyComponent/C_DefaultItemSpawnerComponent.h"
#include "Components/ShapeComponent.h"
#include "Door/C_TutorialGate.h"
#include "InvenUI/BasicItemSlot/WeaponSlot/GunSlot/C_MainGunSlotWidget.h"
#include "InvenUI/BasicItemSlot/WeaponSlot/GunSlot/C_SubGunSlotWidget.h"
#include "InvenUI/Panel/ItemPanel/EquipmentPanel/C_EquipmentPanelWidget.h"
#include "Item/Attachment/C_AttachableItem.h"
#include "Item/ConsumableItem/C_ConsumableItem.h"
#include "Item/Equipment/C_BackPack.h"
#include "Item/Equipment/C_EquipableItem.h"
#include "Item/Equipment/C_Helmet.h"
#include "Item/Equipment/C_Vest.h"
#include "Item/Weapon/C_Weapon.h"
#include "Item/Weapon/Gun/C_Gun.h"
#include "Item/Weapon/ThrowingWeapon/C_ThrowingWeapon.h"
#include "Singleton/C_GameSceneManager.h"
#include "Utility/C_Util.h"


UC_PlayerCombatFieldManager::UC_PlayerCombatFieldManager()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UC_PlayerCombatFieldManager::BeginPlay()
{
	Super::BeginPlay();

	if (!CombatFieldStartGate)
	{
		UC_Util::Print("From UC_PlayerCombatFieldManager::BeginPlay : CombatFieldGate nullptr!", FColor::Red, 10.f);
		return;
	}

	GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
	{
		CombatFieldStartGate->GetGateOpeningBoxComponent()->OnComponentBeginOverlap.AddDynamic(this, &UC_PlayerCombatFieldManager::OnGateOpeningBoxBeginOverlap);
		CombatFieldStartGate->GetGateOpeningBoxComponent()->OnComponentEndOverlap.AddDynamic(this, &UC_PlayerCombatFieldManager::OnGateOpeningBoxEndOverlap);
		
		CombatFieldStartGate->ToggleOpeningBoxTriggerEnabled(true);
	});
}

void UC_PlayerCombatFieldManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UC_PlayerCombatFieldManager::OnGateOpeningBoxBeginOverlap
(
	UPrimitiveComponent*	OverlappedComp,
	AActor*					OtherActor,
	UPrimitiveComponent*	OtherComp,
	int32					OtherBodyIndex,
	bool					bFromSweep,
	const FHitResult&		SweepResult
)
{
	AC_Player* Player = Cast<AC_Player>(OtherActor);
	if (!Player) return;

	PlayerCombatFieldWidget->ToggleStartFKeyInstruction(true);
	GAMESCENE_MANAGER->GetPlayer()->GetInputComponent()->CombatControlFKeyInteractionDelegate.BindUObject(this, &UC_PlayerCombatFieldManager::OnStartGateFKeyInteraction);	
}

void UC_PlayerCombatFieldManager::OnGateOpeningBoxEndOverlap
(
	UPrimitiveComponent* OverlappedComponent,
	AActor*				 OtherActor,
	UPrimitiveComponent* OtherComp,
	int32				 OtherBodyIndex
)
{
	AC_Player* Player = Cast<AC_Player>(OtherActor);
	if (!Player) return;

	GAMESCENE_MANAGER->GetPlayer()->GetInputComponent()->CombatControlFKeyInteractionDelegate.Unbind();
	PlayerCombatFieldWidget->ToggleStartFKeyInstruction(false);
}

bool UC_PlayerCombatFieldManager::OnStartGateFKeyInteraction()
{
	// TODO : Match 끝난 뒤, 다시 활성화 시켜주기 (Match 끝나면 CombatField에서 나가되, OpeningGateBox 바깥에 Player 두기)
	CombatFieldStartGate->ToggleOpeningBoxTriggerEnabled(false);

	UC_Util::Print("START PLAYER COMBAT FIELD", FColor::MakeRandomColor(), 10.f);

	/* Init Matching */
		
	// Round Start 전처리 때 계속 사용될 예정
	
	/*
	 * Player Inven check -> Combat에 필요한 아이템으로 초기화
	 * 초기화할 때 이미 아이템(EquipmentItem, Weapon 등 모든 아이템들)이 존재한다면, 개수만 조정해서 재활용할 것
	 * 없다면 새로 Spawn시켜서 Player inven 및 EquippedComponent에 장착해주기
	 * 불필요한 아이템이 존재한다면 삭제 처리하기
	 */
	InitPlayerCombatItems();
	
	// Player Enemy Start 위치로 초기화
	
	// Player 특정 input들(공격 처리, 이동 등) Round Start 이전까지 막기
	// Round Start Timer 초기화 및 초 세기 (UI 띄우기)

	return true;
}

void UC_PlayerCombatFieldManager::InitPlayerCombatItems()
{
	// TODO : 다 Spawn 및 초기화 이후, Inven UI 통합 초기화 처리를 해주어야 업데이트 됨 
	
	AC_Player* Player = GAMESCENE_MANAGER->GetPlayer();
	UC_InvenComponent* InvenComponent = Player->GetInvenComponent();
	UC_EquippedComponent* EquippedComponent = Player->GetEquippedComponent();
	
	FActorSpawnParameters SpawnParam{};
	SpawnParam.Owner = Player;

	
	if (!IsValid(CombatFieldEnemy))
	{
		UC_Util::Print("From UC_PlayerCombatFieldManager::InitPlayerCombatItems : Invalid CombatFieldEnemy", FColor::Red, 10.f);
		return;
	}
	
	UC_DefaultItemSpawnerComponent* DefaultItemSpawnerHelper = CombatFieldEnemy->GetItemSpawnerHelper();

	/* Init Equipment items (Vest, Helmet, Backpack) */

	// Init BackPack
	AC_EquipableItem* CurrentBackPack = InvenComponent->GetSlotEquipment(EEquipSlot::BACKPACK);
	AC_EquipableItem* Lv3BackPack = GetWorld()->SpawnActor<AC_EquipableItem>(DefaultItemSpawnerHelper->GetBackPackClass(EEquipableItemLevel::LV3), SpawnParam);
	Lv3BackPack->MoveToSlot(Player, Lv3BackPack->GetItemCurStack());
	if (CurrentBackPack) CurrentBackPack->DestroyItem();

	// Init Helmet
	AC_EquipableItem* CurrentHelmet = InvenComponent->GetSlotEquipment(EEquipSlot::HELMET);
	AC_EquipableItem* Lv3Helmet = GetWorld()->SpawnActor<AC_EquipableItem>(DefaultItemSpawnerHelper->GetHelmetClass(EEquipableItemLevel::LV3), SpawnParam);
	Lv3Helmet->MoveToSlot(Player, Lv3Helmet->GetItemCurStack());
	if (CurrentHelmet) CurrentHelmet->DestroyItem();

	// Init Vest
	AC_EquipableItem* CurrentVest = InvenComponent->GetSlotEquipment(EEquipSlot::VEST);
	AC_Vest* Lv3Vest = GetWorld()->SpawnActor<AC_Vest>(DefaultItemSpawnerHelper->GetVestClass(), SpawnParam);
	Lv3Vest->SetItemLevel(EEquipableItemLevel::LV3);
	Lv3Vest->InitVestDatasAndStaticMesh();
	Lv3Vest->MoveToSlot(Player, Lv3Vest->GetItemCurStack());
	if (CurrentVest) CurrentVest->DestroyItem();

	/* Init Weapons */

	// Init MeleeWeapon
	AC_Weapon* MeleeWeapon = EquippedComponent->GetWeapons()[EWeaponSlot::MELEE_WEAPON];
	if (!MeleeWeapon)
	{
		MeleeWeapon = GetWorld()->SpawnActor<AC_Weapon>(DefaultItemSpawnerHelper->GetWeaponClass(EWeaponSlot::MELEE_WEAPON), SpawnParam);
		MeleeWeapon->MoveToSlot(Player, MeleeWeapon->GetItemCurStack());
	}

	// Init Guns

	// Deleting Current Guns
	for (int i = 1; i < 3; ++i)
	{
		const EWeaponSlot GunWeaponSlot = static_cast<EWeaponSlot>(i);
		
		if (AC_Gun* CurrentGun = Cast<AC_Gun>(EquippedComponent->GetWeapons()[GunWeaponSlot]))
		{
			for (TPair<EPartsName, AC_AttachableItem*>& Pair : CurrentGun->GetAttachableItemReference())
			{
				AC_AttachableItem* Item = Pair.Value;
				if (!IsValid(Item)) continue;

				Item->MoveToAround(Player, Item->GetItemCurStack());
				Item->DestroyItem();
				Pair.Value = nullptr;
			}
		
			CurrentGun->MoveToAround(Player, CurrentGun->GetItemCurStack());
			CurrentGun->DestroyItem();
		}
	}

	// Spawn Aug and Attachables
	AC_Gun* Aug						= GetWorld()->SpawnActor<AC_Gun>(DefaultItemSpawnerHelper->GetWeaponClass(EWeaponSlot::MAIN_GUN));
	AC_AttachableItem* VertGrip 	= GetWorld()->SpawnActor<AC_AttachableItem>(GunAttachableClasses[EAttachmentNames::VERTGRIP], SpawnParam);
	AC_AttachableItem* Compensator	= GetWorld()->SpawnActor<AC_AttachableItem>(GunAttachableClasses[EAttachmentNames::COMPENSATOR], SpawnParam);
	AC_AttachableItem* RedDot		= GetWorld()->SpawnActor<AC_AttachableItem>(GunAttachableClasses[EAttachmentNames::REDDOT], SpawnParam);
	
	/*Aug->SetAttachableItemSlot(EPartsName::GRIP, VertGrip);
	Aug->SetAttachableItemSlot(EPartsName::SCOPE, RedDot);
	Aug->SetAttachableItemSlot(EPartsName::MUZZLE, Compensator);*/
	
	Aug->MoveToSlot(Player, Aug->GetItemCurStack());
	Aug->SetActorHiddenInGame(false);
	Aug->SetCurMagazineBulletCount(30);

	UC_GunSlotWidget* MainGunSlotWidget = Player->GetInvenSystem()->GetInvenUI()->GetEquipmentPanel()->GetMainGunSlot();
	UC_SubGunSlotWidget* SubGunSlotWidget = Player->GetInvenSystem()->GetInvenUI()->GetEquipmentPanel()->GetSubGunSlot();
	
	MainGunSlotWidget->SetAttachmentSlotOnDrop(Aug, VertGrip);
	MainGunSlotWidget->SetAttachmentSlotOnDrop(Aug, Compensator);
	MainGunSlotWidget->SetAttachmentSlotOnDrop(Aug, RedDot);
	
	// Spawn MosinNagant and Attachables
	AC_Gun* MosinNagant			= GetWorld()->SpawnActor<AC_Gun>(DefaultItemSpawnerHelper->GetWeaponClass(EWeaponSlot::SUB_GUN));
	AC_AttachableItem* Scope	= GetWorld()->SpawnActor<AC_AttachableItem>(GunAttachableClasses[EAttachmentNames::SCOPE4], SpawnParam);

	MosinNagant->MoveToSlot(Player, MosinNagant->GetItemCurStack());
	MosinNagant->SetActorHiddenInGame(false);
	MosinNagant->SetCurMagazineBulletCount(5);
	
	SubGunSlotWidget->SetAttachmentSlotOnDrop(MosinNagant, Scope);

	// TODO : Ammo 넣고 끝에 처리해주기
	
	MainGunSlotWidget->UpdateWidget();
	SubGunSlotWidget->UpdateWidget();

	// Throwable 초기화

	// Removing Slot throwable
	AC_ThrowingWeapon* SlotThrowingWeapon = Cast<AC_ThrowingWeapon>(EquippedComponent->GetWeapons()[EWeaponSlot::THROWABLE_WEAPON]);
	if (SlotThrowingWeapon)
	{
		SlotThrowingWeapon->MoveToAround(Player, SlotThrowingWeapon->GetItemCurStack());
		SlotThrowingWeapon->DestroyItem();
	}

	// Resetting Inven Throwable
	for (uint8 i = 0; i < static_cast<uint8>(EThrowableType::MAX); ++i)
	{
		const EThrowableType ThrowableType = static_cast<EThrowableType>(i);
		const FName WeaponName = AC_ThrowingWeapon::GetThrowableItemName(ThrowableType);
		AC_Item* ThrowableWeapon = InvenComponent->FindMyItemByName(WeaponName);

		if (ThrowableWeapon)
			ThrowableWeapon->SetItemStack(1); // 이미 Inven에 해당 종류의 Throwable이 존재한다면 하나로 개수 초기화
		else
		{
			AC_ThrowingWeapon* SpawnedThrowable = GetWorld()->SpawnActor<AC_ThrowingWeapon>(DefaultItemSpawnerHelper->GetThrowableClass(ThrowableType), SpawnParam);
			SpawnedThrowable->SetItemStack(1);
			SpawnedThrowable->MoveToInven(Player, 1);
		}
	}

	// Set Slot throwable to grenade
	AC_Item* Grenade = InvenComponent->FindMyItemByName(AC_ThrowingWeapon::GetThrowableItemName(EThrowableType::GRENADE));
	if (Grenade) Grenade->MoveToSlot(Player, Grenade->GetItemCurStack());

	// Consumable Item 초기화

	// Deleting Consumable Items
	for (uint8 i = 0; i < static_cast<uint8>(EConsumableItemType::MAX); ++i)
	{
		const EConsumableItemType ConsumableItemType = static_cast<EConsumableItemType>(i);
		const FName ItemName = AC_ConsumableItem::GetConsumableItemName(ConsumableItemType); 
	}
	
}
