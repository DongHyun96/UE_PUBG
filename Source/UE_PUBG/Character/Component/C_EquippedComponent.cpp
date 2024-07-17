// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/C_EquippedComponent.h"
#include "Character/C_BasicCharacter.h"

#include "Item/Weapon/C_Weapon.h"
#include "Item/Weapon/Gun/C_Gun.h"
#include "Item/Weapon/Gun/C_AR.h"
#include "Item/Weapon/MeleeWeapon/C_MeleeWeapon.h"
#include "Item/Weapon/ThrowingWeapon/C_ThrowingWeapon.h"



// Sets default values for this component's properties
UC_EquippedComponent::UC_EquippedComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UC_EquippedComponent::BeginPlay()
{
	Super::BeginPlay();

	SpawnWeaponsForTesting();
}


// Called every frame
void UC_EquippedComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UC_EquippedComponent::ChangeCurWeapon(EWeaponSlot InChangeTo)
{
	if (IsValid(Weapons[CurWeaponType]))
	{
		// 현재 무기의 Sheath나 Draw animation montage가 이미 재생 중이라면 return
		if (OwnerCharacter->GetMesh()->GetAnimInstance()
			->Montage_IsPlaying(Weapons[CurWeaponType]->GetCurDrawMontage().AnimMontage)) 
			return false;

		if (OwnerCharacter->GetMesh()->GetAnimInstance()
			->Montage_IsPlaying(Weapons[CurWeaponType]->GetCurSheathMontage().AnimMontage))
			return false;
	}

	NextWeaponType = InChangeTo;

	if (CurWeaponType == NextWeaponType) return false; // 현재 무기와 다음 무기가 같을 때 무기를 굳이 다시 꺼내지 않음

	// NextWeaponType이 None이 아니고, 해당 무기 슬롯에 무기가 없을 때
	if (NextWeaponType != EWeaponSlot::NONE && !IsValid(Weapons[NextWeaponType])) return false;

	// 현재 무기를 착용중이지 않을 때 (UnArmed 상태), 다음 무기 Draw만 재생
	if (CurWeaponType == EWeaponSlot::NONE || !IsValid(GetCurWeapon()))
	{
		CurWeaponType = NextWeaponType;
		OwnerCharacter->PlayAnimMontage(Weapons[CurWeaponType]->GetCurDrawMontage());
		return true;
	}
	
	// 현재 무기를 착용중인 상황
	OwnerCharacter->PlayAnimMontage(Weapons[CurWeaponType]->GetCurSheathMontage()); // 현 무기 집어넣는 동작에 Notify함수 걸어서 다음 무기로 전환

	return true;
}

bool UC_EquippedComponent::ToggleArmed()
{
	// 현재 무기도 장착하지 않았고 이전에 들고 있었던 무기도 없을 때 (초기 상태)
	if (CurWeaponType == EWeaponSlot::NONE && PrevWeaponType == EWeaponSlot::NONE) return false;
	
	// 현재 들고 있는 무기가 있을 때
	if (CurWeaponType != EWeaponSlot::NONE && IsValid(GetCurWeapon()))
	{
		PrevWeaponType = CurWeaponType;
		return ChangeCurWeapon(EWeaponSlot::NONE);
	}

	// 현재 들고 있는 무기가 없을 때
	return ChangeCurWeapon(PrevWeaponType);
}

void UC_EquippedComponent::OnSheathEnd()
{
	// 현재 무기 무기집에 붙이기
	GetCurWeapon()->AttachToHolster(OwnerCharacter->GetMesh());

	CurWeaponType = NextWeaponType;
	
	if (!IsValid(GetCurWeapon()))
	{
		OwnerCharacter->SetHandState(EHandState::UNARMED);
		return;
	}

	OwnerCharacter->PlayAnimMontage(GetCurWeapon()->GetCurDrawMontage());
}

void UC_EquippedComponent::OnDrawStart()
{
	GetCurWeapon()->AttachToHand(OwnerCharacter->GetMesh());
}

void UC_EquippedComponent::OnDrawEnd()
{
	GetCurWeapon()->AttachToHand(OwnerCharacter->GetMesh());

	NextWeaponType = EWeaponSlot::NONE;
}

void UC_EquippedComponent::SpawnWeaponsForTesting()
{
	// Test용 weapon spawn들
	FActorSpawnParameters Param{};
	Param.Owner = OwnerCharacter;
	AC_MeleeWeapon* MeleeTemp = GetWorld()->SpawnActor<AC_MeleeWeapon>(WeaponClasses[EWeaponSlot::MELEE_WEAPON], Param);
	MeleeTemp->SetOwnerCharacter(OwnerCharacter);
	MeleeTemp->AttachToHolster(OwnerCharacter->GetMesh());

	Weapons[EWeaponSlot::MELEE_WEAPON] = MeleeTemp;

	FActorSpawnParameters Param2{};
	Param2.Owner = OwnerCharacter;
	AC_Gun* ARTemp = GetWorld()->SpawnActor<AC_Gun>(WeaponClasses[EWeaponSlot::MAIN_GUN], Param2);
	ARTemp->SetOwnerCharacter(OwnerCharacter);
	ARTemp->AttachToHolster(OwnerCharacter->GetMesh());

	Weapons[EWeaponSlot::MAIN_GUN] = ARTemp;

	FActorSpawnParameters Param3{};
	Param3.Owner = OwnerCharacter;
	AC_ThrowingWeapon* ThrowTemp = GetWorld()->SpawnActor<AC_ThrowingWeapon>(WeaponClasses[EWeaponSlot::THROWABLE_WEAPON], Param3);
	ThrowTemp->SetOwnerCharacter(OwnerCharacter);
	ThrowTemp->AttachToHolster(OwnerCharacter->GetMesh());

	Weapons[EWeaponSlot::THROWABLE_WEAPON] = ThrowTemp;

	AC_ThrowingWeapon::InitTestPool(OwnerCharacter, WeaponClasses[EWeaponSlot::THROWABLE_WEAPON], this);
}


