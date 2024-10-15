// Fill out your copyright notice in the Description page of Project Settings.



#include "Item/Weapon/C_Weapon.h"
#include "UE_PUBG/Item/C_Item.h"
#include "Character/Component/C_EquippedComponent.h"
#include "Character/C_BasicCharacter.h"
#include "Gun/C_Gun.h"
#include "MeleeWeapon/C_MeleeWeapon.h"
#include "ThrowingWeapon/C_ThrowingWeapon.h"
#include "Item/Weapon/WeaponStrategy/I_WeaponButtonStrategy.h"


// Sets default values
AC_Weapon::AC_Weapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AC_Weapon::BeginPlay()
{
	Super::BeginPlay();

	InitialRelativeTransform = RootComponent->GetRelativeTransform();
}

// Called every frame
void AC_Weapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

bool AC_Weapon::ExecuteBKey()
{
	if (!WeaponButtonStrategy)
	{
		FString DebugMessage = "In AC_Weapon::ExecuteBKey : WeaponStrategy nullptr!";
		GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, *DebugMessage);

		return false;
	}
	return WeaponButtonStrategy->UseBKeyStrategy(OwnerCharacter, this);
}

bool AC_Weapon::ExecuteRKey()
{
	if (!WeaponButtonStrategy)
	{
		FString DebugMessage = "In AC_Weapon::ExecuteRKey : WeaponStrategy nullptr!";
		GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, *DebugMessage);

		return false;
	}
	return WeaponButtonStrategy->UseRKeyStrategy(OwnerCharacter, this);
}

bool AC_Weapon::ExecuteMlb_Started()
{
	if (!WeaponButtonStrategy)
	{
		FString DebugMessage = "In AC_Weapon::ExecuteMlb_Started : WeaponStrategy nullptr!";
		GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, *DebugMessage);

		return false;
	}
	return WeaponButtonStrategy->UseMlb_StartedStrategy(OwnerCharacter, this);
}

bool AC_Weapon::ExecuteMlb_OnGoing()
{
	if (!WeaponButtonStrategy) return false;
	return WeaponButtonStrategy->UseMlb_OnGoingStrategy(OwnerCharacter, this);
}

bool AC_Weapon::ExecuteMlb_Completed()
{
	if (!WeaponButtonStrategy) return false;
	return WeaponButtonStrategy->UseMlb_CompletedStrategy(OwnerCharacter, this);
}

bool AC_Weapon::ExecuteMrb_Started()
{
	if (!WeaponButtonStrategy)
	{
		FString DebugMessage = "In AC_Weapon::ExecuteMlb_Started : WeaponStrategy nullptr!";
		GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, *DebugMessage);

		return false;
	}
	return WeaponButtonStrategy->UseMrb_StartedStrategy(OwnerCharacter, this);
}

bool AC_Weapon::ExecuteMrb_OnGoing()
{
	if (!WeaponButtonStrategy) return false;
	return WeaponButtonStrategy->UseMrb_OnGoingStrategy(OwnerCharacter, this);
}

bool AC_Weapon::ExecuteMrb_Completed()
{
	if (!WeaponButtonStrategy) return false;
	return WeaponButtonStrategy->UseMrb_CompletedStrategy(OwnerCharacter, this);
}

void AC_Weapon::PickUpItem(AC_BasicCharacter* Character)
{
	EItemTypes Type = ItemDatas.ItemType;

	switch (Type)
	{
	case EItemTypes::MAINGUN:
		PickUpItem(OwnerCharacter);
		break;
	case EItemTypes::MELEEWEAPON:
		PickUpItem(OwnerCharacter);
		break;
	case EItemTypes::THROWABLE:
		PickUpItem(OwnerCharacter);
		break;
	default:
		break;
	}
}

