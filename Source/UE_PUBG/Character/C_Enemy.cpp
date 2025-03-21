// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/C_Enemy.h"

#include "C_Player.h"

#include "AI/C_EnemyAIController.h"
#include "GameFramework/CharacterMovementComponent.h"


#include "Character/Component/C_SwimmingComponent.h"
#include "Character/Component/C_PoseColliderHandlerComponent.h"
#include "Component/EnemyComponent/C_DefaultItemSpawnerComponent.h"
#include "Component/EnemyComponent/C_TargetLocationSettingHelper.h"
#include "Component/SkyDivingComponent/C_EnemySkyDivingComponent.h"



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

	// GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetCharacterMovement()->MaxWalkSpeed = 300.f;

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

			if (bIsActivatingConsumableItem) return false; // TODO : 일어설 수 없습니다 UI 띄우기
			if (!PoseColliderHandlerComponent->CanChangePoseOnCurrentSurroundEnvironment(EPoseState::CROUCH)) return false;
			
			ExecutePoseTransitionAction(GetPoseTransitionMontagesByHandState(HandState).CrawlToCrouch, EPoseState::CROUCH);

			return true;

		case EPoseState::POSE_MAX: default: return false;
		}
	case EPoseState::CRAWL:
		switch (InChangeFrom)
		{
		case EPoseState::STAND: // Stand to Crawl

			if (bIsActivatingConsumableItem) return false; // TODO : 없드릴 수 없습니다 UI 띄우기
			if (!PoseColliderHandlerComponent->CanChangePoseOnCurrentSurroundEnvironment(EPoseState::CRAWL)) return false;

			ExecutePoseTransitionAction(GetPoseTransitionMontagesByHandState(HandState).StandToCrawl, EPoseState::CRAWL);

			return true;

		case EPoseState::CROUCH: // Crouch to Crawl

			if (bIsActivatingConsumableItem) return false; // TODO : 없드릴 수 없습니다 UI 띄우기
			if (!PoseColliderHandlerComponent->CanChangePoseOnCurrentSurroundEnvironment(EPoseState::CRAWL)) return false;

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
	// 실제 CapsuleComponent의 HalfHeight는 88 -> but PIE에서 ActorLocation Z는 90이 나와서 90 상수값 적용
	static const float ACTOR_HALF_Z = 90.f; 
	SetActorLocation(BottomLocation + FVector::UnitZ() * ACTOR_HALF_Z, false, nullptr, TeleportType);	
}



