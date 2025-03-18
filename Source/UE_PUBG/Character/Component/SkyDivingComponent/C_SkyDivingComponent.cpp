// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/SkyDivingComponent/C_SkyDivingComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"

#include "GameFramework/SpringArmComponent.h"

#include "HUD/C_HUDWidget.h"
#include "HUD/C_SkyDiveWidget.h"
#include "HUD/C_InstructionWidget.h"

#include "Utility/C_Util.h"

#include "Singleton/C_GameSceneManager.h"
#include "Airplane/C_AirplaneManager.h"
#include "Airplane/C_Airplane.h"

const FName UC_SkyDivingComponent::PARABACKPACK_SOCKET_NAME = "ParachuteBackPackSocket";
const FName UC_SkyDivingComponent::PARACHUTE_SOCKET_NAME    = "ParachuteSocket";

// SKYDIVING 상태에서 고도를 측정할 때 고도 0m 기준으로 잡을 위치
const float UC_SkyDivingComponent::ALTITUDE_ZERO_Z = 4000.f;

// 원작 수치
//const float PARACHUTE_DEPLOY_LIMIT_HEIGHT = 43000.f;
//const float MAX_SKYDIVE_JUMP_ALTITUDE = 155000.f; // 원작 기준 8 x 8 맵 1.5km 상공 맥시멈에서 뛰어내림

const float UC_SkyDivingComponent::PARACHUTE_DEPLOY_LIMIT_HEIGHT = 10000.f;
//const float MAX_SKYDIVE_JUMP_ALTITUDE		= 155000.f; // 원작 기준 8 x 8 맵 1.5km 상공 맥시멈에서 뛰어내림
const float UC_SkyDivingComponent::MAX_SKYDIVE_JUMP_ALTITUDE = 1e9; // Testing 값

UC_SkyDivingComponent::UC_SkyDivingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	ParachuteBackpackStaticMeshComponent	= CreateDefaultSubobject<UStaticMeshComponent>("ParachuteBackpackStaticMeshComponent");
	ParachuteSkeletalMeshComponent			= CreateDefaultSubobject<USkeletalMeshComponent>("ParachuteSkeletalMeshComponent");
}

void UC_SkyDivingComponent::BeginPlay()
{
	Super::BeginPlay();

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

// TODO : Template method으로 구멍 뚫어줄까 그냥 생각 중
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
		// SkyDiving 가능한지 체크
		if (!GAMESCENE_MANAGER->GetAirplaneManager()->GetCanDive())
		{
			UC_Util::Print("Cannot SkyDive at the current pos");
			return false;
		}

		OwnerCharacter->SetActorLocation(GAMESCENE_MANAGER->GetAirplaneManager()->GetAirplane()->GetActorLocation());
		
		OwnerCharacter->SetCanMove(true);

		SkyDivingState = InSkyDivingState;

		OwnerCharacter->SetActorHiddenInGame(false);
		ParachuteBackpackStaticMeshComponent->SetVisibility(true);
		
		SetStateToSkyDivingState(); // Template method
		return true;
	}
	case ESkyDivingState::PARACHUTING:
	{
		ParachuteSkeletalMeshComponent->SetVisibility(true);
			
		SkyDivingState = ESkyDivingState::PARACHUTING;

		OwnerCharacter->PlayAnimMontage(DeployParachuteMontage);
		ParachuteSkeletalMeshComponent->GetAnimInstance()->Montage_Play(ParachuteDeployMontage);

		SetStateToParachutingState(); // Template method
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

		OwnerCharacter->PlayAnimMontage(LandingMontage);
		SkyDivingState = ESkyDivingState::LANDING;

		SetStateToLandingState(); // Template method
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
		if (CurrentHeight < PARACHUTE_DEPLOY_LIMIT_HEIGHT)
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
		CurrentHeight = OwnerCharacter->GetActorLocation().Z - ALTITUDE_ZERO_Z;
		return;
	case ESkyDivingState::PARACHUTING:
	{
		FCollisionQueryParams CollisionParams{};
		CollisionParams.AddIgnoredActor(OwnerCharacter);

		CollisionParams.AddIgnoredActor(GAMESCENE_MANAGER->GetAirplaneManager()->GetAirplane());

		FHitResult HitResult{};

		FVector StartLocation = OwnerCharacter->GetActorLocation();
		FVector DestLocation  = StartLocation - FVector::UnitZ() * MAX_SKYDIVE_JUMP_ALTITUDE;

		bool HasHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, DestLocation, ECollisionChannel::ECC_Visibility, CollisionParams);
		if (!HasHit) return;

		// Update CurrentHeight
		CurrentHeight = HitResult.Distance;
		return;
	}
	default: return;
	}
}



