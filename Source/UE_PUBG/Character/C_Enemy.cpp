// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/C_Enemy.h"

#include "C_Player.h"
#include "AI/C_BehaviorComponent.h"

#include "AI/C_EnemyAIController.h"
#include "AI/Service/C_BTServiceCombat.h"
#include "GameFramework/CharacterMovementComponent.h"


#include "Character/Component/C_SwimmingComponent.h"
#include "Character/Component/C_PoseColliderHandlerComponent.h"
#include "Component/EnemyComponent/C_DefaultItemSpawnerComponent.h"
#include "Component/EnemyComponent/C_TargetLocationSettingHelper.h"
#include "Component/SkyDivingComponent/C_EnemySkyDivingComponent.h"
#include "Utility/C_Util.h"

const float AC_Enemy::JUMP_VELOCITYZ_ORIGIN = 420.f;

const TMap<EPoseState, float> AC_Enemy::ActorZLocationOffsetFromBottom =
{
	{EPoseState::STAND,		90.f},
	{EPoseState::CROUCH,	69.212f},
	{EPoseState::CRAWL,		22.4f},
};

AC_Enemy::AC_Enemy()
{
	PrimaryActorTick.bCanEverTick = true;
	
	TeamID = ETeamAttitude::Hostile;

	ItemSpawnerHelper = CreateDefaultSubobject<UC_DefaultItemSpawnerComponent>("ItemSpawnerHelper");
	ItemSpawnerHelper->SetOwnerEnemy(this);

	TargetLocationSettingHelper = CreateDefaultSubobject<UC_TargetLocationSettingHelper>("TargetLocationSetter");
	TargetLocationSettingHelper->SetOwnerEnemy(this);
	
	SkyDivingComponent = CreateDefaultSubobject<UC_EnemySkyDivingComponent>("EnemySkyDivingComponent");
	SkyDivingComponent->SetOwnerCharcter(this);
	EnemySkyDivingComponent = Cast<UC_EnemySkyDivingComponent>(SkyDivingComponent);
}

void AC_Enemy::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = 600.f;

    // 비행기 타기 이전에 spawn하는 것으로 수정되었음
    // ItemSpawnerHelper->SpawnDefaultWeaponsAndItems();
}

void AC_Enemy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	/*switch (HandState)
	{
	case EHandState::UNARMED: UC_Util::Print("UNARMED"); break;
	case EHandState::WEAPON_GUN: UC_Util::Print("WEAPON_GUN"); break;
	case EHandState::WEAPON_MELEE: UC_Util::Print("WEAPON_MELEE"); break;
	case EHandState::WEAPON_THROWABLE: UC_Util::Print("WEAPON_THROWABLE"); break;
	case EHandState::HANDSTATE_MAX: 
		break;
	}*/

	//float DistanceToPlayer = FVector::Distance(GAMESCENE_MANAGER->GetPlayer()->GetActorLocation(), this->GetActorLocation());
	//UC_Util::Print(DistanceToPlayer * 0.01f);
	// UC_Util::Print(GetVelocity().Size2D());
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

AC_EnemyAIController* AC_Enemy::GetEnemyAIController() const
{
	return Cast<AC_EnemyAIController>(GetController());
}

void AC_Enemy::SetActorBottomLocation(const FVector& BottomLocation, ETeleportType TeleportType)
{
	SetActorLocation(BottomLocation + FVector::UnitZ() * ActorZLocationOffsetFromBottom[this->PoseState], false, nullptr, TeleportType);	
}

void AC_Enemy::OnTakeDamage(AC_BasicCharacter* DamageCauser)
{
	UC_BehaviorComponent* BehaviorComponent = GetEnemyAIController()->GetBehaviorComponent();
	AC_BasicCharacter* TargetCharacter = BehaviorComponent->GetTargetCharacter();

	UC_Util::Print("OnTakeDamage", FColor::Red, 10.f);

	// Damage를 입힌 사람이 나 자신(자기가 던진 수류탄 등)이거나 nullptr(자기장 등)
	if (DamageCauser == this || DamageCauser == nullptr) return;

	// 이미 누군가를 공격 중이라면
	if (BehaviorComponent->GetServiceType() == EServiceType::COMBAT &&
		BehaviorComponent->GetCombatTaskType() == ECombatTaskType::ATTACK)
		return;

	// Try Update TargetCharacter
	{
		// 현재 TargetCharacter가 존재하지 않다면
		if (!TargetCharacter)
		{
			BehaviorComponent->SetTargetCharacter(DamageCauser);
		}
		
		// Detected Characters 중 Lv1 캐릭터가 한 명도 없을 때
		if (!GetEnemyAIController()->HasAnyCharacterEnteredLevel1SightRange())
		{
			// 바로 TargetCharacter로 지정
			BehaviorComponent->SetTargetCharacter(DamageCauser);
		}

		// Lv1 캐릭터가 있는 상황 -> Damage Causer도 Lv1에 들어오는지 체크
		float DamageCauserDistance = FVector::Distance(DamageCauser->GetActorLocation(), this->GetActorLocation());
		if (DamageCauserDistance < AC_EnemyAIController::GetLv1SightRangeDistance())
		{
			BehaviorComponent->SetTargetCharacter(DamageCauser);
		}

		
	}
	
}

void AC_Enemy::OnPoseTransitionFinish()
{
	UpdateMaxWalkSpeed({1.f, 0.f}); // Forward 방면 기준으로 MaxWalkSpeed 재설정
	Super::OnPoseTransitionFinish();
}
