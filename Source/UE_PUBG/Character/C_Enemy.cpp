// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/C_Enemy.h"

#include "C_Player.h"
#include "AI/C_BehaviorComponent.h"

#include "AI/C_EnemyAIController.h"
#include "AI/Service/C_BTServiceCombat.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


#include "Character/Component/C_SwimmingComponent.h"
#include "Character/Component/C_PoseColliderHandlerComponent.h"
#include "Component/C_EquippedComponent.h"
#include "Component/C_SmokeEnteredChecker.h"
#include "Component/EnemyComponent/C_DefaultItemSpawnerComponent.h"
#include "Component/EnemyComponent/C_TargetLocationSettingHelper.h"
#include "Component/SkyDivingComponent/C_EnemySkyDivingComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "HUD/C_GameOverWidget.h"
#include "HUD/EnemyWidget/C_EnemyHPWidget.h"
#include "Item/Weapon/ThrowingWeapon/C_ThrowingWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"
#include "Singleton/C_GameSceneManager.h"
#include "Utility/C_Util.h"

const float AC_Enemy::JUMP_VELOCITYZ_ORIGIN = 420.f;

const TMap<EPoseState, float> AC_Enemy::ActorZLocationOffsetFromBottom =
{
	{EPoseState::STAND,		90.f},
	{EPoseState::CROUCH,	69.212f},
	{EPoseState::CRAWL,		22.4f},
};

// TODO : 각 BehaviorType에 맞는 BehaviorTree로 초기화할 것
const TMap<EEnemyBehaviorType, FString> AC_Enemy::BehaviorTreeReferenceDirectories =
{
	{EEnemyBehaviorType::InGamePlayable,	"/Script/AIModule.BehaviorTree'/Game/Project_PUBG/Common/AI/BT_Enemy.BT_Enemy'"},
	{EEnemyBehaviorType::MovementTest,		"/Script/AIModule.BehaviorTree'/Game/Project_PUBG/Common/AI/BT_MovementTest.BT_MovementTest'"},
	{EEnemyBehaviorType::StatCareTest,		"/Script/AIModule.BehaviorTree'/Game/Project_PUBG/Common/AI/BT_StatCareTest.BT_StatCareTest'"}, 
	{EEnemyBehaviorType::SkyDivingTest,		"/Script/AIModule.BehaviorTree'/Game/Project_PUBG/Common/AI/BT_SkyDiveTest.BT_SkyDiveTest'"},
	{EEnemyBehaviorType::CombatTest,		"/Script/AIModule.BehaviorTree'/Game/Project_PUBG/Common/AI/BT_CombatTest.BT_CombatTest'"},
};

TMap<EEnemyBehaviorType, UBehaviorTree*> AC_Enemy::BehaviorTrees{};

AC_Enemy::AC_Enemy()
{
	//PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bCanEverTick = false;
	
	TeamID = ETeamAttitude::Hostile;

	ItemSpawnerHelper = CreateDefaultSubobject<UC_DefaultItemSpawnerComponent>("ItemSpawnerHelper");
	ItemSpawnerHelper->SetOwnerEnemy(this);

	TargetLocationSettingHelper = CreateDefaultSubobject<UC_TargetLocationSettingHelper>("TargetLocationSetter");
	TargetLocationSettingHelper->SetOwnerEnemy(this);
	
	SkyDivingComponent = CreateDefaultSubobject<UC_EnemySkyDivingComponent>("EnemySkyDivingComponent");
	SkyDivingComponent->SetOwnerCharacter(this);
	EnemySkyDivingComponent = Cast<UC_EnemySkyDivingComponent>(SkyDivingComponent);

	// Init Behavior Trees if empty
	if (BehaviorTrees.IsEmpty())
	{
		for (uint8 i = 0; i < static_cast<uint8>(EEnemyBehaviorType::Max); ++i)
		{
			EEnemyBehaviorType Type = static_cast<EEnemyBehaviorType>(i);
			const FString& Path = BehaviorTreeReferenceDirectories[Type];
			
			UBehaviorTree* LoadedBehaviorTree = Cast<UBehaviorTree>(StaticLoadObject(UBehaviorTree::StaticClass(), nullptr, *Path));
			if (!LoadedBehaviorTree)
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to load BehaviorTree for index %d"), i);
				continue;
			}
			else UE_LOG(LogTemp, Warning, TEXT("Succeeded to load BehaviorTree for index %d"), i);

			BehaviorTrees.Add(Type, LoadedBehaviorTree);
		}
	}
}

void AC_Enemy::BeginPlay()
{
	Super::BeginPlay();

	if (!BehaviorTree) InitBehaviorTreeBySelfBehaviorType();
	
	GetCharacterMovement()->MaxWalkSpeed = 600.f;

	GetMesh()->SetMaterial(0, GetMesh()->GetMaterial(1));

	// SkyDive Tester 초기 위치로 돌아올 때 회전도 초기 회전으로 주기 위해 저장해 둠
	InitialRotation = GetActorRotation();

	// CombatTester와 SkyDivingTester의 Spectate 카메라 관련 초기화
	if (EnemyBehaviorType == EEnemyBehaviorType::CombatTest || EnemyBehaviorType == EEnemyBehaviorType::SkyDivingTest)
	{
		SpectatorCameraComponent = Cast<UCameraComponent>(GetDefaultSubobjectByName(TEXT("Camera")));
		if (!SpectatorCameraComponent) UC_Util::Print("From AC_Enemy::BeginPlay : Spectator Enemy's Camera init failed!", FColor::Red, 10.f);

		SpectatorSpringArmComponent = Cast<USpringArmComponent>(GetDefaultSubobjectByName(TEXT("SpringArm")));
		if (!SpectatorSpringArmComponent) UC_Util::Print("From AC_Enemy::BeginPlay : Spectator Enemy's SpringArm init failed!", FColor::Red, 10.f);
	}

	EnemyHPBarWidget->SetOwnerEnemy(this);
	if (EnemyBehaviorType != EEnemyBehaviorType::StatCareTest) EnemyHPBarWidget->SetVisibility(ESlateVisibility::Hidden);

	// GetMesh()->GetMaterials()[0];

	/* UDataTable* RandomNameDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Project_PUBG/DongHyun/Character/DataTable/DT_RandomNameTable"));

	if (!RandomNameDataTable)
	{
		UC_Util::Print("RandomNameTable not inited!", FColor::Red, 20.f);
		return;
	}

	TArray<FName> RowNames = RandomNameDataTable->GetRowNames();
	int RandomIndex = FMath::RandRange(0, RowNames.Num() - 1); */

}

void AC_Enemy::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GAMESCENE_MANAGER->GetEnemies().Remove(this);
}

void AC_Enemy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Handling Stat Care testing Enemy HP Bar Widget visibility by distanceToPlayer
	if (EnemyBehaviorType != EEnemyBehaviorType::StatCareTest) return;
	
	float DistanceToPlayer = FVector::Distance(GAMESCENE_MANAGER->GetPlayer()->GetActorLocation(), this->GetActorLocation());
	bool bPlayerInWidgetVisibleDistance = DistanceToPlayer < 3000.f; // 30m 이내에 들어와 있다면 HP Widget 보이기
	EnemyHPBarWidget->SetWidgetRenderOpacityDest(bPlayerInWidgetVisibleDistance ? 1.f : 0.f);
	
	//UC_Util::Print(DistanceToPlayer * 0.01f);
	// UC_Util::Print(GetVelocity().Size2D());
}

void AC_Enemy::InitBehaviorTreeBySelfBehaviorType()
{
	if (BehaviorTrees.IsEmpty())
	{
		UC_Util::Print("From AC_Enemy::InitBehaviorTreeBySelfBehaviorType : BehaviorTress static variable is empty!", FColor::Red, 10.f);
		return;
	}

	BehaviorTree = BehaviorTrees[EnemyBehaviorType];
	if (!IsValid(BehaviorTree)) UC_Util::Print("From AC_Enemy::InitBehaviorTreeBySelfBehaviorType : BehaviorTree initing failed!", FColor::Red, 10.f);
}

void AC_Enemy::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	// Jump Velocity 초기화
	GetCharacterMovement()->JumpZVelocity = JUMP_VELOCITYZ_ORIGIN; 
}

bool AC_Enemy::SetPoseState(EPoseState InChangeFrom, EPoseState InChangeTo)
{
	if (!bCanMove)											return false;
	if (bIsJumping || GetCharacterMovement()->IsFalling())	return false;
	if (InChangeFrom == InChangeTo)							return false;
	if (SwimmingComponent->IsSwimming())					return false;

	switch (InChangeTo)
	{
	case EPoseState::STAND:
		switch (InChangeFrom)
		{
		case EPoseState::CROUCH: // Crouch To Stand (Pose transition 없이 바로 처리)

			if (!PoseColliderHandlerComponent->CanChangePoseOnCurrentSurroundEnvironment(EPoseState::STAND)) return false;

			Super::SetPoseState(EPoseState::STAND);

			if (CharacterSounds->CrouchToStandSound)
				UGameplayStatics::PlaySoundAtLocation(this, CharacterSounds->CrouchToStandSound, this->GetActorLocation());
			
			return true;

		case EPoseState::CRAWL: // Crawl To Stand

			if (bIsActivatingConsumableItem) return false;
			if (!PoseColliderHandlerComponent->CanChangePoseOnCurrentSurroundEnvironment(EPoseState::STAND)) return false;

			if (SwimmingComponent->GetSwimmingState() != ESwimmingState::ON_GROUND)
			{
				Super::SetPoseState(EPoseState::STAND);
				return true;
			}
			UpdateMaxWalkSpeed({0.f, 0.f}); // PoseTransitioning 처리하는 동안 MaxWalkSpeed 0으로 만들기
			ExecutePoseTransitionAction(GetPoseTransitionMontagesByHandState(HandState).CrawlToStand, EPoseState::STAND);
			return true;

		case EPoseState::POSE_MAX: default: return false;
		}
	case EPoseState::CROUCH:
		switch (InChangeFrom)
		{
		case EPoseState::STAND: // Stand To Crouch

			if (!PoseColliderHandlerComponent->CanChangePoseOnCurrentSurroundEnvironment(EPoseState::CROUCH)) return false;
			Super::SetPoseState(EPoseState::CROUCH);

			if (CharacterSounds->StandToCrouchSound)
				UGameplayStatics::PlaySoundAtLocation(this, CharacterSounds->StandToCrouchSound, this->GetActorLocation());
			
			return true;

		case EPoseState::CRAWL: // Crawl To Crouch

			if (bIsActivatingConsumableItem) return false;
			if (!PoseColliderHandlerComponent->CanChangePoseOnCurrentSurroundEnvironment(EPoseState::CROUCH)) return false;
			UpdateMaxWalkSpeed({0.f, 0.f});
			ExecutePoseTransitionAction(GetPoseTransitionMontagesByHandState(HandState).CrawlToCrouch, EPoseState::CROUCH);

			return true;

		case EPoseState::POSE_MAX: default: return false;
		}
	case EPoseState::CRAWL:
		switch (InChangeFrom)
		{
		case EPoseState::STAND: // Stand to Crawl

			if (bIsActivatingConsumableItem) return false;
			if (!PoseColliderHandlerComponent->CanChangePoseOnCurrentSurroundEnvironment(EPoseState::CRAWL)) return false;
			UpdateMaxWalkSpeed({0.f, 0.f});
			ExecutePoseTransitionAction(GetPoseTransitionMontagesByHandState(HandState).StandToCrawl, EPoseState::CRAWL);

			return true;

		case EPoseState::CROUCH: // Crouch to Crawl

			if (bIsActivatingConsumableItem) return false;
			if (!PoseColliderHandlerComponent->CanChangePoseOnCurrentSurroundEnvironment(EPoseState::CRAWL)) return false;
			UpdateMaxWalkSpeed({0.f, 0.f});
			ExecutePoseTransitionAction(GetPoseTransitionMontagesByHandState(HandState).CrouchToCrawl, EPoseState::CRAWL);
			return true;

		case EPoseState::POSE_MAX: default: return false;
		}
	case EPoseState::POSE_MAX: default: return false;
	}
}

void AC_Enemy::SetActorBottomLocation(const FVector& BottomLocation, ETeleportType TeleportType)
{
	SetActorLocation(BottomLocation + FVector::UnitZ() * ActorZLocationOffsetFromBottom[this->PoseState], false, nullptr, TeleportType);	
}

void AC_Enemy::OnTakeDamage(AC_BasicCharacter* DamageCauser)
{
	UC_BehaviorComponent* BehaviorComponent = GetController<AC_EnemyAIController>()->GetBehaviorComponent();
	
	// Damage를 입힌 사람이 나 자신(자기가 던진 수류탄 등)이거나 nullptr(자기장 등)
	if (DamageCauser == this || !DamageCauser) return;

	// 이미 누군가를 공격 중이라면
	if (BehaviorComponent->GetServiceType() == EServiceType::COMBAT)
	{
		// 지금 공격하는 공격 대상이 DamageCauser이고(쌍방 공격 중), 피가 20 아래일 때 40%의 확률로 TakeCover 처리
		if (BehaviorComponent->GetTargetCharacter() == DamageCauser &&
			StatComponent->GetCurHP() < 20.f && FMath::RandRange(0.f, 1.f) < 0.4f)
		{
			UC_Util::Print("Switching to TakeCover Task while attacking!", FColor::MakeRandomColor(), 20.f);
			BehaviorComponent->SetIdleTaskType(EIdleTaskType::TAKE_COVER);
			BehaviorComponent->SetServiceType(EServiceType::IDLE);
		}
		// 아니라면, 지속해서 공격 처리
		return; 
	}

	// TODO : 다른 Tester들에 대한 처리도 해주어야 함
	// Stat-Care Tester용 처리
	if (EnemyBehaviorType == EEnemyBehaviorType::StatCareTest)
	{
		// 플레이어가 아닌 다른 Enemy의 공격을 받은 경우라면 조치 x
		if (DamageCauser == GAMESCENE_MANAGER->GetPlayer())
			BehaviorComponent->SetIdleTaskType(EIdleTaskType::TAKE_COVER);
		return;
	}

	bool bDamageCauserSetToTargetCharacter = TryUpdateTargetCharacterToDamageCauser(DamageCauser);

	// Damage Causer가 TargetCharacter로 제대로 잡히지 않았거나, 잡혔어도 피가 25 아래로 떨어졌다면 엄폐 시도
	if (!bDamageCauserSetToTargetCharacter || StatComponent->GetCurHP() < 25.f)
	{
		BehaviorComponent->SetServiceType(EServiceType::IDLE);
		BehaviorComponent->SetIdleTaskType(EIdleTaskType::TAKE_COVER);
		return;
	}

	// Damage Causer가 제대로 잡혔고, 피가 50 이하일 때 50%의 확률로 TakeCover 처리
	if (StatComponent->GetCurHP() < 50.f && FMath::RandRange(0.f, 1.f) < 0.5f)
	{
		BehaviorComponent->SetServiceType(EServiceType::IDLE);
		BehaviorComponent->SetIdleTaskType(EIdleTaskType::TAKE_COVER);
		return;
	}

	if (SmokeEnteredChecker->IsCurrentlyInSmokeArea()) return;

	// Damage Causer 공격 시도
	BehaviorComponent->SetServiceType(EServiceType::COMBAT);
}

bool AC_Enemy::TryUpdateTargetCharacterToDamageCauser(AC_BasicCharacter* DamageCauser)
{
	UC_BehaviorComponent*	BehaviorComponent	= GetController<AC_EnemyAIController>()->GetBehaviorComponent();
	AC_BasicCharacter*		TargetCharacter		= BehaviorComponent->GetTargetCharacter();

	// 이미 TargetCharacter가 해당 캐릭터로 잡혀 있다면
	if (TargetCharacter == DamageCauser) return true;
	
	// Try Update TargetCharacter.
	// TargetCharacter Setting Priority : 현재 공격중인 TargetCharacter(Priority Max) |
	// 시야에 보이는 Damage Causer | 시야에 보이지 않는 Lv1 Damage Causer | Lv1 | 시야에 보이지 않는 Damage Causer | Lv2 ...
	
	// 현재 TargetCharacter가 존재하지 않다면
	if (!TargetCharacter)
	{
		BehaviorComponent->SetTargetCharacter(DamageCauser);
		return true;
	}

	// 현재 시야에 보이는 Damage Causer
	if (GetController<AC_EnemyAIController>()->IsCurrentlyOnSight(DamageCauser))
	{
		BehaviorComponent->SetTargetCharacter(DamageCauser);
		return true;
	}

	// 시야에 보이지 않는 중
	
	// Detected Characters 중 Lv1 캐릭터가 한 명도 없을 때, DamageCauser로 바로 TargetCharacter 지정
	if (!GetController<AC_EnemyAIController>()->HasAnyCharacterEnteredLevel1SightRange())
	{
		BehaviorComponent->SetTargetCharacter(DamageCauser);
		return true;
	}

	// Lv1 캐릭터가 있는 상황 -> Damage Causer도 Lv1에 들어오는지 체크
	
	float DamageCauserDistance = FVector::Distance(DamageCauser->GetActorLocation(), this->GetActorLocation());
	if (DamageCauserDistance < AC_EnemyAIController::GetLv1SightRangeDistance()) // 시야에 보이지 않는 Lv1 Damage Causer
	{
		BehaviorComponent->SetTargetCharacter(DamageCauser);
		return true;
	}
	return false;
}

void AC_Enemy::OnPoseTransitionFinish()
{
	UpdateMaxWalkSpeed({1.f, 0.f}); // Forward 방면 기준으로 MaxWalkSpeed 재설정
	Super::OnPoseTransitionFinish();
}

void AC_Enemy::CharacterDead(const FKillFeedDescriptor& KillFeedDescriptor)
{
	Super::CharacterDead(KillFeedDescriptor);

	// 속도 0으로 setting
	UpdateMaxWalkSpeed({0.f, 0.f});
	
	AC_EnemyAIController* EnemyAIController = GetController<AC_EnemyAIController>(); 
	EnemyAIController->GetPerceptionComponent()->Deactivate();
	EnemyAIController->GetPerceptionComponent()->SetComponentTickEnabled(false);

	EnemyAIController->GetBrainComponent()->StopLogic("Died");

	// 현재 Level이 Training Ground일 경우, 모든 Enemy가 사망했다고 하더라도 Winner가 없음
	UC_GameInstance* GameInstance = Cast<UC_GameInstance>(GetGameInstance());
	// if (GameInstance->GetCurrentSelectedLevelType() == ELevelType::TrainingGround) return;
	if (GameInstance->GetCurrentSelectedLevelType() != ELevelType::ShantyTown) return;
	
	// Player만 남은 상황이라면 Winner Winner Chicken Dinner UI Sequence 띄우기
	AC_Player* Player = GAMESCENE_MANAGER->GetPlayer();
	if (!IsValid(Player)) return;
	
	if (Ranking == 2 && Player->GetMainState() != EMainState::DEAD)
	{
		GAMESCENE_MANAGER->SetIsGameOver(true);
		Player->GetGameOverWidget()->ActivateWinningSequence();
	}
}

void AC_Enemy::DestroyCharacter()
{
	if (Delegate_OnCombatCharacterDestroy.ExecuteIfBound()) return;
	
	Super::DestroyCharacter();
	
	UC_Util::Print("Destroying Enemy", FColor::Red, 10.f);
	GAMESCENE_MANAGER->GetEnemies().Remove(this);
	this->Destroy();
}
