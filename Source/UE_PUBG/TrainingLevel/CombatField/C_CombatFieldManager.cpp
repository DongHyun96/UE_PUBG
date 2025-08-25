// Fill out your copyright notice in the Description page of Project Settings.


#include "C_CombatFieldManager.h"

#include "C_CombatFieldWidget.h"
#include "C_EnemyCombatFieldManager.h"
#include "C_PlayerCombatFieldManager.h"
#include "AI/C_BehaviorComponent.h"
#include "AI/C_EnemyAIController.h"
#include "AI/Task/CombatTask/C_BTTaskSwapWeapon.h"
#include "Character/C_Enemy.h"
#include "Character/C_Player.h"
#include "Character/Component/C_InvenComponent.h"
#include "Character/Component/EnemyComponent/C_DefaultItemSpawnerComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HUD/C_HUDWidget.h"
#include "HUD/C_TimeBoxWidget.h"
#include "Item/ConsumableItem/C_ConsumableItem.h"
#include "Item/Equipment/C_EquipableItem.h"
#include "Item/Equipment/C_Helmet.h"
#include "Item/Weapon/Gun/C_Gun.h"
#include "Item/Weapon/ThrowingWeapon/C_ThrowingWeapon.h"
#include "Perception/AIPerceptionComponent.h"
#include "Singleton/C_GameSceneManager.h"
#include "Utility/C_Util.h"


AC_CombatFieldManager::AC_CombatFieldManager()
{
	PrimaryActorTick.bCanEverTick = true;

	EnemyCombatFieldManager  = CreateDefaultSubobject<UC_EnemyCombatFieldManager>(TEXT("EnemyVsEnemyCombatFieldManager"));
	PlayerCombatFieldManager = CreateDefaultSubobject<UC_PlayerCombatFieldManager>(TEXT("PlayerVsEnemyCombatFieldManager"));

	EnemyCombatFieldManager->SetOwnerCombatFieldManager(this);
	PlayerCombatFieldManager->SetOwnerCombatFieldManager(this);
}

void AC_CombatFieldManager::BeginPlay()
{
	Super::BeginPlay();

	/* for (int i = 0; i < 2; ++i)
	{
		EnemyVsEnemySpawnTransform.Add(VersusAIEnemies[i]->GetActorTransform());
		PlayerVsEnemySpawnTransform.Add(VersusPlayerEnemies[i]->GetActorTransform());
	} */

	/* PlayerVsEnemySpawnTransform 저장 */
	const UCapsuleComponent* VersusPlayerSpawnCapsuleComponent = Cast<UCapsuleComponent>(GetDefaultSubobjectByName(TEXT("VersusPlayerSpawnTransform")));
	if (!VersusPlayerSpawnCapsuleComponent) UC_Util::Print("From AC_CombatFieldManager::BeginPlay : VersusPlayerSpawnCapsuleComponent missing!", FColor::Red, 10.f);
	else PlayerVsEnemySpawnTransform.Add(VersusPlayerSpawnCapsuleComponent->GetComponentTransform());

	if (VersusPlayerEnemy)
	{
		PlayerVsEnemySpawnTransform.Add(VersusPlayerEnemy->GetActorTransform());
		if (VersusPlayerEnemy->GetBehaviorType() != EEnemyBehaviorType::CombatTest)
			UC_Util::Print("From AC_CombatFieldManager::BeginPlay : VersusPlayerEnemy Behavior Type is not COMBAT", FColor::Red, 10.f);
	}

	for (AC_Enemy* Enemy : VersusAIEnemies)
	{
		if (Enemy->GetBehaviorType() != EEnemyBehaviorType::CombatTest)
			UC_Util::Print("From AC_CombatFieldManager::BeginPlay : VersusEnemy Behavior Type is not COMBAT", FColor::Red, 10.f);

		// EnemyVsEnemySpawnTransform 저장
		EnemyVsEnemySpawnTransform.Add(Enemy->GetActorTransform());

		// 죽은 뒤, Destroy될 때 Delegate 지정
		Enemy->Delegate_OnCombatCharacterDestroy.BindUObject(this, &AC_CombatFieldManager::StartEnemyVsEnemyRound);

		// Initial Mesh Transform 저장
		CharacterMeshInitialRelativeTransform = Enemy->GetMesh()->GetRelativeTransform();
	}

	// VersusPlayerEnemy의 TargetCharacter 세팅은 P vs E 라운드가 시작되기 직전에 잡아주기 & Boost stat도 라운드가 시작되면 최대치로

	if (!CombatFieldWidget)
	{
		UC_Util::Print("CombatFieldWidget not inited in BPC_CombatFieldManager!", FColor::Red, 10.f);
		return;
	}

	CombatFieldWidget->AddToViewport(15);
	CombatFieldWidget->SetOwnerCombatFieldManager(this);

	PlayerCombatFieldManager->SetPlayerCombatFieldWidget(CombatFieldWidget->GetPlayerCombatFieldWidget());
	PlayerCombatFieldManager->SetCombatFieldEnemy(VersusPlayerEnemy);
}

void AC_CombatFieldManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

AC_Enemy* AC_CombatFieldManager::GetVersusAIEnemy(uint8 Index) const
{
	if (Index >= VersusAIEnemies.Num()) return nullptr;
	return VersusAIEnemies[Index];
}

void AC_CombatFieldManager::InitEnemyVsEnemyRound()
{
	uint8 Index{};
	
	for (AC_Enemy* Enemy : VersusAIEnemies)
	{
		// 죽은 Enemy에 한해 소생 처리
		TryReviveCharacter(Enemy);

		// Spawn Transform으로 위치 지정
		Enemy->SetActorTransform(EnemyVsEnemySpawnTransform[Index], false, nullptr, ETeleportType::TeleportPhysics);
		
		// Boost량과 체력 최대치 설정
		Enemy->GetStatComponent()->SetCurBoosting(100.f);
		Enemy->GetStatComponent()->SetCurHP(100.f);

		// MainGun 상태로 시작
		if (Enemy->GetEquippedComponent()->GetWeapons()[EWeaponSlot::MAIN_GUN])
			Enemy->GetEquippedComponent()->ChangeCurWeapon(EWeaponSlot::MAIN_GUN);
		else UC_Util::Print("From AC_CombatFieldManager::StartEnemyVsEnemyRound : Enemy Main Gun Slot empty!", FColor::Red, 10.f);
		
		// BehaviorTree Task Wait으로 setting
		const AC_EnemyAIController* Controller = Enemy->GetController<AC_EnemyAIController>();
		Controller->GetBehaviorComponent()->SetServiceType(EServiceType::IDLE);
		Controller->GetBehaviorComponent()->SetIdleTaskType(EIdleTaskType::WAIT);

		/* 기본 장비 및 아이템 필요하다면 재정비 */
		
		FActorSpawnParameters SpawnParams{};
		SpawnParams.Owner = Enemy;
		
		UC_InvenComponent* InvenComponent = Enemy->GetInvenComponent();

		// Helmet이 Slot에 존재한다면, 해당 Helmet의 내구도 다시 repair
		if (AC_EquipableItem* Helmet = InvenComponent->GetSlotEquipment(EEquipSlot::HELMET))
			Helmet->RepairDurabilityToMax();
		else // 기존 Helmet의 내구도가 0이되어, Helmet이 없는 상황 / 새로운 Helmet 스폰 처리
		{
			TSubclassOf<AC_Helmet> HelmetClass = Enemy->GetItemSpawnerHelper()->GetHelmetClass(EEquipableItemLevel::LV3);
			Helmet = GetWorld()->SpawnActor<AC_Helmet>(HelmetClass, SpawnParams);
			Helmet->MoveToSlot(Enemy, Helmet->GetItemCurStack());
		}

		// Vest Repair
		InvenComponent->GetSlotEquipment(EEquipSlot::VEST)->RepairDurabilityToMax();

		UC_EquippedComponent* EquippedComponent = Enemy->GetEquippedComponent();

		// Gun CurMagazineBulletCount 재정비
		AC_Gun* MainGun = Cast<AC_Gun>(EquippedComponent->GetWeapons()[EWeaponSlot::MAIN_GUN]);
		MainGun->SetCurMagazineBulletCount(30);

		AC_Gun* SubGun = Cast<AC_Gun>(EquippedComponent->GetWeapons()[EWeaponSlot::SUB_GUN]);
		SubGun->SetCurMagazineBulletCount(5);

		// Throwable 재정비 (Throwable 종류별 각각 1개씩 있어야 함)
		// 개수 파악용 TMap
		TMap<EThrowableType, bool> PossessThrowableMap =
		{
			{EThrowableType::GRENADE, false}, {EThrowableType::SMOKE, false}, {EThrowableType::FLASH_BANG, false}  
		};

		// Slot에 장착된 Throwable 조사
		const AC_ThrowingWeapon* SlotThrowingWeapon = Cast<AC_ThrowingWeapon>(EquippedComponent->GetWeapons()[EWeaponSlot::THROWABLE_WEAPON]);
		if (SlotThrowingWeapon) PossessThrowableMap[SlotThrowingWeapon->GetThrowableType()] = true;
			
		// Inven에 있는 Throwable 개수 조사
		for (uint8 j = 0; j < static_cast<uint8>(EThrowableType::MAX); ++j)
		{
			EThrowableType ThrowableType = static_cast<EThrowableType>(j);
			if (InvenComponent->FindMyItemByName(AC_ThrowingWeapon::GetThrowableItemName(ThrowableType)))
				PossessThrowableMap[ThrowableType] = true;
		}

		// 없는 ThrowingWeapon은 모두 새로 생성해서 집어넣어줌
		for (const TPair<EThrowableType, bool>& Pair : PossessThrowableMap)
		{
			if (!Pair.Value)
			{
				TSubclassOf<AC_ThrowingWeapon> ThrowableWeaponClass = Enemy->GetItemSpawnerHelper()->GetThrowableClass(Pair.Key); 
				AC_ThrowingWeapon* SpawnedThrowingWeapon = GetWorld()->SpawnActor<AC_ThrowingWeapon>(ThrowableWeaponClass, SpawnParams);
				SpawnedThrowingWeapon->SetItemStack(1);
				SpawnedThrowingWeapon->MoveToInven(Enemy, SpawnedThrowingWeapon->GetItemCurStack());
			}
		}

		// Consumable Item 초기화 (구급상자 2개 / 붕대 5개)
		const FName FirstAidKitName = AC_ConsumableItem::GetConsumableItemName(EConsumableItemType::FIRST_AID_KIT);
		const FName BandageName = AC_ConsumableItem::GetConsumableItemName(EConsumableItemType::BANDAGE);

		if (AC_Item* FirstAidKit = InvenComponent->FindMyItemByName(FirstAidKitName))
			FirstAidKit->SetItemStack(2);
		else
		{
			TSubclassOf<AC_ConsumableItem> FirstAidKitClass = Enemy->GetItemSpawnerHelper()->GetConsumableItemClass(EConsumableItemType::FIRST_AID_KIT);
			AC_ConsumableItem* SpawnedFirstAidKit = GetWorld()->SpawnActor<AC_ConsumableItem>(FirstAidKitClass, SpawnParams);
			SpawnedFirstAidKit->SetItemStack(2);
			SpawnedFirstAidKit->MoveToInven(Enemy, SpawnedFirstAidKit->GetItemCurStack());
		}

		if (AC_Item* Bandage = InvenComponent->FindMyItemByName(BandageName))
			Bandage->SetItemStack(5);
		else
		{
			TSubclassOf<AC_ConsumableItem> BandageClass = Enemy->GetItemSpawnerHelper()->GetConsumableItemClass(EConsumableItemType::BANDAGE);
			AC_ConsumableItem* SpawnedBandage = GetWorld()->SpawnActor<AC_ConsumableItem>(BandageClass, SpawnParams);
			SpawnedBandage->SetItemStack(5);
			SpawnedBandage->MoveToInven(Enemy, SpawnedBandage->GetItemCurStack());
		}

		++Index;
	}

	// Test Print
	/*for (AC_Enemy* Enemy : VersusAIEnemies)
	{
		const FColor Color = FColor::MakeRandomColor();

		UC_Util::Print("======================", Color, 20.f);
		UC_Util::Print("Equipment Status : ", Color, 20.f);
		if (AC_EquipableItem* Vest = Enemy->GetInvenComponent()->GetSlotEquipment(EEquipSlot::VEST))
			UC_Util::Print("Vest Durability : " + FString::SanitizeFloat(Vest->GetCurDurabilityRate() * 100.f), Color, 20.f);

		if (AC_EquipableItem* Helmet = Enemy->GetInvenComponent()->GetSlotEquipment(EEquipSlot::HELMET))
			UC_Util::Print("Helmet Durability : " + FString::SanitizeFloat(Helmet->GetCurDurabilityRate() * 100.f), Color, 20.f);

		AC_Gun* MainGun = Cast<AC_Gun>(Enemy->GetEquippedComponent()->GetWeapons()[EWeaponSlot::MAIN_GUN]);
		AC_Gun* SubGun = Cast<AC_Gun>(Enemy->GetEquippedComponent()->GetWeapons()[EWeaponSlot::SUB_GUN]);
		UC_Util::Print("======================", Color, 20.f);
		UC_Util::Print("Gun Status : ", Color, 20.f);
		UC_Util::Print("Main Gun Magazine : " + FString::SanitizeFloat(MainGun->GetCurMagazineBulletCount()), Color, 20.f);
		UC_Util::Print("Sub Gun Magazine : " + FString::SanitizeFloat(SubGun->GetCurMagazineBulletCount()), Color, 20.f);

		UC_Util::Print("======================", Color, 20.f);
		UC_Util::Print("Throwable Status : ", Color, 20.f);
		
		// 개수 파악용 TMap
		TMap<EThrowableType, int> PossessThrowableMap =
		{
			{EThrowableType::GRENADE, 0}, {EThrowableType::SMOKE, 0}, {EThrowableType::FLASH_BANG, 0}  
		};

		// Slot에 장착된 Throwable 조사
		const AC_ThrowingWeapon* SlotThrowingWeapon = Cast<AC_ThrowingWeapon>(Enemy->GetEquippedComponent()->GetWeapons()[EWeaponSlot::THROWABLE_WEAPON]);
		if (SlotThrowingWeapon) ++PossessThrowableMap[SlotThrowingWeapon->GetThrowableType()];
			
		// Inven에 있는 Throwable 개수 조사
		for (uint8 i = 0; i < static_cast<uint8>(EThrowableType::MAX); ++i)
		{
			EThrowableType ThrowableType = static_cast<EThrowableType>(i);
			if (AC_Item* Throwable = Enemy->GetInvenComponent()->FindMyItemByName(AC_ThrowingWeapon::GetThrowableItemName(ThrowableType)))
				PossessThrowableMap[ThrowableType] += Throwable->GetItemCurStack();  
		}

		for (const TPair<EThrowableType, int>& Pair : PossessThrowableMap)
			UC_Util::Print("COUNT : " + FString::FromInt(Pair.Value), Color, 20.f);
		
		UC_Util::Print("======================", Color, 20.f);
		UC_Util::Print("Consumable Status : ", Color, 20.f);
		AC_Item* FirstAidKit = Enemy->GetInvenComponent()->FindMyItemByName(AC_ConsumableItem::GetConsumableItemName(EConsumableItemType::FIRST_AID_KIT));
		if (FirstAidKit) UC_Util::Print("FirstAidKit Count : " + FString::FromInt(FirstAidKit->GetItemCurStack()), Color, 20.f);
		
		AC_Item* Bandage = Enemy->GetInvenComponent()->FindMyItemByName(AC_ConsumableItem::GetConsumableItemName(EConsumableItemType::BANDAGE));
		if (Bandage) UC_Util::Print("Bandage Count : " + FString::FromInt(Bandage->GetItemCurStack()), Color, 20.f);
		UC_Util::Print("======================", Color, 20.f);
	}*/
}

void AC_CombatFieldManager::StartEnemyVsEnemyRound()
{
	EnemyCombatFieldManager->SetIsPlaying(true);
	InitEnemyVsEnemyRound();

	// 서로 TargetCharacter로 잡도록 setting
	VersusAIEnemies[0]->GetController<AC_EnemyAIController>()->GetBehaviorComponent()->SetTargetCharacter(VersusAIEnemies[1]);
	VersusAIEnemies[1]->GetController<AC_EnemyAIController>()->GetBehaviorComponent()->SetTargetCharacter(VersusAIEnemies[0]);
}

void AC_CombatFieldManager::StopEnemyVsEnemyRound()
{
	EnemyCombatFieldManager->SetIsPlaying(false);
	InitEnemyVsEnemyRound();

	// 서로의 TargetCharacter 제거
	for (AC_Enemy* Enemy : VersusAIEnemies)
		Enemy->GetController<AC_EnemyAIController>()->GetBehaviorComponent()->SetTargetCharacter(nullptr);
}

bool AC_CombatFieldManager::TryReviveCharacter(AC_BasicCharacter* Character)
{
	if (Character->GetMainState() != EMainState::DEAD) return false;
			
	/* 죽은 Character 리스폰 처리 */
	
	Character->SetMainState(EMainState::IDLE);

	// Dead 처리 역순으로 되돌리기

	// 물리 시뮬레이션 끄기
	Character->GetMesh()->SetSimulatePhysics(false);

	// Root를 다시 Capsule로 변경
	Character->SetRootComponent(Character->GetCapsuleComponent());
	Character->GetMesh()->AttachToComponent(Character->GetCapsuleComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);

	// Capsule 충돌 복원
	Character->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// Mesh 충돌 프로필 복구
	Character->GetMesh()->SetCollisionProfileName(TEXT("CharacterMesh"));

	// Mesh Transform 초기화
	Character->GetMesh()->SetRelativeTransform(CharacterMeshInitialRelativeTransform);

	// MovementComponent 다시 켜기			
	Character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	// 회전 초기화
	//FRotator InitialRotation = FRotator(0.f, Character->GetActorRotation().Yaw, 0.f);
	//Character->SetActorRotation(InitialRotation);

	// AliveCount 업데이트
	int LeftCharacterCount = GAMESCENE_MANAGER->AddOneToCurrentAliveCharacterCount();
	GAMESCENE_MANAGER->GetPlayer()->GetHUDWidget()->GetTimeBoxWidget()->SetAliveCountText(LeftCharacterCount);

	// Default MaxWalkSpeed로 setting
	Character->GetCharacterMovement()->MaxWalkSpeed = 600.f;
	
	// Stand 자세가 아니라면 Stand로 자세 변환 처리 시도
	Character->SetPoseState(Character->GetPoseState(), EPoseState::STAND);

	AC_EnemyAIController* EnemyAIController = Character->GetController<AC_EnemyAIController>();
	if (!IsValid(EnemyAIController)) return true; // Player Revive처리는 여기서 끝

	EnemyAIController->GetPerceptionComponent()->Activate();
	EnemyAIController->GetPerceptionComponent()->SetComponentTickEnabled(true);
	
	// BrainComponent 재시작
	if (!IsValid(EnemyAIController->GetBrainComponent()))
		UC_Util::Print("From AC_CombatFieldManager::RestartEnemyVsEnemyRound : Invalid BrainComponent", FColor::Red, 10.f);
	else EnemyAIController->GetBrainComponent()->RestartLogic();
	
	return true;
}