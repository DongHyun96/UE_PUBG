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
	// ?¤ì–´???¬ë¡¯???´ì „ ë¬´ê¸°???€??PoseTransitionEnd ?¸ë¦¬ê²Œì´???´ì œ
	if (Weapons[InSlot]) 
		OwnerCharacter->Delegate_OnPoseTransitionFin.RemoveAll(Weapons[InSlot]);

	Weapons[InSlot] = Weapon;

	// ?ˆë¡œ ?¥ì°©??ë¬´ê¸°???€??PoseTransitionEnd ?¸ë¦¬ê²Œì´??callback ê±¸ê¸°
	if (Weapons[InSlot]) 
		OwnerCharacter->Delegate_OnPoseTransitionFin.AddUObject(Weapons[InSlot], &AC_Weapon::OnOwnerCharacterPoseTransitionFin);
}

bool UC_EquippedComponent::ChangeCurWeapon(EWeaponSlot InChangeTo)
{
	if (IsValid(Weapons[CurWeaponType]))
	{
		// ?„ì¬ ë¬´ê¸°??Sheath??Draw animation montageê°€ ?´ë? ?¬ìƒ ì¤‘ì´?¼ë©´ return
		if (OwnerCharacter->GetMesh()->GetAnimInstance()
			->Montage_IsPlaying(Weapons[CurWeaponType]->GetCurDrawMontage().AnimMontage)) 
			return false;

		if (OwnerCharacter->GetMesh()->GetAnimInstance()
			->Montage_IsPlaying(Weapons[CurWeaponType]->GetCurSheathMontage().AnimMontage))
			return false;
	}

	NextWeaponType = InChangeTo;

	if (CurWeaponType == NextWeaponType) return false; // ?„ì¬ ë¬´ê¸°?€ ?¤ìŒ ë¬´ê¸°ê°€ ê°™ì„ ??ë¬´ê¸°ë¥?êµ³ì´ ?¤ì‹œ êº¼ë‚´ì§€ ?ŠìŒ

	// NextWeaponType??None???„ë‹ˆê³? ë°”ê¾¸?¤ëŠ” ë¬´ê¸° ?¬ë¡¯??ë¬´ê¸°ê°€ ?†ì„ ??
	if (NextWeaponType != EWeaponSlot::NONE && !IsValid(Weapons[NextWeaponType])) return false;

	// ?„ì¬ ë¬´ê¸°ë¥?ì°©ìš©ì¤‘ì´ì§€ ?Šì„ ??(UnArmed ?íƒœ), ?ëŠ” ?„ì¬ ?¬ë¡¯???¥ì°©??ë¬´ê¸°ê°€ ?†ì„ ???¤ìŒ ë¬´ê¸° Drawë§??¬ìƒ
	if (CurWeaponType == EWeaponSlot::NONE || !IsValid(GetCurWeapon()))
	{
		// ë§Œì•½ ?¤ìŒ??ë°”ê? ë¬´ê¸°ê°€ None?´ê±°???¤ìŒ??ë°”ê? ë¬´ê¸° ?¬ë¡¯??ë¬´ê¸°ê°€ ?†ì„ ??
		if (NextWeaponType == EWeaponSlot::NONE || !IsValid(Weapons[NextWeaponType]))
		{
			CurWeaponType = EWeaponSlot::NONE;
			NextWeaponType = EWeaponSlot::NONE;
			OwnerCharacter->SetHandState(EHandState::UNARMED);
			return false;
		}

		 // ?¤ìŒ ë¬´ê¸°ê°€ ?ˆì„ ??
		OwnerCharacter->PlayAnimMontage(Weapons[NextWeaponType]->GetCurDrawMontage());
		return true;
	}
	
	// ?„ì¬ ë¬´ê¸°ë¥?ì°©ìš©ì¤‘ì¸ ?í™©

	// ?¬ì²™ë¥??ˆì™¸ì²˜ë¦¬
	if (CurWeaponType == EWeaponSlot::THROWABLE_WEAPON)
	{
		AC_ThrowingWeapon* ThrowingWeapon = Cast<AC_ThrowingWeapon>(GetCurWeapon());
		if (IsValid(ThrowingWeapon))
		{
			// ?´ë? ì¿ í‚¹???œì‘?˜ì—ˆê³? ?„ì§ ?ì—??? ë‚˜ì§€ ?Šì? ?¬ì²™ë¥˜ë¼ë©??…ì— ?¨êµ¼
			if (ThrowingWeapon->GetIsCooked() && ThrowingWeapon->GetAttachParentActor())
				return ThrowingWeapon->ReleaseOnGround();
		}
	}
	//ì´ì„ ?¤ê³  Aiming ì¤‘ì¼ ??ì¹´ë©”???¤ì‹œ ?ë˜?€ë¡??„í™˜
	if (CurWeaponType == EWeaponSlot::MAIN_GUN || CurWeaponType == EWeaponSlot::SUB_GUN)
	{
		AC_Gun* TempWeapon = Cast<AC_Gun>(Weapons[CurWeaponType]);
		if (IsValid(TempWeapon))
			TempWeapon->BackToMainCamera();
	}

	OwnerCharacter->PlayAnimMontage(Weapons[CurWeaponType]->GetCurSheathMontage()); // ??ë¬´ê¸° ì§‘ì–´?£ëŠ” ?™ì‘??Notify?¨ìˆ˜ ê±¸ì–´???¤ìŒ ë¬´ê¸°ë¡??„í™˜
	return true;
}

bool UC_EquippedComponent::ToggleArmed()
{
	// ?„ì¬ ë¬´ê¸°???¥ì°©?˜ì? ?Šì•˜ê³??´ì „???¤ê³  ?ˆì—ˆ??ë¬´ê¸°???†ì„ ??(ì´ˆê¸° ?íƒœ)
	if (CurWeaponType == EWeaponSlot::NONE && PrevWeaponType == EWeaponSlot::NONE) return false;
	
	// ?„ì¬ ?¤ê³  ?ˆëŠ” ë¬´ê¸°ê°€ ?ˆì„ ??
	if (CurWeaponType != EWeaponSlot::NONE && IsValid(GetCurWeapon()))
	{
		PrevWeaponType = CurWeaponType;


		return ChangeCurWeapon(EWeaponSlot::NONE);
	}

	// ?„ì¬ ?¤ê³  ?ˆëŠ” ë¬´ê¸°ê°€ ?†ì„ ??
	return ChangeCurWeapon(PrevWeaponType);
}

void UC_EquippedComponent::OnSheathEnd()
{
	// ?„ì¬ ë¬´ê¸° ë¬´ê¸°ì§‘ì— ë¶™ì´ê¸?
	GetCurWeapon()->AttachToHolster(OwnerCharacter->GetMesh());

	// ì´ê¸°ë¥??ˆì™¸ì²˜ë¦¬
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


