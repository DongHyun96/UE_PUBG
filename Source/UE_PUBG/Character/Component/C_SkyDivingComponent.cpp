// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/C_SkyDivingComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"

#include "GameFramework/SpringArmComponent.h"

#include "Utility/C_Util.h"

UC_SkyDivingComponent::UC_SkyDivingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UC_SkyDivingComponent::BeginPlay()
{
	Super::BeginPlay();

	if (ParachuteBackpackStaticMesh)
	{
		ParachuteBackpackStaticMesh->AttachToComponent
		(
			OwnerCharacter->GetMesh(),
			FAttachmentTransformRules(EAttachmentRule::KeepRelative, true),
			PARABACKPACK_SOCKET_NAME
		);

		ParachuteBackpackStaticMesh->SetVisibility(false);
	}
	
	if (ParachuteSkeletalMesh)
	{
		ParachuteSkeletalMesh->AttachToComponent
		(
			OwnerCharacter->GetMesh(),
			FAttachmentTransformRules(EAttachmentRule::KeepRelative, true),
			PARACHUTE_SOCKET_NAME
		);

		ParachuteSkeletalMesh->SetVisibility(false);
	}

	// OwnerPlayer�� Casting �õ�
	OwnerPlayer = Cast<AC_Player>(OwnerCharacter);
}

void UC_SkyDivingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	LerpPlayerMainCameraArmLength(DeltaTime);
	LerpVelocityZ(DeltaTime);

	HandleStateTransitionByHeight();

	if (!IsValid(ParachuteSkeletalMesh) || !IsValid(ParachuteBackpackStaticMesh)) UC_Util::Print("WARNING");
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

	if (MovementVector.X == -1)  // �޹��� input ���ϼӵ� ������ ����
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

	// Update MaxWalkSpeed & �߷� ���� Pitch�� ���� ����ó��
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

	// Pitch�� ���� NextSpeed �������� �������� animation �ٸ��� �ֱ� & �߷� ����
	// Pitch 0 ~ 89.99 -> �� ���� / 360 ~ 270 �Ʒ�����

	if (HoldingDirection) return; // HoldDirection ���̶�� return (HoldDirection ������ ������ ������ ��� ����)

	float AnimNextSpeed = (270.f <= Rotation.Pitch) ? 
		                  FMath::GetMappedRangeValueClamped(FVector2D(360.f, 270.f), FVector2D(2000.f, 4000.f), Rotation.Pitch) :
						  2000.f;

	OwnerCharacter->SetNextSpeed(AnimNextSpeed); // AnimCharacter���� Speed Lerp�� �� setting
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
		// TODO : Player�� ��� ����� ��ġ�� ī�޶� ���
		OwnerCharacter->SetActorHiddenInGame(true);
		OwnerCharacter->GetCharacterMovement()->GravityScale = 0.f;
		SkyDivingState = InSkyDivingState;
		return;
	case ESkyDivingState::SKYDIVING:
	{
		SkyDivingState = InSkyDivingState;

		// TODO : ����� ��ġ�� Character spawn(Visibility on) & Player�� ��� MainCamera Player�� �ٽ� ����ֱ�
		OwnerCharacter->SetActorHiddenInGame(false);
		ParachuteBackpackStaticMesh->SetVisibility(true);

		UCharacterMovementComponent* OwnerMovement = OwnerCharacter->GetCharacterMovement();

		OwnerMovement->AirControl   = 1.f;
		//OwnerMovement->GravityScale = 0.5f;
		VelocityZLerpDest = State_DivingSpeeds[SkyDivingState].BackKeyZSpeed;

		OwnerMovement->BrakingDecelerationFalling = 2048.f;

		//OwnerMovement->AirControlBoostMultiplier = 10.0f;
		//OwnerMovement->MaxFlySpeed = 1000.f;
		//OwnerMovement->BrakingDecelerationFlying = 2048.f;
		return;
	}
	case ESkyDivingState::PARACHUTING:
	{
		ParachuteSkeletalMesh->SetVisibility(true);
		VelocityZLerpDest = State_DivingSpeeds[SkyDivingState].BackKeyZSpeed;
		SkyDivingState = ESkyDivingState::PARACHUTING;
		OwnerCharacter->PlayAnimMontage(DeployParachuteMontage);
		ParachuteSkeletalMesh->GetAnimInstance()->Montage_Play(ParachuteDeployMontage);
		//ParachuteSkeletalMesh->PlayAnimation()
		return;
	}
	case ESkyDivingState::LANDING:
	{
		OwnerCharacter->PlayAnimMontage(LandingMontage);
		SkyDivingState = ESkyDivingState::LANDING;
		return;
	}
	default: return;
	}
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

void UC_SkyDivingComponent::LerpPlayerMainCameraArmLength(const float& DeltaTime)
{
	if (!OwnerPlayer) return;
	if (OwnerPlayer->GetMainState() != EMainState::SKYDIVING) return;

	USpringArmComponent* MainCamSpringArm = OwnerPlayer->GetMainSpringArm();
	
	switch (SkyDivingState)
	{
	case ESkyDivingState::SKYDIVING:
		MainCamSpringArm->TargetArmLength = FMath::Lerp(MainCamSpringArm->TargetArmLength, PLAYER_SKYDIVE_MAINCAM_ARMLENGTH, DeltaTime * 5.f);
		return;
	case ESkyDivingState::PARACHUTING:
		MainCamSpringArm->TargetArmLength = FMath::Lerp(MainCamSpringArm->TargetArmLength, PLAYER_PARACHUTE_MAINCAM_ARMLENGTH, DeltaTime * 5.f);
		return;
	case ESkyDivingState::LANDING: // TODO : Landing �� ���ķ� TargetArmLength ����ġ �Ǿ��ִ��� Ȯ��(LANDING �ܰ迡�� AnimMontage�� Landing ���� ����� ����)
		MainCamSpringArm->TargetArmLength = FMath::Lerp(MainCamSpringArm->TargetArmLength, PLAYER_ORIGIN_MAINCAM_ARMLENGTH, DeltaTime * 5.f);
		return;
	case ESkyDivingState::READY:
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

}
