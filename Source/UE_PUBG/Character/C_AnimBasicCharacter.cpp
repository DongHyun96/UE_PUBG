// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/C_AnimBasicCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Character/Component/C_EquippedComponent.h"
#include "Item/Weapon/C_Weapon.h"
#include "Item/Weapon/Gun/C_Gun.h"
#include "Character/C_Player.h"

#include "Utility/C_Util.h"

#include "Character/C_BasicCharacter.h"

#include "Character/Component/C_PoseColliderHandlerComponent.h"
#include "Character/Component/C_SwimmingComponent.h"
#include "Character/Component/C_SkyDivingComponent.h"

void UC_AnimBasicCharacter::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	OwnerCharacter = Cast<AC_BasicCharacter>(TryGetPawnOwner());
	//CSpineRotation = FRotator(0);
	//CHeadLookAtRotation = FQuat(0);
}

void UC_AnimBasicCharacter::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!OwnerCharacter) return;

	//Speed = OwnerCharacter->GetVelocity().Size2D();		
	Speed = FMath::Lerp(Speed, OwnerCharacter->GetNextSpeed(), DeltaSeconds * 15.f);

	const FRotator Rotation = OwnerCharacter->GetActorRotation();

	const FRotator YawRotation(0, Rotation.Yaw, 0);

	Direction = CalculateDirection(OwnerCharacter->GetVelocity().GetSafeNormal2D(), YawRotation);

	//FString TheFloatStr = FString::SanitizeFloat(Direction);
	//GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, *TheFloatStr);

	MainState		  = OwnerCharacter->GetMainState();
	HandState         = OwnerCharacter->GetHandState();
	PoseState         = OwnerCharacter->GetPoseState();
	bIsFalling        = OwnerCharacter->GetCharacterMovement()->IsFalling();
	bIsJumping        = OwnerCharacter->GetIsJumping();
	bIsAimingRifle    = OwnerCharacter->GetIsAimDown();
	bCanCharacterMove = OwnerCharacter->GetCanMove();
	bIsHoldDirection  = OwnerCharacter->GetIsHoldDirection();
	bIsAimDownSight   = OwnerCharacter->GetIsAimDown();
	
	SwimmingState	  = OwnerCharacter->GetSwimmingComponent()->GetSwimmingState();
	SkyDivingState	  = OwnerCharacter->GetSkyDivingComponent()->GetSkyDivingState();
	//switch (SwimmingState)
	//{
	//case ESwimmingState::ON_GROUND:
	//	UC_Util::Print("OnGround");
	//	break;
	//case ESwimmingState::SWIMMING_SURFACE: case ESwimmingState::SWIMMING_UNDER:
	//	UC_Util::Print("Swimming");
	//	break;
	//default:
	//	break;
	//}
	bCharacterIsWatchingSight = OwnerCharacter->GetIsWatchingSight();
	SetLeftHandIKOn();
	SetCanUseAimOffset();
	SetIsLeftHandIKOn();

	CrawlRotationAngle = OwnerCharacter->GetPoseColliderHandlerComponent()->GetCurrentCrawlSlopeAngleForRigControl();

	AC_Gun* CurrentGun = Cast<AC_Gun>(OwnerCharacter->GetEquippedComponent()->GetCurWeapon());

	ControlHeadRotation();
	SetAimOfssetRotation();
	if (IsValid(CurrentGun))
	{
		bGunHasGrip = CurrentGun->GetGunHasGrip();
	}
	SetAimingTurnInPlaceRotation();
}

void UC_AnimBasicCharacter::AnimNotify_OnStartTransition_Stand_To_Falling()
{
	AnimNotify_OnStartTransition_RunningJump_To_Falling();
	OwnerCharacter->GetCharacterMovement()->Velocity.X = 0;
	OwnerCharacter->GetCharacterMovement()->Velocity.Y = 0;
	//OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = 0;
	//OwnerCharacter->GetCharacterMovement()->MaxAcceleration = 1000000000000;
}

void UC_AnimBasicCharacter::AnimNotify_OnStartTransition_RunningJump_To_Falling()
{
	// Start Transition 시 수행할 로직 추가
	FString TheFloatStr = "Start RunningJump To Falling Transition";
	GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, *TheFloatStr);
	UE_LOG(LogTemp, Warning, TEXT("Transition Started"));
	//Cast<AC_Player>(OwnerCharacter)->BackToMainCamera();
	if (OwnerCharacter->GetIsAimDown())
	{
		AC_Gun* CurGun = Cast<AC_Gun>(OwnerCharacter->GetEquippedComponent()->GetCurWeapon());
		if (IsValid(CurGun))
		{
			CurGun->BackToMainCamera();
		}
	}
	OwnerCharacter->SetCanMove(false);

}

void UC_AnimBasicCharacter::AnimNotify_OnEndTransition_HardLand_To_Stand()
{
	// End Transition 시 수행할 로직 추가
	FString TheFloatStr = "End Transition";
	GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, *TheFloatStr);
	UE_LOG(LogTemp, Warning, TEXT("Transition Ended"));
	OwnerCharacter->SetCanMove(true);
	AnimNotify_OnEndTransition_Falling_To_Standing();


}

void UC_AnimBasicCharacter::AnimNotify_OnEndTransition_Falling_To_Standing()
{
	OwnerCharacter->SetIsJumping(false);
	OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = 600;
	OwnerCharacter->GetCharacterMovement()->MaxAcceleration = 2048;


}

void UC_AnimBasicCharacter::ControlHeadRotation()
{
	FRotator ControlRotation = OwnerCharacter->GetControlRotation();
	FRotator CapsuleRotation = OwnerCharacter->GetCapsuleComponent()->GetComponentRotation();

	FRotator DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(ControlRotation, CapsuleRotation);
	DeltaRotation.Roll  /= -5.0f;
	DeltaRotation.Pitch /= -5.0f;
	DeltaRotation.Yaw   /=  5.0f;

	DeltaRotation.Roll  = FMath::Clamp(DeltaRotation.Roll,  -90, 90);	
	DeltaRotation.Pitch = FMath::Clamp(DeltaRotation.Pitch, -90, 90);	
	DeltaRotation.Yaw   = FMath::Clamp(DeltaRotation.Yaw,   -90, 90);

	//DeltaRotation.Roll  = 0;
	//DeltaRotation.Pitch = 0;
	//DeltaRotation.Yaw   = 0;
	
	CSpineRotation = UKismetMathLibrary::RInterpTo(CSpineRotation, DeltaRotation, 1.0, 0.3);
	//CSpineRotation = DeltaRotation;

	CHeadLookAtRotation = UKismetMathLibrary::MakeRotator(CSpineRotation.Pitch, CSpineRotation.Yaw, CSpineRotation.Roll).Quaternion();
	//CHeadLookAtRotation = FQuat::Identity;
	DeltaRotation.Roll  *= 5.0f / 3.0f;
	DeltaRotation.Pitch *= 5.0f / 3.0f;
	DeltaRotation.Yaw   *= 5.0f / 3.0f;

	DeltaRotation.Roll = FMath::Clamp(DeltaRotation.Roll, -30, 30);
	DeltaRotation.Pitch = FMath::Clamp(DeltaRotation.Pitch, -30, 30);
	DeltaRotation.Yaw = FMath::Clamp(DeltaRotation.Yaw, -30, 30);
	//CSpineRotation = DeltaRotation;
	CAimingRotation.Roll = DeltaRotation.Pitch;
	CAimingRotation.Pitch = DeltaRotation.Yaw;
	CAimingRotation.Yaw = DeltaRotation.Roll;
}

void UC_AnimBasicCharacter::RilfeLeftHandIK()
{
}

void UC_AnimBasicCharacter::SetAimOfssetRotation()
{
	float DeltaTime = OwnerCharacter->GetWorld()->GetDeltaSeconds();
	//AimOffsetLerpDelayTime += DeltaTime;
	////if (AimOffsetLerpDelayTime <= 0.1f)
	////	return;
	FRotator ControlRotation = OwnerCharacter->GetControlRotation();
	FRotator CapsuleRotation = OwnerCharacter->GetActorRotation();
	CAimOffsetRotation = UKismetMathLibrary::NormalizedDeltaRotator(ControlRotation, CapsuleRotation);

	FRotator LerpAlphaRotater  = UKismetMathLibrary::NormalizedDeltaRotator(CCurrentAimOffsetRotation, CAimOffsetRotation);
	float RotatorSizeX = LerpAlphaRotater.Roll  * LerpAlphaRotater.Roll  / 8100.f;
	float RotatorSizeY = LerpAlphaRotater.Pitch * LerpAlphaRotater.Pitch / 8100.f;
	float RotatorSizeZ = LerpAlphaRotater.Yaw   * LerpAlphaRotater.Yaw   / 8100.f;
	float Size = 10* UKismetMathLibrary::Sqrt(RotatorSizeX + RotatorSizeY + RotatorSizeZ);
	//UC_Util::Print(Size, FColor::Green);

	float LerpAlpha = UKismetMathLibrary::FClamp(Size, 0.3, 1);
	//UC_Util::Print(float(LerpAlpha));

	//CCurrentAimOffsetRotation = CAimOffsetRotation;
	FRotator::ZeroRotator;
	if(!OwnerCharacter->GetIsAimDown())
		CCurrentAimOffsetRotation = UKismetMathLibrary::RLerp(CCurrentAimOffsetRotation, CAimOffsetRotation, DeltaTime * 15.f * LerpAlpha, true);
	else
	{
		FRotator TempRotator = UKismetMathLibrary::RLerp(CCurrentAimOffsetRotation, CAimOffsetRotation, DeltaTime * 15.f * LerpAlpha, true);
		CCurrentAimOffsetRotation.Pitch = TempRotator.Pitch;
		CCurrentAimOffsetRotation.Yaw = 0;

	}
	AimOffsetLerpDelayTime = 0;
	AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharacter);
	AC_Gun* CurGun = Cast<AC_Gun>(OwnerPlayer->GetEquippedComponent()->GetCurWeapon());

	if (OwnerPlayer->GetIsWatchingSight() && OwnerPlayer->GetPoseState() == EPoseState::CRAWL)
	{
		if (CCurrentAimOffsetRotation.Pitch < 0)
		{

			OwnerCharacter->GetMesh()->HideBoneByName(FName("LeftArm"), EPhysBodyOp::PBO_None);
			OwnerCharacter->GetMesh()->HideBoneByName(FName("RightArm"), EPhysBodyOp::PBO_None);
			CurGun->GetGunMesh()->SetVisibility(false,true);
		}
		else
		{

			OwnerCharacter->GetMesh()->UnHideBoneByName(FName("LeftArm"));
			OwnerCharacter->GetMesh()->UnHideBoneByName(FName("RightArm"));
			CurGun->GetGunMesh()->SetVisibility(true,true);

		}


	}
	else
	{
		OwnerCharacter->GetMesh()->UnHideBoneByName(FName("LeftArm"));
		OwnerCharacter->GetMesh()->UnHideBoneByName(FName("RightArm"));
		if(IsValid(CurGun))
			CurGun->GetGunMesh()->SetVisibility(true,true);



	}
	//UC_Util::Print(float(CCurrentAimOffsetRotation.Yaw),FColor::Blue);

	//UC_Util::Print(float(CCurrentAimOffsetRotation.Yaw));

	//GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Green, *TheFloatStr);
	
}

void UC_AnimBasicCharacter::SetAimingTurnInPlaceRotation()
{
	float ControlRotation = OwnerCharacter->GetControlRotation().Yaw;
	float CapsuleRotation = OwnerCharacter->GetCapsuleComponent()->GetComponentRotation().Yaw;
	if (AimingTurnInPlaceTimeCount <= 0)
	{
		SavedYaw = CapsuleRotation;
	}
	AimingTurnInPlaceTimeCount += OwnerCharacter->GetWorld()->DeltaTimeSeconds;
	DeltaYaw = CapsuleRotation - SavedYaw;
	if (AimingTurnInPlaceTimeCount >= 0.5f)
	{
		//UKismetMathLibrary::Lerp(AimingTurnInPlaceTimeCount, -0.1, 0.5);
		AimingTurnInPlaceTimeCount = 0;
		return;
	}
}

void UC_AnimBasicCharacter::SetLeftHandIKOn()
{

	AC_Gun* CurrentGun = Cast<AC_Gun>(OwnerCharacter->GetEquippedComponent()->GetCurWeapon());
	if (IsValid(CurrentGun))
	{
		RifleLeftHandSocket = CurrentGun->GetLeftHandSocketTransform();
		UAnimMontage* RifleSheathMontage = CurrentGun->GetSheathMontages()[OwnerCharacter->GetPoseState()].Montages[CurrentGun->GetCurrentWeaponState()].AnimMontage;
		UAnimMontage* RifleDrawMontage = CurrentGun->GetDrawMontages()[OwnerCharacter->GetPoseState()].Montages[CurrentGun->GetCurrentWeaponState()].AnimMontage;

		if (IsValid(RifleSheathMontage) || IsValid(RifleDrawMontage))
		{
			UAnimInstance* TempAnim = OwnerCharacter->GetMesh()->GetAnimInstance();
			bool IsSheatOrDraw = (TempAnim->Montage_IsPlaying(RifleDrawMontage) || TempAnim->Montage_IsPlaying(RifleSheathMontage));
			bCharacterIsSheathing = IsSheatOrDraw;
			//if (IsSheatOrDraw)
			//	UC_Util::Print("OK");
			//else UC_Util::Print("No",FColor::Blue);
		}
		else
			bCharacterIsSheathing = false;
	}
	else
	{
		bCharacterIsSheathing = true;
		//UC_Util::Print("Sheating!");
	}
}

void UC_AnimBasicCharacter::SetCanUseAimOffset()
{
	bool Condition0 = !bCanCharacterMove && !bIsAimDownSight;
	bool Condition1 = bIsFalling || Condition0;
	bool Condition2 = bCharacterIsSheathing || Condition1;
	bool Condition3 = (Speed >= 480) || Condition2;
	bool Condition4 = OwnerCharacter->GetIsReloadingBullet() || Condition3;
	bCanUseAimOffset = Condition4;

}

void UC_AnimBasicCharacter::SetIsLeftHandIKOn()
{
	//if (HandState != EHandState::WEAPON_GUN) 
	//{
	//	bIsLeftHandIKOn = false;
	//	return;
	//}
	bool Condition0 = bCanCharacterMove && !bIsFalling;
	bool Condition1 = Condition0 && (HandState == EHandState::WEAPON_GUN);	
	bool Condition2 = !bCharacterIsSheathing && Condition1;
	bool Condition3 = !OwnerCharacter->GetIsReloadingBullet() && Condition2;

	bIsLeftHandIKOn = Condition3;
}

