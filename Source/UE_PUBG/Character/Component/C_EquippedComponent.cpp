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

	// Test�� weapon spawn, �������Ҹ� ���� ������
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
		// ���� ������ Sheath�� Draw animation montage�� �̹� ��� ���̶�� return
		if (OwnerCharacter->GetMesh()->GetAnimInstance()
			->Montage_IsPlaying(Weapons[CurWeaponType]->GetCurDrawMontage().AnimMontage)) 
			return false;

		if (OwnerCharacter->GetMesh()->GetAnimInstance()
			->Montage_IsPlaying(Weapons[CurWeaponType]->GetCurSheathMontage().AnimMontage))
			return false;
	}

	NextWeaponType = InChangeTo;

	if (CurWeaponType == NextWeaponType) return false; // ���� ����� ���� ���Ⱑ ���� �� ���⸦ ���� �ٽ� ������ ����

	// NextWeaponType�� None�� �ƴϰ�, �ش� ���� ���Կ� ���Ⱑ ���� ��
	if (NextWeaponType != EWeaponSlot::NONE && !IsValid(Weapons[NextWeaponType])) return false;

	// ���� ���⸦ ���������� ���� �� (UnArmed ����), ���� ���� Draw�� ���
	if (CurWeaponType == EWeaponSlot::NONE || !IsValid(GetCurWeapon()))
	{
		CurWeaponType = NextWeaponType;
		OwnerCharacter->PlayAnimMontage(Weapons[CurWeaponType]->GetCurDrawMontage());
		return true;
	}
	
	// ���� ���⸦ �������� ��Ȳ
	OwnerCharacter->PlayAnimMontage(Weapons[CurWeaponType]->GetCurSheathMontage()); // �� ���� ����ִ� ���ۿ� Notify�Լ� �ɾ ���� ����� ��ȯ

	return true;
}

bool UC_EquippedComponent::ToggleArmed()
{
	// ���� ���⵵ �������� �ʾҰ� ������ ��� �־��� ���⵵ ���� �� (�ʱ� ����)
	if (CurWeaponType == EWeaponSlot::NONE && PrevWeaponType == EWeaponSlot::NONE) return false;
	
	// ���� ��� �ִ� ���Ⱑ ���� ��
	if (CurWeaponType != EWeaponSlot::NONE && IsValid(GetCurWeapon()))
	{
		PrevWeaponType = CurWeaponType;
		return ChangeCurWeapon(EWeaponSlot::NONE);
	}

	// ���� ��� �ִ� ���Ⱑ ���� ��
	return ChangeCurWeapon(PrevWeaponType);
}

void UC_EquippedComponent::OnSheathEnd()
{
	// ���� ���� �������� ���̱�
	GetCurWeapon()->AttachToHolster(OwnerCharacter->GetMesh());

	CurWeaponType = NextWeaponType;
	
	if (!IsValid(GetCurWeapon()))
	{
		OwnerCharacter->SetHandState(EHandState::UNARMED);
		return;
	}

	OwnerCharacter->PlayAnimMontage(GetCurWeapon()->GetCurDrawMontage());
}

void UC_EquippedComponent::OnDrawEnd()
{
	GetCurWeapon()->AttachToHand(OwnerCharacter->GetMesh());

	NextWeaponType = EWeaponSlot::NONE;
}


