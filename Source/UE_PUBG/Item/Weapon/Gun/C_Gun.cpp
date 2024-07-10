// Fill out your copyright notice in the Description page of Project Settings.

#include "Item/Weapon/Gun/C_Gun.h"
#include "Character/C_BasicCharacter.h"
#include "Character/Component/C_EquippedComponent.h"
#include "GameFramework/Actor.h"
#include "Components/ShapeComponent.h"
#include "Components/SceneComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"

// Sets default values
AC_Gun::AC_Gun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AC_Gun::BeginPlay()
{
	Super::BeginPlay();

}

void AC_Gun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CurDrawMontage = DrawMontages[OwnerCharacter->GetPoseState()].Montages[CurState];
	CurSheathMontage = SheathMontages[OwnerCharacter->GetPoseState()].Montages[CurState];
}

bool AC_Gun::AttachToHolster(USceneComponent* InParent)
{
	switch (CurState)
	{
	case EGunState::MAIN_GUN:
		return AttachToComponent
		(
			InParent,
			FAttachmentTransformRules(EAttachmentRule::KeepRelative, true),
			MAIN_HOLSTER_SOCKET_NAME
		);
		break;
	case EGunState::SUB_GUN:
		return AttachToComponent
		(
			InParent,
			FAttachmentTransformRules(EAttachmentRule::KeepRelative, true),
			SUB_HOLSTER_SOCKET_NAME
		);
		break;
	default:
		return false;
		break;
	}

}

bool AC_Gun::AttachToHand(USceneComponent* InParent)
{
	UAnimMontage* DrawMontage = DrawMontages[OwnerCharacter->GetPoseState()].Montages[EGunState::SUB_GUN].AnimMontage;
	//UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();

	//if (!IsValid(DrawMontage)) return false;
	// ���� SUB_GUN�� Draw�� ����ȴٸ� �޼����� ���� Attach
	//���� �޼����� �Ű����� �ִٸ� �ٽ� ����ġ ���� �ʰ� If�� ������ ����
	FName CurSocketName = GetAttachParentSocketName();
	if (OwnerCharacter->GetMesh()->GetAnimInstance()->Montage_IsPlaying(DrawMontage) && CurSocketName != SUB_DRAW_SOCKET_NAME)
	{
		return AttachToComponent
		(
			InParent,
			FAttachmentTransformRules(EAttachmentRule::KeepRelative, true),
			SUB_DRAW_SOCKET_NAME
		);
	}

	OwnerCharacter->SetHandState(EHandState::WEAWPON_GUN);


	return AttachToComponent
	(
		InParent,
		FAttachmentTransformRules(EAttachmentRule::KeepRelative, true),
		EQUIPPED_SOCKET_NAME
	);

}


