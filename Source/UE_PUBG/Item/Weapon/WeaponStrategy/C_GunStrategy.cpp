// Fill out your copyright notice in the Description page of Project Settings.

#include "Item/Weapon/WeaponStrategy/C_GunStrategy.h"
#include "GameFramework/Actor.h"
#include "Character/C_BasicCharacter.h"
#include "Item/Weapon/C_Weapon.h"
#include "Item/Weapon/Gun/C_Gun.h"
#include "Item/Weapon/MeleeWeapon/C_MeleeWeapon.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimMontage.h"
#include "Character/C_BasicCharacter.h"
#include "Utility/C_Util.h"

#include "Character/C_Player.h"


bool AC_GunStrategy::UseBKeyStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	return false;
}

bool AC_GunStrategy::UseRKeyStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	return false;
}

bool AC_GunStrategy::UseMlb_StartedStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	UC_Util::Print("Mlb Clicked");
	FHitResult HitResult;
	
	AController* Controller = WeaponUser->GetController();
	APlayerCameraManager* PlayerCamera = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
	FVector StartLocation = PlayerCamera->GetCameraLocation();
	FRotator CameraRotation = PlayerCamera->GetCameraRotation();
	
	FVector ForwardVector = CameraRotation.Vector().GetSafeNormal() * 10000;
	FVector DestLocation = StartLocation + ForwardVector;
	FCollisionQueryParams CollisionParams{};
	CollisionParams.AddIgnoredActor(Weapon);
	CollisionParams.AddIgnoredActor(WeaponUser);
	CollisionParams.AddIgnoredActor(PlayerCamera);

	HitResult = {};
	bool HasHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, DestLocation, ECC_Visibility, CollisionParams);
	FVector WorldLocation, WorldDirection;
	APlayerController* WolrdContorller = GetWorld()->GetFirstPlayerController();
	WolrdContorller->DeprojectScreenPositionToWorld(0.5, 0.5, WorldLocation,WorldDirection);
	//Controller->ActorLineTraceSingle(HitResult, StartLocation, StartLocation + ForwardVector, ECollisionChannel::ECC_Visibility,FCollisionQueryParams::DefaultQueryParam);
	UC_Util::Print(HitResult.Distance);
	UC_Util::Print(WorldLocation);
	DrawDebugSphere(GetWorld(), HitResult.Location, 10.0f, 12, FColor::Red, true);
	//Controller->ActorLineTraceSingle(nullptr, ),)
	return false;
}

bool AC_GunStrategy::UseMlb_OnGoingStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	return false;
}

bool AC_GunStrategy::UseMlb_CompletedStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	return false;
}

bool AC_GunStrategy::UseMrb_StartedStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	MrbPressTimeCount = 0;
	AC_Gun* CurWeapon = Cast<AC_Gun>(Weapon);

	CurWeapon->SetAimingPress();
	UC_Util::Print("Mrb Clicked");



	return false;
}

bool AC_GunStrategy::UseMrb_OnGoingStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	AC_Gun* CurWeapon = Cast<AC_Gun>(Weapon);

	if (WeaponUser->GetMesh()->GetAnimInstance()->Montage_IsPlaying(Weapon->GetCurDrawMontage().AnimMontage)) return false;
	if (WeaponUser->GetMesh()->GetAnimInstance()->Montage_IsPlaying(Weapon->GetCurSheathMontage().AnimMontage)) return false;
	MrbPressTimeCount += WeaponUser->GetWorld()->GetDeltaSeconds();

	if (MrbPressTimeCount >= 0.2)
	{
		CurWeapon->SetIsAimPress(false);
		//UC_Util::Print("False");
		return true;
	}



	return true;
}

bool AC_GunStrategy::UseMrb_CompletedStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	//MrbPressTimeCount = 0;
	if (WeaponUser->GetMesh()->GetAnimInstance()->Montage_IsPlaying(Weapon->GetCurDrawMontage().AnimMontage)) return false;
	if (WeaponUser->GetMesh()->GetAnimInstance()->Montage_IsPlaying(Weapon->GetCurSheathMontage().AnimMontage)) return false;
	AC_Player* CurUser = Cast<AC_Player>(WeaponUser);
	AC_Gun* CurWeapon = Cast<AC_Gun>(Weapon);
	if (!IsValid(CurUser))   return false;
	if (!IsValid(CurWeapon)) return false;

	if (!CurWeapon->GetIsAimPress() && MrbPressTimeCount < 0.2)
	{
		CurWeapon->SetAimingDown();
		return true;

	}
	if (CurWeapon->GetIsAimPress())
	{
		
		CurWeapon->BackToMainCamera();

		return true;

	}
	CurWeapon->BackToMainCamera();
	CurWeapon->SetIsAimPress(false);

	//CurUser->BackToMainCamera();
	return true;
}
