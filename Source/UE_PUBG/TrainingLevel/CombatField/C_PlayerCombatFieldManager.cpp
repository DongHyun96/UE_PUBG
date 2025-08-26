// Fill out your copyright notice in the Description page of Project Settings.


#include "C_PlayerCombatFieldManager.h"

#include "C_CombatFieldManager.h"
#include "C_PlayerCombatFieldWidget.h"
#include "Character/C_Enemy.h"
#include "Character/C_Player.h"
#include "Character/Component/C_EquippedComponent.h"
#include "Character/Component/C_InputComponent.h"
#include "Character/Component/C_InvenComponent.h"
#include "Character/Component/EnemyComponent/C_DefaultItemSpawnerComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/ShapeComponent.h"
#include "Door/C_TutorialGate.h"
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

	/* PlayerVsEnemySpawnTransform 저장 */
	
	const UCapsuleComponent* VersusPlayerSpawnCapsuleComponent =
		Cast<UCapsuleComponent>(OwnerCombatFieldManager->GetDefaultSubobjectByName(TEXT("VersusPlayerSpawnTransform")));
	
	if (!VersusPlayerSpawnCapsuleComponent) UC_Util::Print("From AC_PlayerCombatFieldManager::BeginPlay : VersusPlayerSpawnCapsuleComponent missing!", FColor::Red, 10.f);
	else SpawnTransforms.Add(VersusPlayerSpawnCapsuleComponent->GetComponentTransform());

	if (CombatFieldEnemy)
	{
		SpawnTransforms.Add(CombatFieldEnemy->GetActorTransform());
		if (CombatFieldEnemy->GetBehaviorType() != EEnemyBehaviorType::CombatTest)
			UC_Util::Print("From AC_CombatFieldManager::BeginPlay : VersusPlayerEnemy Behavior Type is not COMBAT", FColor::Red, 10.f);
	}

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
		
	/*
	 * Player Inven check -> Combat에 필요한 아이템으로 초기화
	 */

	AC_Player* Player = GAMESCENE_MANAGER->GetPlayer();

	// Combat에 필요한 아이템으로 Player 아이템 초기화
	Player->GetInvenComponent()->ClearInventory();
	Player->GetEquippedComponent()->ClearEquippedWeapons();
	CombatFieldEnemy->GetItemSpawnerHelper()->SpawnDefaultWeaponsAndItemsForCombatFieldCharacter(Player);
	
	// Player Enemy Start 위치로 초기화
	
	// Player 특정 input들(공격 처리, 이동 등) Round Start 이전까지 막기
	// Round Start Timer 초기화 및 초 세기 (UI 띄우기)

	return true;
}
