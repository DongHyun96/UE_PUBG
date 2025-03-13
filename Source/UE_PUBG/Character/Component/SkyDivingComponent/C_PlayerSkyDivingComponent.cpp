// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/SkyDivingComponent/C_PlayerSkyDivingComponent.h"

#include "Character/C_Player.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "HUD/C_HUDWidget.h"
#include "HUD/C_InstructionWidget.h"
#include "HUD/C_SkyDiveWidget.h"
#include "Utility/C_Util.h"

UC_PlayerSkyDivingComponent::UC_PlayerSkyDivingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UC_PlayerSkyDivingComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerPlayer = Cast<AC_Player>(OwnerCharacter);
	if (!IsValid(OwnerPlayer)) UC_Util::Print("From UC_PlayerSkyDivingComponent : OwnerPlayer casting failed!", FColor::Red, 10.f);
}

void UC_PlayerSkyDivingComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdatePlayerSkyDiveHUD(); // CurrentHeight에 따른 HUD 업데이트 처리
	LerpPlayerMainCameraArmLength(DeltaTime);
	LerpVelocityZ(DeltaTime);
}

void UC_PlayerSkyDivingComponent::LerpPlayerMainCameraArmLength(const float& DeltaTime)
{
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
	case ESkyDivingState::MAX: default: return;
	}
}

void UC_PlayerSkyDivingComponent::UpdatePlayerSkyDiveHUD()
{
	if (UC_SkyDiveWidget* SkyDiveWidget = OwnerPlayer->GetHUDWidget()->GetSkyDiveWidget())
		SkyDiveWidget->SetAltitude(CurrentHeight);
	else UC_Util::Print("From UC_PlayerSkyDivingComponent::UpdatePlayerSkyDiveHUD : Cannot get SkyDiveWidget!"); 		
}

void UC_PlayerSkyDivingComponent::LerpVelocityZ(const float& DeltaTime)
{
	if (OwnerCharacter->GetMainState() != EMainState::SKYDIVING) return;

	UCharacterMovementComponent* OwnerMovement = OwnerCharacter->GetCharacterMovement();
	
	float ZSpeed = FMath::Lerp(OwnerMovement->Velocity.Z, VelocityZLerpDest, DeltaTime * 10.f);
	OwnerMovement->Velocity.Z = ZSpeed;
}

void UC_PlayerSkyDivingComponent::SetStateToSkyDivingState()
{
	UCharacterMovementComponent* OwnerMovement = OwnerCharacter->GetCharacterMovement();

	OwnerMovement->AirControl = 1.f;
	//OwnerMovement->GravityScale = 0.5f;
	VelocityZLerpDest = STATE_DIVINGSPEEDS[SkyDivingState].ZSpeedMin;

	OwnerMovement->BrakingDecelerationFalling = 2048.f;

	//OwnerMovement->AirControlBoostMultiplier = 10.0f;
	//OwnerMovement->MaxFlySpeed = 1000.f;
	//OwnerMovement->BrakingDecelerationFlying = 2048.f;

	// SkyDive Widget Jumped 고도 잡아주기 전 Height 업데이트 한 번 처리
	UpdateCurrentHeight();

	UC_SkyDiveWidget* PlayerSkyDiveWidget = OwnerPlayer->GetHUDWidget()->GetSkyDiveWidget();

	PlayerSkyDiveWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
	PlayerSkyDiveWidget->SetJumpedAltitude(CurrentHeight);
	PlayerSkyDiveWidget->SetParachuteLimitAltitude(PARACHUTE_DEPLOY_LIMIT_HEIGHT); // TODO: 맵에 따른 parachute Limit 고도 설정(SkyDiveWidget에서 사용)

	// Player 마커 MainMap에 다시 표시하기
	//UC_MainMapWidget* PlayerMainMapWidget = OwnerPlayer->GetHUDWidget()->GetMainMapWidget();
	//PlayerMainMapWidget->TogglePlayerMarkerImageVisibility(true);

	// Instruction Key HUD 업데이트
	UC_InstructionWidget* InstructionWidget = OwnerPlayer->GetHUDWidget()->GetInstructionWidget();
	InstructionWidget->ToggleDeployParachuteInstructionVisibility(true);
	InstructionWidget->ToggleEjectInstructionVisibility(false);
}

void UC_PlayerSkyDivingComponent::SetStateToParachutingState()
{
	// Alt키가 눌려있을 때에, Character Rotation 비율에 따라 Speed를 갱신시켜주어야 함
	if (OwnerCharacter->GetIsHoldDirection() || OwnerCharacter->GetIsAltPressed())
	{
		float CharacterPitch = OwnerCharacter->GetActorRotation().Pitch;
		UCharacterMovementComponent* OwnerMovement = OwnerCharacter->GetCharacterMovement();
			
		if (270.f <= CharacterPitch) // 아랫방향으로 숙였을 때 ( 360 ~ 270(아래로 최대로 숙였을 때) )
		{
			OwnerMovement->MaxWalkSpeed = FMath::GetMappedRangeValueClamped
			(
				FVector2D(360.f, 270.f),
				STATE_DIVINGSPEEDS[SkyDivingState].GetMaxWalkSpeedMaxMin(),
				CharacterPitch
			);
				
			VelocityZLerpDest = FMath::GetMappedRangeValueClamped
			(
				FVector2D(360.f, 270.f),
				STATE_DIVINGSPEEDS[SkyDivingState].GetZSpeedMinMax(),
				CharacterPitch
			);
		}
		else // Pitch 0 ~ 89.99 -> 윗 방향
		{
			OwnerMovement->MaxWalkSpeed = STATE_DIVINGSPEEDS[SkyDivingState].MaxWalkSpeed_Max;
			VelocityZLerpDest = STATE_DIVINGSPEEDS[SkyDivingState].ZSpeedMin;
		}
	}
	else VelocityZLerpDest = STATE_DIVINGSPEEDS[SkyDivingState].ZSpeedMin;

	// Instruction Key HUD 업데이트
	OwnerPlayer->GetHUDWidget()->GetInstructionWidget()->ToggleDeployParachuteInstructionVisibility(false);
}

void UC_PlayerSkyDivingComponent::SetStateToLandingState()
{
	// SkyDiving HUD 끄기
	OwnerPlayer->GetHUDWidget()->GetSkyDiveWidget()->SetVisibility(ESlateVisibility::Hidden);
}

void UC_PlayerSkyDivingComponent::OnCharacterLandedOnWater()
{
	Super::OnCharacterLandedOnWater();
	OwnerPlayer->GetHUDWidget()->GetSkyDiveWidget()->SetVisibility(ESlateVisibility::Hidden); // HUD 끄기
}

void UC_PlayerSkyDivingComponent::HandlePlayerMovement(const FVector2D& MovementVector)
{
	switch (SkyDivingState) case ESkyDivingState::READY: case ESkyDivingState::LANDING: case ESkyDivingState::MAX: return;

	if (MovementVector.X == -1)  // 뒷방향 input 낙하속도 적절히 조정
	{
		OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = 0.f;
		//VelocityZLerpDest = SKYDIVE_BACKKEY_ZSPEED;
		VelocityZLerpDest = STATE_DIVINGSPEEDS[SkyDivingState].BackKeyZSpeed;

		OwnerCharacter->SetNextSpeed(0.f);
		return;
	}

	bool HoldingDirection = OwnerCharacter->GetIsHoldDirection() || OwnerCharacter->GetIsAltPressed();

	FRotator Rotation = (!HoldingDirection) ? OwnerCharacter->GetController()->GetControlRotation() :
									          OwnerCharacter->GetActorRotation();
	Rotation.Roll = 0.f;

	UCharacterMovementComponent* OwnerMovement = OwnerCharacter->GetCharacterMovement();

	// Update MaxWalkSpeed & 중력 또한 Pitch에 따라 조정처리
	if (!HoldingDirection)
	{
		if (270.f <= Rotation.Pitch) // 아랫방향으로 숙였을 때 ( 360 ~ 270(아래로 최대로 숙였을 때) )
		{
			OwnerMovement->MaxWalkSpeed = FMath::GetMappedRangeValueClamped
			(
				FVector2D(360.f, 270.f),
				STATE_DIVINGSPEEDS[SkyDivingState].GetMaxWalkSpeedMaxMin(),
				Rotation.Pitch
			);
			
			VelocityZLerpDest = FMath::GetMappedRangeValueClamped
			(
				FVector2D(360.f, 270.f),
				STATE_DIVINGSPEEDS[SkyDivingState].GetZSpeedMinMax(),
				Rotation.Pitch
			);
		}
		else // Pitch 0 ~ 89.99 -> 윗 방향
		{
			OwnerMovement->MaxWalkSpeed = STATE_DIVINGSPEEDS[SkyDivingState].MaxWalkSpeed_Max;
			VelocityZLerpDest = STATE_DIVINGSPEEDS[SkyDivingState].ZSpeedMin;
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
		                  FMath::GetMappedRangeValueClamped(FVector2D(360.f, 270.f), FVector2D(250.f, 700.f), Rotation.Pitch) :
						  250.f;

	OwnerCharacter->SetNextSpeed(AnimNextSpeed); // AnimCharacter에서 Speed Lerp할 값 setting
}

void UC_PlayerSkyDivingComponent::OnPlayerSkyMoveEnd()
{
	// if (SkyDivingState == ESkyDivingState::SKYDIVING)
	VelocityZLerpDest = STATE_DIVINGSPEEDS[SkyDivingState].ZSpeedMin;
}

