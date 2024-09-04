// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/MeleeWeapon/C_MeleeWeapon.h"
#include "Item/Weapon/WeaponStrategy/I_WeaponButtonStrategy.h"
#include "Item/Weapon/WeaponStrategy/C_MeleeWeaponStrategy.h"

#include "Character/C_BasicCharacter.h"
#include "Character/Component/C_EquippedComponent.h"

#include "Components/ShapeComponent.h"

#include "Kismet/GamePlayStatics.h"

#include "UObject/ConstructorHelpers.h"


AC_MeleeWeapon::AC_MeleeWeapon()
{
	WeaponButtonStrategy = CreateDefaultSubobject<AC_MeleeWeaponStrategy>("MeleeWeaponStrategy");

	AttackMontage.Priority = EMontagePriority::ATTACK;

	CurDrawMontage.Priority = EMontagePriority::DRAW_SHEATH_WEAPON;
	CurSheathMontage.Priority = EMontagePriority::DRAW_SHEATH_WEAPON;

	//ItemType 설정.
	MyItemType = EItemTypes::MELEEWEAPON;
}

void AC_MeleeWeapon::BeginPlay()
{
	Super::BeginPlay();

	AttackCollider = Cast<UShapeComponent>(GetComponentByClass(UShapeComponent::StaticClass()));
	AttackCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AttackCollider->OnComponentBeginOverlap.AddDynamic(this, &AC_MeleeWeapon::OnBodyColliderBeginOverlap);
}

void AC_MeleeWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (IsValid(OwnerCharacter))
	{
		CurDrawMontage   = DrawMontages[OwnerCharacter->GetPoseState()];
		CurSheathMontage = SheathMontages[OwnerCharacter->GetPoseState()];
	}

}

bool AC_MeleeWeapon::AttachToHolster(USceneComponent* InParent)
{
	return AttachToComponent
	(
		InParent,
		FAttachmentTransformRules(EAttachmentRule::KeepRelative, true),
		HOLSTER_SOCKET_NAME
	);
}

bool AC_MeleeWeapon::AttachToHand(USceneComponent* InParent)
{
	OwnerCharacter->SetHandState(EHandState::WEAPON_MELEE);

	return AttachToComponent
	(
		InParent,
		FAttachmentTransformRules(EAttachmentRule::KeepRelative, true),
		EQUIPPED_SOCKET_NAME
	);
}

void AC_MeleeWeapon::SetAttackColliderEnabled(const bool& Enabled)
{
	if (Enabled) AttackCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	else		 AttackCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AC_MeleeWeapon::OnBodyColliderBeginOverlap
(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	// TODO : 피격체에 데미지 주기

}

void AC_MeleeWeapon::OnOwnerCharacterPoseTransitionFin()
{
}

//void AC_MeleeWeapon::InitPriorityAnimMontages()
//{
//	AttackMontage.Priority = EMontagePriority::ATTACK;
//
//	/*FString leftMontagePath = FString::Printf
//	(
//		TEXT("/Game/Project_PUBG/DongHyun/Character/Animation/Turn_In_Place/Montages/%u%u_TurnLeft_Montage"),
//		handState,
//		poseState
//	);
//
//	ConstructorHelpers::FObjectFinder<UAnimMontage> TurnLeftMontage(*leftMontagePath);*/
//
//	//static ConstructorHelpers::FObjectFinder<UAnimMontage> attackAnimMontage
//	//(
//	//	TEXT("/Game/Project_PUBG/DongHyun/Character/Animation/WeaponMotions/MeleeMontages/MeleeAttack_Montage")
//	//);
//	//
//	//AttackMontage.AnimMontage = (attackAnimMontage.Succeeded()) ? attackAnimMontage.Object : nullptr;
//	//AttackMontage.Priority = EMontagePriority::ATTACK;
//
//	// Init Draw Montages
//
//	// Init Sheath Montages
//
//	//CurDrawMontage.Priority = EMontagePriority::DRAW_SHEATH_WEAPON;
//	//CurSheathMontage.Priority = EMontagePriority::DRAW_SHEATH_WEAPON;
//}
