// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/C_EquippedComponent.h"
#include "Character/C_BasicCharacter.h"

#include "Weapon/C_Weapon.h"
#include "Weapon/Gun/C_Gun.h"
#include "Weapon/MeleeWeapon/C_MeleeWeapon.h"
#include "Weapon/ThrowingWeapon/C_ThrowingWeapon.h"

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

	// Test용 weapon spawn, 프라이팬만 스폰 시켰음
	FActorSpawnParameters Param{};
	Param.Owner = OwnerCharacter;
	AC_MeleeWeapon* MeleeTemp = GetWorld()->SpawnActor<AC_MeleeWeapon>(WeaponClasses[EWeaponSlot::MELEE_WEAPON], Param);
	MeleeTemp->SetOwnerCharacter(OwnerCharacter);
	MeleeTemp->AttachToHolster(OwnerCharacter->GetMesh());
	MeleeWeapon = MeleeTemp;
}


// Called every frame
void UC_EquippedComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UC_EquippedComponent::ChangeCurWeapon(EWeaponSlot InChangeTo)
{
	if (CurWeapon)
	{
		// 현재 무기의 Sheath나 Draw animation montage가 이미 재생 중이라면 return
		if (OwnerCharacter->GetMesh()->GetAnimInstance()->Montage_IsPlaying(CurWeapon->GetCurDrawMontage())) return false;

		if (OwnerCharacter->GetMesh()->GetAnimInstance()->Montage_IsPlaying(CurWeapon->GetCurSheathMontage())) return false;
	}

	switch (InChangeTo)
	{
	case EWeaponSlot::NONE:				NextWeapon = nullptr;
		break;
	case EWeaponSlot::MAIN_GUN:			NextWeapon = MainGun;
		break;
	case EWeaponSlot::SUB_GUN:			NextWeapon = SubGun;
		break;
	case EWeaponSlot::MELEE_WEAPON:		NextWeapon = MeleeWeapon;
		break;
	case EWeaponSlot::THROWABLE_WEAPON:	NextWeapon = ThrowingWeapon;
		break;
	default: break;
	}

	if (CurWeapon == NextWeapon) return false; // 현재 무기와 다음 무기가 같을 때 무기를 굳이 다시 꺼내지 않음

	// 바꿀 다음 무기가 UNARMED가 아닌 상태에서, 해당 무기 슬롯에 무기가 없을 때
	if (!NextWeapon && InChangeTo != EWeaponSlot::NONE) return false;

	// 현재 무기를 착용중이지 않을 때 (UnArmed 상태), 다음 무기 Draw만 재생
	if (!CurWeapon)
	{
		CurWeapon = NextWeapon;
		OwnerCharacter->PlayAnimMontage(CurWeapon->GetCurDrawMontage());
		return true;
	}
	
	// 현재 무기를 착용중인 상황
	OwnerCharacter->PlayAnimMontage(CurWeapon->GetCurSheathMontage()); // 현 무기 집어넣는 동작에 Notify함수 걸어서 다음 무기로 전환

	return false;
}

bool UC_EquippedComponent::ToggleArmed()
{
	// 현재 무기도 장착하지 않았고 이전에 들고 있었던 무기도 없을 때
	if (!CurWeapon && !PrevWeapon) return false;

	// 현재 들고 있는 무기가 있을 때 
	if (CurWeapon)
	{
		PrevWeapon = CurWeapon;



		return true;
	}

	// 현재 들고 있는 무기가 없을 때


	return false;
}

void UC_EquippedComponent::OnSheathEnd()
{
	// 현재 무기 무기집에 붙이기
	CurWeapon->AttachToHolster(OwnerCharacter->GetMesh());

	CurWeapon = NextWeapon;
	
	if (!CurWeapon)
	{
		OwnerCharacter->SetHandState(EHandState::UNARMED);
		return;
	}

	OwnerCharacter->PlayAnimMontage(CurWeapon->GetCurDrawMontage());
}

void UC_EquippedComponent::OnDrawEnd()
{
	CurWeapon->AttachToHand(OwnerCharacter->GetMesh());

	NextWeapon = nullptr;
}


