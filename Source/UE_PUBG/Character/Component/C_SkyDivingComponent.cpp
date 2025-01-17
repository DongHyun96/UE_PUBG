// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/C_SkyDivingComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"

#include "GameFramework/SpringArmComponent.h"

#include "Character/Component/C_SwimmingComponent.h"
#include "HUD/C_HUDWidget.h"
#include "HUD/C_SkyDiveWidget.h"
#include "HUD/C_MainMapWidget.h"
#include "HUD/C_InstructionWidget.h"

#include "Utility/C_Util.h"

#include "Singleton/C_GameSceneManager.h"
#include "Airplane/C_AirplaneManager.h"
#include "Airplane/C_Airplane.h"

const FName UC_SkyDivingComponent::PARABACKPACK_SOCKET_NAME = "ParachuteBackPackSocket";
const FName UC_SkyDivingComponent::PARACHUTE_SOCKET_NAME    = "ParachuteSocket";

const float UC_SkyDivingComponent::PLAYER_READY_MAINCAM_ARMLENGTH		= 3000.f; // 비행기 안에 탑승했을 때의 ArmLength
const float UC_SkyDivingComponent::PLAYER_PARACHUTE_MAINCAM_ARMLENGTH	= 600.f;
const float UC_SkyDivingComponent::PLAYER_SKYDIVE_MAINCAM_ARMLENGTH		= 400.f;
const float UC_SkyDivingComponent::PLAYER_ORIGIN_MAINCAM_ARMLENGTH		= 220.f;

// 원작 수치
//const float PARACHUTE_DEPLOY_LIMIT_HEIGHT = 43000.f;
//const float MAX_SKYDIVE_JUMP_ALTITUDE = 155000.f; // 원작 기준 8 x 8 맵 1.5km 상공 맥시멈에서 뛰어내림

const float UC_SkyDivingComponent::PARACHUTE_DEPLOY_LIMIT_HEIGHT = 10000.f;
//const float MAX_SKYDIVE_JUMP_ALTITUDE		= 155000.f; // 원작 기준 8 x 8 맵 1.5km 상공 맥시멈에서 뛰어내림
const float UC_SkyDivingComponent::MAX_SKYDIVE_JUMP_ALTITUDE = 1e9; // Testing 값

UC_SkyDivingComponent::UC_SkyDivingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	ParachuteBackpackStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("ParachuteBackpackStaticMeshComponent");
	ParachuteSkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("ParachuteSkeletalMeshComponent");
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

	// OwnerPlayer로 Casting 시도
	OwnerPlayer = Cast<AC_Player>(OwnerCharacter);
}

void UC_SkyDivingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateCurrentHeight();
	UpdatePlayerSkyDiveHUD();

	LerpPlayerMainCameraArmLength(DeltaTime);
	LerpVelocityZ(DeltaTime);

	HandleStateTransitionByHeight();

	if (!IsValid(ParachuteSkeletalMeshComponent) || !IsValid(ParachuteBackpackStaticMeshComponent)) UC_Util::Print("WARNING");

	

	/*switch (SkyDivingState)
	{
	case ESkyDivingState::READY:
		UC_Util::Print("Ready");
		break;
	case ESkyDivingState::SKYDIVING:
		UC_Util::Print("SkyDiving");
		break;
	case ESkyDivingState::PARACHUTING:
		UC_Util::Print("Parachuting");
		break;
	case ESkyDivingState::LANDING:
		break;
	case ESkyDivingState::MAX:
		break;
	default:
		break;
	}*/
	//UC_Util::Print(VelocityZLerpDest);
	//UC_Util::Print(OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed);
	//UC_Util::Print(OwnerCharacter->GetCharacterMovement()->Velocity);
}

void UC_SkyDivingComponent::HandlePlayerMovement(const FVector2D& MovementVector)
{
	switch (SkyDivingState)
	{
	case ESkyDivingState::READY: case ESkyDivingState::LANDING: case ESkyDivingState::MAX: return;
	}

	if (MovementVector.X == -1)  // 뒷방향 input 낙하속도 적절히 조정
	{
		OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = 0.f;
		//VelocityZLerpDest = SKYDIVE_BACKKEY_ZSPEED;
		VelocityZLerpDest = State_DivingSpeeds[SkyDivingState].BackKeyZSpeed;

		OwnerCharacter->SetNextSpeed(0.f);
		return;
	}
	
	if (!OwnerCharacter->Controller) return;

	bool HoldingDirection = OwnerCharacter->GetIsHoldDirection() || OwnerCharacter->GetIsAltPressed();

	FRotator Rotation = (!HoldingDirection) ? OwnerCharacter->GetController()->GetControlRotation() :
									          OwnerCharacter->GetActorRotation();
	Rotation.Roll = 0.f;

	UCharacterMovementComponent* OwnerMovement = OwnerCharacter->GetCharacterMovement();

	// Update MaxWalkSpeed & 중력 또한 Pitch에 따라 조정처리
	if (!HoldingDirection)
	{
		if (270.f <= Rotation.Pitch)
		{
			OwnerMovement->MaxWalkSpeed = FMath::GetMappedRangeValueClamped(FVector2D(360.f, 270.f), State_DivingSpeeds[SkyDivingState].GetMaxWalkSpeedMaxMin(), Rotation.Pitch);
			VelocityZLerpDest           = FMath::GetMappedRangeValueClamped(FVector2D(360.f, 270.f), State_DivingSpeeds[SkyDivingState].GetZSpeedMinMax(), Rotation.Pitch);
		}
		else
		{
			OwnerMovement->MaxWalkSpeed = State_DivingSpeeds[SkyDivingState].MaxWalkSpeed_Max;
			VelocityZLerpDest = State_DivingSpeeds[SkyDivingState].ZSpeedMin;
		}
	}

	//UC_Util::Print(FVector2D(OwnerMovement->MaxWalkSpeed, OwnerMovement->GravityScale));

	const FVector ForwardDirection = FRotationMatrix(Rotation).GetUnitAxis(EAxis::X);
	const FVector   RightDirection = FRotationMatrix(Rotation).GetUnitAxis(EAxis::Y);

	OwnerCharacter->AddMovementInput(ForwardDirection, MovementVector.X);
	OwnerCharacter->AddMovementInput(RightDirection, MovementVector.Y);

	// Pitch에 따른 NextSpeed 조정으로 떨어지는 animation 다르게 주기 & 중력 조절
	// Pitch 0 ~ 89.99 -> 윗 방향 / 360 ~ 270 아랫방향

	if (HoldingDirection) return; // HoldDirection 중이라면 return (HoldDirection 이전에 조정한 값으로 계속 유지)

	float AnimNextSpeed = (270.f <= Rotation.Pitch) ? 
		                  FMath::GetMappedRangeValueClamped(FVector2D(360.f, 270.f), FVector2D(2000.f, 4000.f), Rotation.Pitch) :
						  2000.f;

	OwnerCharacter->SetNextSpeed(AnimNextSpeed); // AnimCharacter에서 Speed Lerp할 값 setting
}

void UC_SkyDivingComponent::OnSkyMoveEnd()
{
	if (SkyDivingState == ESkyDivingState::SKYDIVING)
		VelocityZLerpDest = State_DivingSpeeds[SkyDivingState].ZSpeedMin;
}

void UC_SkyDivingComponent::SetSkyDivingState(ESkyDivingState InSkyDivingState)
{
	switch (InSkyDivingState)
	{
	case ESkyDivingState::READY:
		OwnerCharacter->SetActorHiddenInGame(true);
		OwnerCharacter->GetCharacterMovement()->GravityScale = 0.f;
		SkyDivingState = InSkyDivingState;
		return;
	case ESkyDivingState::SKYDIVING:
	{
		// SkyDiving 가능한지 체크
		if (!GAMESCENE_MANAGER->GetAirplaneManager()->GetCanDive())
		{
			UC_Util::Print("Cannot SkyDive at the current pos");
			return;
		}
		
		OwnerCharacter->SetCanMove(true);

		SkyDivingState = InSkyDivingState;

		// TODO : 비행기 위치에 Character spawn(Visibility on) & Player일 경우 MainCamera Player로 다시 잡아주기
		OwnerCharacter->SetActorHiddenInGame(false);
		ParachuteBackpackStaticMeshComponent->SetVisibility(true);

		UCharacterMovementComponent* OwnerMovement = OwnerCharacter->GetCharacterMovement();

		OwnerMovement->AirControl   = 1.f;
		//OwnerMovement->GravityScale = 0.5f;
		VelocityZLerpDest = State_DivingSpeeds[SkyDivingState].BackKeyZSpeed;

		OwnerMovement->BrakingDecelerationFalling = 2048.f;

		//OwnerMovement->AirControlBoostMultiplier = 10.0f;
		//OwnerMovement->MaxFlySpeed = 1000.f;
		//OwnerMovement->BrakingDecelerationFlying = 2048.f;

		// SkyDive Widget Jumped고도 잡아주기
		if (OwnerPlayer)
		{
			UpdateCurrentHeight();

			UC_SkyDiveWidget* PlayerSkyDiveWidget = OwnerPlayer->GetHUDWidget()->GetSkyDiveWidget();

			PlayerSkyDiveWidget->SetVisibility(ESlateVisibility::HitTestInvisible);

			PlayerSkyDiveWidget->SetJumpedAltitude(CurrentHeight);

			// TODO: 맵에 따른 parachute Limit 고도 설정(SkyDiveWidget에서 사용)
			PlayerSkyDiveWidget->SetParachuteLimitAltitude(PARACHUTE_DEPLOY_LIMIT_HEIGHT);

			// Player 마커 MainMap에 다시 표시하기
			//UC_MainMapWidget* PlayerMainMapWidget = OwnerPlayer->GetHUDWidget()->GetMainMapWidget();
			//PlayerMainMapWidget->TogglePlayerMarkerImageVisibility(true);

			// Instruction Key HUD 업데이트
			UC_InstructionWidget* InstructionWidget = OwnerPlayer->GetHUDWidget()->GetInstructionWidget();
			InstructionWidget->ToggleDeployParachuteInstructionVisibility(true);
			InstructionWidget->ToggleEjectInstructionVisibility(false);
		}

		return;
	}
	case ESkyDivingState::PARACHUTING:
	{
		ParachuteSkeletalMeshComponent->SetVisibility(true);
		VelocityZLerpDest = State_DivingSpeeds[SkyDivingState].BackKeyZSpeed;
		SkyDivingState = ESkyDivingState::PARACHUTING;
		OwnerCharacter->PlayAnimMontage(DeployParachuteMontage);
		ParachuteSkeletalMeshComponent->GetAnimInstance()->Montage_Play(ParachuteDeployMontage);
		//ParachuteSkeletalMesh->PlayAnimation()

		// Instruction Key HUD 업데이트
		if (OwnerPlayer) OwnerPlayer->GetHUDWidget()->GetInstructionWidget()->ToggleDeployParachuteInstructionVisibility(false);

		return;
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

		// HUD 끄기
		if (OwnerPlayer) OwnerPlayer->GetHUDWidget()->GetSkyDiveWidget()->SetVisibility(ESlateVisibility::Hidden);
		return;
	}
	default: return;
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

	// HUD 끄기
	if (OwnerPlayer) OwnerPlayer->GetHUDWidget()->GetSkyDiveWidget()->SetVisibility(ESlateVisibility::Hidden);
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

void UC_SkyDivingComponent::LerpPlayerMainCameraArmLength(const float& DeltaTime)
{
	if (!OwnerPlayer) return;

	USpringArmComponent* MainCamSpringArm = OwnerPlayer->GetMainSpringArm();

	if (OwnerPlayer->GetMainState() != EMainState::SKYDIVING)
	{
		// Landing한 이후로 TargetArmLength가 원위치 되지 않았을 때의 예외처리
		if (FMath::Abs(MainCamSpringArm->TargetArmLength - PLAYER_ORIGIN_MAINCAM_ARMLENGTH) > 0.1f)
			MainCamSpringArm->TargetArmLength = FMath::Lerp(MainCamSpringArm->TargetArmLength, PLAYER_ORIGIN_MAINCAM_ARMLENGTH, DeltaTime * 5.f);

		return;
	}

	
	switch (SkyDivingState)
	{
	case ESkyDivingState::SKYDIVING:
		MainCamSpringArm->TargetArmLength = FMath::Lerp(MainCamSpringArm->TargetArmLength, PLAYER_SKYDIVE_MAINCAM_ARMLENGTH, DeltaTime * 5.f);
		return;
	case ESkyDivingState::PARACHUTING:
		MainCamSpringArm->TargetArmLength = FMath::Lerp(MainCamSpringArm->TargetArmLength, PLAYER_PARACHUTE_MAINCAM_ARMLENGTH, DeltaTime * 5.f);
		return;
	case ESkyDivingState::LANDING: // TODO : Landing 한 이후로 TargetArmLength 원위치 되어있는지 확인(LANDING 단계에서 AnimMontage로 Landing 동작 재생할 예정)
		MainCamSpringArm->TargetArmLength = FMath::Lerp(MainCamSpringArm->TargetArmLength, PLAYER_ORIGIN_MAINCAM_ARMLENGTH, DeltaTime * 5.f);
		return;
	case ESkyDivingState::READY:
		MainCamSpringArm->TargetArmLength = FMath::Lerp(MainCamSpringArm->TargetArmLength, PLAYER_READY_MAINCAM_ARMLENGTH, DeltaTime * 5.f);
		return;
	case ESkyDivingState::MAX:
	default:
		return;
	}

}

void UC_SkyDivingComponent::LerpVelocityZ(const float& DeltaTime)
{
	if (OwnerCharacter->GetMainState() != EMainState::SKYDIVING) return;

	UCharacterMovementComponent* OwnerMovement = OwnerCharacter->GetCharacterMovement();

	float ZSpeed = OwnerMovement->Velocity.Z;
	ZSpeed = FMath::Lerp(ZSpeed, VelocityZLerpDest, DeltaTime * 10.f);
	OwnerMovement->Velocity.Z = ZSpeed;
}

void UC_SkyDivingComponent::HandleStateTransitionByHeight()
{
	switch (SkyDivingState)
	{
	case ESkyDivingState::SKYDIVING:
	{
		if (CurrentHeight < PARACHUTE_DEPLOY_LIMIT_HEIGHT)
		{
			UC_Util::Print(CurrentHeight, FColor::Red, 10.f);
			SetSkyDivingState(ESkyDivingState::PARACHUTING);
		}
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
	case ESkyDivingState::SKYDIVING: case ESkyDivingState::PARACHUTING:
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
	}
	default: return;
	}
}

void UC_SkyDivingComponent::UpdatePlayerSkyDiveHUD()
{
	// SkyDive HUD Widget 가져오기
	if (!OwnerPlayer) return;

	UC_SkyDiveWidget* SkyDiveWidget = OwnerPlayer->GetHUDWidget()->GetSkyDiveWidget();
	if (!SkyDiveWidget) return;

	// Update Height
	SkyDiveWidget->SetAltitude(CurrentHeight);
}
