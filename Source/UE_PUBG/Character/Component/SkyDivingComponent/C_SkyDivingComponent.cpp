// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/SkyDivingComponent/C_SkyDivingComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Character/C_BasicCharacter.h"
#include "Utility/C_Util.h"

#include "Singleton/C_GameSceneManager.h"
#include "Airplane/C_AirplaneManager.h"
#include "Airplane/C_Airplane.h"
#include "Character/Component/C_InvenComponent.h"
#include "Item/Equipment/C_EquipableItem.h"
#include "Singleton/C_GameInstance.h"

const FName UC_SkyDivingComponent::PARABACKPACK_SOCKET_NAME = "ParachuteBackPackSocket";
const FName UC_SkyDivingComponent::PARACHUTE_SOCKET_NAME    = "ParachuteSocket";

// 원작 수치
//const float PARACHUTE_DEPLOY_LIMIT_HEIGHT = 43000.f;
//const float MAX_SKYDIVE_JUMP_ALTITUDE = 155000.f; // 원작 기준 8 x 8 맵 1.5km 상공 맥시멈에서 뛰어내림

//const float MAX_SKYDIVE_JUMP_ALTITUDE		= 155000.f; // 원작 기준 8 x 8 맵 1.5km 상공 맥시멈에서 뛰어내림

const TMap<ELevelType, FSkyDivingData> UC_SkyDivingComponent::SkyDivingDataMap =
{
	{ELevelType::ShantyTown, {4000.f, 10000.f, 1e9}},
	{ELevelType::TrainingGround, {0.f, 6000.f, 1e9}}
};


UC_SkyDivingComponent::UC_SkyDivingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	ParachuteBackpackStaticMeshComponent	= CreateDefaultSubobject<UStaticMeshComponent>("ParachuteBackpackStaticMeshComponent");
	ParachuteSkeletalMeshComponent			= CreateDefaultSubobject<USkeletalMeshComponent>("ParachuteSkeletalMeshComponent");
}

void UC_SkyDivingComponent::BeginPlay()
{
	Super::BeginPlay();

	GameInstance = Cast<UC_GameInstance>(OwnerCharacter->GetGameInstance());
	if (!GameInstance) UC_Util::Print("From UC_SkyDivingComponent::BeginPlay : Invalid GameInstance", FColor::Red, 10.f);

	if (ParachuteBackpackStaticMeshComponent)
	{
		ParachuteBackpackStaticMeshComponent->AttachToComponent
		(
			OwnerCharacter->GetMesh(),
			FAttachmentTransformRules(EAttachmentRule::KeepRelative, true),
			PARABACKPACK_SOCKET_NAME
		);

		ParachuteBackpackStaticMeshComponent->SetVisibility(false);
	}
	else UC_Util::Print("From SkyDivingComponent::BeginPlay : ParachuteBackpack SM nullptr!", FColor::Red, 10.f);
	
	if (ParachuteSkeletalMeshComponent)
	{
		ParachuteSkeletalMeshComponent->AttachToComponent
		(
			OwnerCharacter->GetMesh(),
			FAttachmentTransformRules(EAttachmentRule::KeepRelative, true),
			PARACHUTE_SOCKET_NAME
		);

		ParachuteSkeletalMeshComponent->SetVisibility(false);
	}
	else UC_Util::Print("From SkyDivingComponent::BeginPlay : Parachute SM nullptr!", FColor::Red, 10.f);
}

void UC_SkyDivingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateCurrentHeight();
	HandleStateTransitionByHeight();

	// if (OwnerPlayer) UC_Util::Print("Speed : " + FString::SanitizeFloat(OwnerPlayer->GetVelocity().Length()));
}

bool UC_SkyDivingComponent::SetSkyDivingState(ESkyDivingState InSkyDivingState)
{
	if (InSkyDivingState == ESkyDivingState::MAX)
	{
		UC_Util::Print("From UC_SkyDivingComponent::SetSkyDivingState : InState Max!", FColor::Red, 10.f);
		return false;
	}

	switch (InSkyDivingState)
	{
	case ESkyDivingState::READY:
		OwnerCharacter->SetActorHiddenInGame(true);
		OwnerCharacter->GetCharacterMovement()->GravityScale = 0.f;
		SkyDivingState = InSkyDivingState;
		return true;
	case ESkyDivingState::SKYDIVING:
	{
		// 비행기가 존재하는 맵일 때, SkyDiving 가능한 상황인지 체크
		if (GAMESCENE_MANAGER->GetAirplaneManager())
		{
			if (!GAMESCENE_MANAGER->GetAirplaneManager()->GetCanDive())
			{
				UC_Util::Print("Cannot SkyDive at the current pos");
				return false;
			}

			OwnerCharacter->SetActorLocation(GAMESCENE_MANAGER->GetAirplaneManager()->GetAirplane()->GetActorLocation());
		}
		
		OwnerCharacter->SetCanMove(true);

		SkyDivingState = InSkyDivingState;

		OwnerCharacter->SetActorHiddenInGame(false);
		ParachuteBackpackStaticMeshComponent->SetVisibility(true);

		// 낙하산 재사용 가능성 때문에 다시금 AttachToComponent 처리 해줌
		if (ParachuteSkeletalMeshComponent)
		{
			ParachuteSkeletalMeshComponent->SetRelativeTransform(FTransform::Identity);
			ParachuteSkeletalMeshComponent->AttachToComponent
			(
				OwnerCharacter->GetMesh(),
				FAttachmentTransformRules(EAttachmentRule::KeepRelative, true),
				PARACHUTE_SOCKET_NAME
			);

			ParachuteSkeletalMeshComponent->SetVisibility(false);
		}
		
		SetStateToSkyDivingState(); // Primitive operation
		return true;
	}
	case ESkyDivingState::PARACHUTING:
	{
		ParachuteSkeletalMeshComponent->SetVisibility(true);
			
		SkyDivingState = ESkyDivingState::PARACHUTING;

		OwnerCharacter->PlayAnimMontage(DeployParachuteMontage);
		ParachuteSkeletalMeshComponent->GetAnimInstance()->Montage_Play(ParachuteDeployMontage);

		// Set CollisionParams for Height estimation LineTrace
		ParachutingHeightCheckCollisionParams = {};
		ParachutingHeightCheckCollisionParams.AddIgnoredActor(OwnerCharacter);
		
		if (GAMESCENE_MANAGER->GetAirplaneManager())
			ParachutingHeightCheckCollisionParams.AddIgnoredActor(GAMESCENE_MANAGER->GetAirplaneManager()->GetAirplane());

		SetStateToParachutingState(); // Primitive operation
		return true;
	}
	case ESkyDivingState::LANDING:
	{
		// 낙하산 animation
		//ParachuteSkeletalMesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
		ParachuteSkeletalMeshComponent->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
		ParachuteSkeletalMeshComponent->GetAnimInstance()->Montage_Play(ParachuteEjectMontage);

		// 가방 숨기기
		ParachuteBackpackStaticMeshComponent->SetVisibility(false);

		// Equipment 배낭을 메고 있었다면 다시 HiddenInGame 꺼주기
		if (AC_EquipableItem* BackPack = OwnerCharacter->GetInvenComponent()->GetEquipmentItems()[EEquipSlot::BACKPACK])
			BackPack->SetActorHiddenInGame(false);

		OwnerCharacter->PlayAnimMontage(LandingMontage);
		SkyDivingState = ESkyDivingState::LANDING;

		SetStateToLandingState(); // Primitive operation
		return true;
	}
	default: return false;
	}
}

void UC_SkyDivingComponent::OnCharacterLandedOnWater()
{
	ParachuteSkeletalMeshComponent->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	ParachuteSkeletalMeshComponent->GetAnimInstance()->Montage_Play(ParachuteEjectMontage);

	ParachuteBackpackStaticMeshComponent->SetVisibility(false);

	UCharacterMovementComponent* OwnerMovement = OwnerCharacter->GetCharacterMovement();
	OwnerMovement->AirControl					= 0.f;
	OwnerMovement->GravityScale					= 1.f;
	OwnerMovement->BrakingDecelerationFalling	= 0.f;
	OwnerMovement->Velocity.Z					= 0.f;

	OwnerCharacter->SetMainState(EMainState::IDLE);
	SkyDivingState = ESkyDivingState::LANDING;
}

void UC_SkyDivingComponent::OnDeployParachuteMontageEnd()
{
	//ParachuteSkeletalMesh->SetVisibility(true);
	//VelocityZLerpDest = State_DivingSpeeds[SkyDivingState].BackKeyZSpeed;
	//SkyDivingState = ESkyDivingState::PARACHUTING;
}

void UC_SkyDivingComponent::OnLandingMontageEnd()
{
	UCharacterMovementComponent* OwnerMovement = OwnerCharacter->GetCharacterMovement();
	OwnerMovement->AirControl   = 0.f;
	OwnerMovement->GravityScale = 1.f;
	OwnerMovement->BrakingDecelerationFalling = 0.f;
	
	OwnerMovement->Velocity.Z = 0.f;
	OwnerCharacter->SetMainState(EMainState::IDLE);
}

void UC_SkyDivingComponent::OnParachuteLandingMontageEnd()
{
	ParachuteSkeletalMeshComponent->SetVisibility(false);
}

void UC_SkyDivingComponent::HandleStateTransitionByHeight()
{
	switch (SkyDivingState)
	{
	case ESkyDivingState::SKYDIVING:
	{
		if (CurrentHeight < SkyDivingDataMap[GameInstance->GetCurrentSelectedLevelType()].ParachuteDeployLimitHeight)
			SetSkyDivingState(ESkyDivingState::PARACHUTING);
		return;
	}
	case ESkyDivingState::PARACHUTING:
		if (!OwnerCharacter->GetCharacterMovement()->IsFalling())
			SetSkyDivingState(ESkyDivingState::LANDING);
		return;
	default: return;
	}
}

void UC_SkyDivingComponent::UpdateCurrentHeight()
{
	switch (SkyDivingState)
	{
	case ESkyDivingState::SKYDIVING:
		CurrentHeight = OwnerCharacter->GetActorLocation().Z - SkyDivingDataMap[GameInstance->GetCurrentSelectedLevelType()].AltitudeZeroZ;
		return;
	case ESkyDivingState::PARACHUTING:
	{
		FHitResult HitResult{};

		FVector StartLocation = OwnerCharacter->GetActorLocation();
		FVector DestLocation  = StartLocation - FVector::UnitZ() * SkyDivingDataMap[GameInstance->GetCurrentSelectedLevelType()].MaxSkyDiveJumpAltitude;

		bool HasHit = GetWorld()->LineTraceSingleByChannel
		(
			HitResult,
			StartLocation, DestLocation,
			ECollisionChannel::ECC_Visibility,
			ParachutingHeightCheckCollisionParams
		);
		
		if (!HasHit) return;

		// Update CurrentHeight
		CurrentHeight = HitResult.Distance;
		return;
	}
	default: return;
	}
}



