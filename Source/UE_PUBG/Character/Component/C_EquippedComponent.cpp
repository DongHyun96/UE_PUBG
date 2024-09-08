// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/C_EquippedComponent.h"
#include "Character/C_BasicCharacter.h"

#include "Item/Weapon/C_Weapon.h"
#include "Item/Weapon/Gun/C_Gun.h"
#include "Item/Weapon/Gun/C_AR.h"
#include "Item/Weapon/MeleeWeapon/C_MeleeWeapon.h"
#include "Item/Weapon/ThrowingWeapon/C_ThrowingWeapon.h"

#include "Utility/C_Util.h"

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

void UC_EquippedComponent::SetSlotWeapon(EWeaponSlot InSlot, AC_Weapon* Weapon)
{
	// ?�어???�롯???�전 무기???�??PoseTransitionEnd ?�리게이???�제
	if (Weapons[InSlot]) 
		OwnerCharacter->Delegate_OnPoseTransitionFin.RemoveAll(Weapons[InSlot]);

	Weapons[InSlot] = Weapon;

	// ?�로 ?�착??무기???�??PoseTransitionEnd ?�리게이??callback 걸기
	if (Weapons[InSlot]) 
		OwnerCharacter->Delegate_OnPoseTransitionFin.AddUObject(Weapons[InSlot], &AC_Weapon::OnOwnerCharacterPoseTransitionFin);
}

bool UC_EquippedComponent::ChangeCurWeapon(EWeaponSlot InChangeTo)
{
	if (IsValid(Weapons[CurWeaponType]))
	{
		// ?�재 무기??Sheath??Draw animation montage가 ?��? ?�생 중이?�면 return
		if (OwnerCharacter->GetMesh()->GetAnimInstance()
			->Montage_IsPlaying(Weapons[CurWeaponType]->GetCurDrawMontage().AnimMontage)) 
			return false;

		if (OwnerCharacter->GetMesh()->GetAnimInstance()
			->Montage_IsPlaying(Weapons[CurWeaponType]->GetCurSheathMontage().AnimMontage))
			return false;
	}

	NextWeaponType = InChangeTo;

	if (CurWeaponType == NextWeaponType) return false; // ?�재 무기?� ?�음 무기가 같을 ??무기�?굳이 ?�시 꺼내지 ?�음

	// NextWeaponType??None???�니�? 바꾸?�는 무기 ?�롯??무기가 ?�을 ??
	if (NextWeaponType != EWeaponSlot::NONE && !IsValid(Weapons[NextWeaponType])) return false;

	// ?�재 무기�?착용중이지 ?�을 ??(UnArmed ?�태), ?�는 ?�재 ?�롯???�착??무기가 ?�을 ???�음 무기 Draw�??�생
	if (CurWeaponType == EWeaponSlot::NONE || !IsValid(GetCurWeapon()))
	{
		// 만약 ?�음??바�? 무기가 None?�거???�음??바�? 무기 ?�롯??무기가 ?�을 ??
		if (NextWeaponType == EWeaponSlot::NONE || !IsValid(Weapons[NextWeaponType]))
		{
			CurWeaponType = EWeaponSlot::NONE;
			NextWeaponType = EWeaponSlot::NONE;
			OwnerCharacter->SetHandState(EHandState::UNARMED);
			return false;
		}

		 // ?�음 무기가 ?�을 ??
		OwnerCharacter->PlayAnimMontage(Weapons[NextWeaponType]->GetCurDrawMontage());
		return true;
	}
	
	// ?�재 무기�?착용중인 ?�황

	// ?�척�??�외처리
	if (CurWeaponType == EWeaponSlot::THROWABLE_WEAPON)
	{
		AC_ThrowingWeapon* ThrowingWeapon = Cast<AC_ThrowingWeapon>(GetCurWeapon());
		if (IsValid(ThrowingWeapon))
		{
			// ?��? 쿠킹???�작?�었�? ?�직 ?�에???�나지 ?��? ?�척류라�??�에 ?�굼
			if (ThrowingWeapon->GetIsCooked() && ThrowingWeapon->GetAttachParentActor())
				return ThrowingWeapon->ReleaseOnGround();
		}
	}
	//총을 ?�고 Aiming 중일 ??카메???�시 ?�래?��??�환
	if (CurWeaponType == EWeaponSlot::MAIN_GUN || CurWeaponType == EWeaponSlot::SUB_GUN)
	{
		AC_Gun* TempWeapon = Cast<AC_Gun>(Weapons[CurWeaponType]);
		if (IsValid(TempWeapon))
			TempWeapon->BackToMainCamera();
	}

	OwnerCharacter->PlayAnimMontage(Weapons[CurWeaponType]->GetCurSheathMontage()); // ??무기 집어?�는 ?�작??Notify?�수 걸어???�음 무기�??�환
	return true;
}

bool UC_EquippedComponent::ToggleArmed()
{
	// ?�재 무기???�착?��? ?�았�??�전???�고 ?�었??무기???�을 ??(초기 ?�태)
	if (CurWeaponType == EWeaponSlot::NONE && PrevWeaponType == EWeaponSlot::NONE) return false;
	
	// ?�재 ?�고 ?�는 무기가 ?�을 ??
	if (CurWeaponType != EWeaponSlot::NONE && IsValid(GetCurWeapon()))
	{
		PrevWeaponType = CurWeaponType;


		return ChangeCurWeapon(EWeaponSlot::NONE);
	}

	// ?�재 ?�고 ?�는 무기가 ?�을 ??
	return ChangeCurWeapon(PrevWeaponType);
}

void UC_EquippedComponent::OnSheathEnd()
{
	// ?�재 무기 무기집에 붙이�?
	GetCurWeapon()->AttachToHolster(OwnerCharacter->GetMesh());

	// 총기�??�외처리
	if (CurWeaponType == EWeaponSlot::MAIN_GUN || CurWeaponType == EWeaponSlot::SUB_GUN)
	{
		AC_Gun* TempWeapon = Cast<AC_Gun>(Weapons[CurWeaponType]);
		if (IsValid(TempWeapon))
			TempWeapon->SetIsAimPress(false);
	}

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
	Weapons[NextWeaponType]->AttachToHand(OwnerCharacter->GetMesh());
	//GetCurWeapon()->AttachToHand(OwnerCharacter->GetMesh());
}

void UC_EquippedComponent::OnDrawEnd()
{
	if (NextWeaponType == EWeaponSlot::NONE) return;

	UC_Util::Print("OnDrawEnd", FColor::Cyan, 5.f);

	Weapons[NextWeaponType]->AttachToHand(OwnerCharacter->GetMesh());
	CurWeaponType = NextWeaponType;
	NextWeaponType = EWeaponSlot::NONE;
}

void UC_EquippedComponent::SpawnWeaponsForTesting()
{
	// Test??weapon spawn??
	FActorSpawnParameters Param{};
	Param.Owner = OwnerCharacter;
	AC_MeleeWeapon* MeleeTemp = GetWorld()->SpawnActor<AC_MeleeWeapon>(WeaponClasses[EWeaponSlot::MELEE_WEAPON], Param);
	MeleeTemp->SetOwnerCharacter(OwnerCharacter);
	MeleeTemp->AttachToHolster(OwnerCharacter->GetMesh());

	SetSlotWeapon(EWeaponSlot::MELEE_WEAPON, MeleeTemp);

	FActorSpawnParameters Param2{};
	Param2.Owner = OwnerCharacter;
	AC_Gun* ARTemp = GetWorld()->SpawnActor<AC_Gun>(WeaponClasses[EWeaponSlot::MAIN_GUN], Param2);
	ARTemp->SetOwnerCharacter(OwnerCharacter);
	ARTemp->AttachToHolster(OwnerCharacter->GetMesh());

	SetSlotWeapon(EWeaponSlot::MAIN_GUN, ARTemp);

	FActorSpawnParameters Param3{};
	Param3.Owner = OwnerCharacter;
	AC_ThrowingWeapon* ThrowTemp = GetWorld()->SpawnActor<AC_ThrowingWeapon>(WeaponClasses[EWeaponSlot::THROWABLE_WEAPON], Param3);
	ThrowTemp->SetOwnerCharacter(OwnerCharacter);
	ThrowTemp->AttachToHolster(OwnerCharacter->GetMesh());

	SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, ThrowTemp);

	AC_ThrowingWeapon::InitTestPool(OwnerCharacter, WeaponClasses[EWeaponSlot::THROWABLE_WEAPON], this);
}


