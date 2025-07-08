// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/C_PreviewCharacter.h"
#include "C_PreviewCharacter.h"
#include "Item/Weapon/C_Weapon.h"
#include "Item/Weapon/Gun/C_Gun.h"
#include "Character/Component/C_EquippedComponent.h"
#include "Character/Component/C_InvenComponent.h"
#include "Character/C_Player.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Utility/C_Util.h"
// Sets default values
AC_PreviewCharacter::AC_PreviewCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture"));
	SceneCapture->SetupAttachment(GetMesh());
}

// Called when the game starts or when spawned
void AC_PreviewCharacter::BeginPlay()
{
	Super::BeginPlay();
	previewMesh = this->GetMesh();

}

// Called every frame
void AC_PreviewCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AC_PreviewCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

bool AC_PreviewCharacter::AttachWeaponMesh(AC_Weapon* Weapon)
{
	if (!OwnerPlayer) return false;

	AC_Gun* Gun = Cast<AC_Gun>(Weapon);

	if (!Gun) return false;

	// 무기 없으면 제거만 하고 끝
	if (!Weapon)
	{
		if (PreviewMainWeaponMesh)
		{
			PreviewMainWeaponMesh->DestroyComponent();
			PreviewMainWeaponMesh = nullptr;
		}
		return true;
	}

	// 기존 메시 제거
	if (PreviewMainWeaponMesh)
	{
		PreviewMainWeaponMesh->DestroyComponent();
		PreviewMainWeaponMesh = nullptr;
	}

	// 메시 복사 생성
	USkeletalMeshComponent* WeaponMesh = Weapon->GetWeaponMesh();
	PreviewMainWeaponMesh = NewObject<USkeletalMeshComponent>(this, TEXT("PreviewWeaponMesh"));
	PreviewMainWeaponMesh->RegisterComponent();
	PreviewMainWeaponMesh->SetSkeletalMesh(WeaponMesh->SkeletalMesh);
	PreviewMainWeaponMesh->SetVisibility(true);
	PreviewMainWeaponMesh->SetHiddenInGame(false);

	// 머티리얼 복사
	int32 MatCount = Weapon->GetWeaponMesh()->GetNumMaterials();
	for (int32 i = 0; i < MatCount; ++i)
	{
		PreviewMainWeaponMesh->SetMaterial(i, WeaponMesh->GetMaterial(i));
	}

	// 부착 위치 결정
	AC_EquipableItem* curBackPack = OwnerPlayer->GetInvenComponent()->GetEquipmentItems()[EEquipSlot::BACKPACK];

	EGunState CurState = Cast<AC_Gun>(Weapon)->GetCurrentWeaponState();

	FName SocketName = NAME_None;

	if (!curBackPack)
	{
		SocketName = (CurState == EGunState::MAIN_GUN) ? MAIN_HOLSTER_SOCKET_NAME : SUB_HOLSTER_SOCKET_NAME;
	}
	else
	{
		SocketName = (CurState == EGunState::MAIN_GUN) ? MAIN_HOLSTER_BAG_SOCKET_NAME : SUB_HOLSTER_BAG_SOCKET_NAME;

	}

	if (CurState == EGunState::MAIN_GUN)
	{
		UC_Util::Print("CurState is Main Gun");
	}
	else
	{
		UC_Util::Print("CurState is Sub Gun");
	}

	// 부착
	PreviewMainWeaponMesh->AttachToComponent(previewMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), SocketName);
	if (SceneCapture && PreviewMainWeaponMesh)
	{
		SceneCapture->ShowOnlyComponents.Add(PreviewMainWeaponMesh);
	}

	return true;

}

void AC_PreviewCharacter::AttachHelmetMesh(AC_Weapon* Weapon)
{
}

void AC_PreviewCharacter::AttachArmorMesh(AC_Weapon* Weapon)
{
}

void AC_PreviewCharacter::AttachBackMesh(AC_Weapon* Weapon)
{
}

void AC_PreviewCharacter::AttachMeleeWeaponMesh(AC_Weapon* Weapon)
{
}


