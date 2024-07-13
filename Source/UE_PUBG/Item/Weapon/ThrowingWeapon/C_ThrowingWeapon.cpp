// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/ThrowingWeapon/C_ThrowingWeapon.h"

#include "Item/Weapon/WeaponStrategy/I_WeaponButtonStrategy.h"
#include "Item/Weapon/WeaponStrategy/C_ThrowingWeaponStrategy.h"


AC_ThrowingWeapon::AC_ThrowingWeapon()
{
	WeaponButtonStrategy = CreateDefaultSubobject<AC_ThrowingWeaponStrategy>("ThrowingWeaponStrategy");
}

void AC_ThrowingWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AC_ThrowingWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsValid(OwnerCharacter))
	{
		CurDrawMontage			= DrawMontages[OwnerCharacter->GetPoseState()];
		CurSheathMontage		= SheathMontages[OwnerCharacter->GetPoseState()];
		CurThrowProcessMontages = ThrowProcessMontages[OwnerCharacter->GetPoseState()];
	}
}

bool AC_ThrowingWeapon::AttachToHolster(USceneComponent* InParent)
{
	SetActorHiddenInGame(true);

	return AttachToComponent
	(
		InParent,
		FAttachmentTransformRules(EAttachmentRule::KeepRelative, true),
		HOLSTER_SOCKET_NAME
	);
}

bool AC_ThrowingWeapon::AttachToHand(USceneComponent* InParent)
{
	//this->SetHidden(false);

	SetActorHiddenInGame(false);

	OwnerCharacter->SetHandState(EHandState::WEAPON_THROWABLE);

	return AttachToComponent
	(
		InParent,
		FAttachmentTransformRules(EAttachmentRule::KeepRelative, true),
		EQUIPPED_SOCKET_NAME
	);
}

void AC_ThrowingWeapon::OnSetNextAction()
{
	// Play Ready or Throw
	if (bIsCharging)	OwnerCharacter->PlayAnimMontage(CurThrowProcessMontages.ThrowReadyMontage);
	else 				OwnerCharacter->PlayAnimMontage(CurThrowProcessMontages.ThrowMontage);
}
