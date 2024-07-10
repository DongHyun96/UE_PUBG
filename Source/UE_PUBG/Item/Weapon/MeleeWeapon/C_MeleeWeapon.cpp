// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/MeleeWeapon/C_MeleeWeapon.h"
#include "Item/Weapon/WeaponStrategy/I_WeaponButtonStrategy.h"
#include "Item/Weapon/WeaponStrategy/C_MeleeWeaponStrategy.h"

#include "Character/C_BasicCharacter.h"
#include "Character/Component/C_EquippedComponent.h"

#include "Components/ShapeComponent.h"

#include "Kismet/GamePlayStatics.h"


AC_MeleeWeapon::AC_MeleeWeapon()
{
	WeaponButtonStrategy = CreateDefaultSubobject<AC_MeleeWeaponStrategy>("MeleeWeaponStrategy");

	AttackMontage.Priority = EMontagePriority::ATTACK;

	CurDrawMontage.Priority = EMontagePriority::DRAW_SHEATH_WEAPON;
	CurSheathMontage.Priority = EMontagePriority::DRAW_SHEATH_WEAPON;
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

	CurDrawMontage   = DrawMontages[OwnerCharacter->GetPoseState()];
	CurSheathMontage = SheathMontages[OwnerCharacter->GetPoseState()];

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
