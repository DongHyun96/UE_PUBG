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

	// Test�� weapon spawn, �������Ҹ� ���� ������
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
		// ���� ������ Sheath�� Draw animation montage�� �̹� ��� ���̶�� return
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

	if (CurWeapon == NextWeapon) return false; // ���� ����� ���� ���Ⱑ ���� �� ���⸦ ���� �ٽ� ������ ����

	// �ٲ� ���� ���Ⱑ UNARMED�� �ƴ� ���¿���, �ش� ���� ���Կ� ���Ⱑ ���� ��
	if (!NextWeapon && InChangeTo != EWeaponSlot::NONE) return false;

	// ���� ���⸦ ���������� ���� �� (UnArmed ����), ���� ���� Draw�� ���
	if (!CurWeapon)
	{
		CurWeapon = NextWeapon;
		OwnerCharacter->PlayAnimMontage(CurWeapon->GetCurDrawMontage());
		return true;
	}
	
	// ���� ���⸦ �������� ��Ȳ
	OwnerCharacter->PlayAnimMontage(CurWeapon->GetCurSheathMontage()); // �� ���� ����ִ� ���ۿ� Notify�Լ� �ɾ ���� ����� ��ȯ

	return false;
}

bool UC_EquippedComponent::ToggleArmed()
{
	// ���� ���⵵ �������� �ʾҰ� ������ ��� �־��� ���⵵ ���� ��
	if (!CurWeapon && !PrevWeapon) return false;

	// ���� ��� �ִ� ���Ⱑ ���� �� 
	if (CurWeapon)
	{
		PrevWeapon = CurWeapon;



		return true;
	}

	// ���� ��� �ִ� ���Ⱑ ���� ��


	return false;
}

void UC_EquippedComponent::OnSheathEnd()
{
	// ���� ���� �������� ���̱�
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


