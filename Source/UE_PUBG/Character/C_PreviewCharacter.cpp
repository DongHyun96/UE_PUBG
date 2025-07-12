// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/C_PreviewCharacter.h"
#include "C_PreviewCharacter.h"
#include "Item/Weapon/C_Weapon.h"
#include "Item/Weapon/Gun/C_Gun.h"
#include "Item/Weapon/MeleeWeapon/C_MeleeWeapon.h"
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

	WeaponMeshes.Add(EWeaponSlot::MAIN_GUN, PreviewMainWeaponMesh);
	WeaponMeshes.Add(EWeaponSlot::SUB_GUN, PreviewSubWeaponMesh);
	WeaponMeshes.Add(EWeaponSlot::MELEE_WEAPON, PreviewMeleeWeaponMesh);

	EquipMeshes.Add(EEquipSlot::HELMET, PreviewHelmetMesh);
	EquipMeshes.Add(EEquipSlot::VEST, PreviewArmorMesh);
	EquipMeshes.Add(EEquipSlot::BACKPACK, PreviewBackpackMesh);
}

// Called when the game starts or when spawned
void AC_PreviewCharacter::BeginPlay()
{
	Super::BeginPlay();
	previewCharacterMesh = this->GetMesh();




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

bool AC_PreviewCharacter::AttachWeaponMesh(AC_Weapon* Weapon, EWeaponSlot InSlot)
{
	if (!OwnerPlayer) return false;

	UC_EquippedComponent* EquippedComponent = OwnerPlayer->GetEquippedComponent();

	if (!EquippedComponent) return false;

	//if (!Weapon) return DetachWeaponMesh(InSlot); //이 부분은 현재 EquipComp의 SetSlotWeapon에서 처리됨

	if (!Weapon) return false;

	USkeletalMeshComponent* WeaponMesh = nullptr;
	USkeletalMeshComponent* PreWeaponMesh = nullptr;

	switch (InSlot)
	{
	case EWeaponSlot::MAIN_GUN:
		WeaponMesh = Cast<USkeletalMeshComponent>(Weapon->GetWeaponMeshComp());

		PreviewMainWeaponMesh = NewObject<USkeletalMeshComponent>(this, TEXT("PreviewMainWeaponMesh"));
		PreviewMainWeaponMesh->RegisterComponent();
		PreviewMainWeaponMesh->SetSkeletalMesh(WeaponMesh->SkeletalMesh);
		PreWeaponMesh = PreviewMainWeaponMesh;
		break;
	case EWeaponSlot::SUB_GUN:
		WeaponMesh = Cast<USkeletalMeshComponent>(Weapon->GetWeaponMeshComp());

		PreviewSubWeaponMesh = NewObject<USkeletalMeshComponent>(this, TEXT("PreviewSubWeaponMesh"));
		PreviewSubWeaponMesh->RegisterComponent();
		PreviewSubWeaponMesh->SetSkeletalMesh(WeaponMesh->SkeletalMesh);
		PreWeaponMesh = PreviewSubWeaponMesh;
		break;
	case EWeaponSlot::MELEE_WEAPON:
		// TODO : 근접 무기 메시 처리, 근접 무기는 SkeletalMesh가 아닌 StaticMesh로 처리할 예정
		return AttachMeleeWeaponMesh(Weapon);
		break;
	default:
		return false;
	}

	// 머티리얼 복사
	//int32 MatCount = Weapon->GetWeaponMesh()->GetNumMaterials();
	//for (int32 i = 0; i < MatCount; ++i)
	//{
	//	PreWeaponMesh->SetMaterial(i, WeaponMesh->GetMaterial(i));
	//}

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

	PreWeaponMesh->AttachToComponent(previewCharacterMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), SocketName);

	if (SceneCapture && PreWeaponMesh)
	{
		SceneCapture->ShowOnlyComponents.Add(PreWeaponMesh);
		return true;
	}
	else
	{
		if (PreWeaponMesh)
		{
			PreWeaponMesh->DestroyComponent();
		}
		return false;
	}
}

bool AC_PreviewCharacter::AttachHelmetMesh(AC_EquipableItem* InItem)
{
	if (!OwnerPlayer) return false;

	UC_EquippedComponent* EquippedComp = OwnerPlayer->GetEquippedComponent();

	UC_InvenComponent* InvenComp = OwnerPlayer->GetInvenComponent();

	if (!InvenComp) return false;

	AC_EquipableItem* curHelmet = InvenComp->GetEquipmentItems()[EEquipSlot::HELMET];

	if (!curHelmet) return DetachHelmetMesh();

	return false;
}

bool AC_PreviewCharacter::AttachArmorMesh(AC_EquipableItem* InItem)
{
	if (!OwnerPlayer) return false;

	UC_EquippedComponent* EquippedComp = OwnerPlayer->GetEquippedComponent();

	if (!EquippedComp) return false;

	return false;

}

bool AC_PreviewCharacter::AttachBackMesh(AC_EquipableItem* InItem)
{
	if (!OwnerPlayer) return false;

	UC_EquippedComponent* EquippedComp = OwnerPlayer->GetEquippedComponent();

	if (!EquippedComp) return false;

	return false;

}

bool AC_PreviewCharacter::AttachMeleeWeaponMesh()
{
	UC_EquippedComponent* EquippedComp = OwnerPlayer->GetEquippedComponent();

	// 기존 메시 제거
	DetachWeaponMesh(EWeaponSlot::MELEE_WEAPON);

	AC_MeleeWeapon* MeleeWeapon = Cast<AC_MeleeWeapon>(EquippedComp->GetWeapons()[EWeaponSlot::MELEE_WEAPON]);
	if (!MeleeWeapon) return false; // 무기가 없으면 false 리턴

	UStaticMeshComponent* SourceMesh = Cast<UStaticMeshComponent>(MeleeWeapon->GetWeaponMeshComp());
	if (!SourceMesh || !SourceMesh->GetStaticMesh()) return false;

	// Preview용 StaticMeshComponent 생성
	UStaticMeshComponent* NewMesh = NewObject<UStaticMeshComponent>(this, TEXT("PreviewMeleeWeaponMesh"));
	if (!NewMesh) return false;

	NewMesh->RegisterComponent();
	NewMesh->SetStaticMesh(SourceMesh->GetStaticMesh());

	// 부착
	NewMesh->AttachToComponent(previewCharacterMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), MeleeWeapon->GetHolsterSocketName());

	// SceneCapture에 추가
	if (SceneCapture)
	{
		SceneCapture->ShowOnlyComponents.Add(NewMesh);
	}

	// TMap에 저장
	WeaponMeshes.Add(EWeaponSlot::MELEE_WEAPON, NewMesh);
	return true;

	//if (!Weapon) return false;

	//AC_MeleeWeapon* MeleeWeapon = Cast<AC_MeleeWeapon>(Weapon);

	//UStaticMeshComponent* WeaponMesh = Cast<UStaticMeshComponent>(MeleeWeapon->GetWeaponMeshComp());

	//PreviewMeleeWeaponMesh = NewObject<UStaticMeshComponent>(this, TEXT("PreviewMeleeWeaponMesh"));
	//PreviewMeleeWeaponMesh->RegisterComponent();
	//PreviewMeleeWeaponMesh->SetStaticMesh(WeaponMesh->GetStaticMesh());

	//PreviewMeleeWeaponMesh->AttachToComponent(previewCharacterMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), MeleeWeapon->GetHolsterSocketName());

	//if (SceneCapture && PreviewMeleeWeaponMesh)
	//{
	//	SceneCapture->ShowOnlyComponents.Add(PreviewMeleeWeaponMesh);
	//	return true;
	//}
	//else
	//{
	//	if (PreviewMeleeWeaponMesh)
	//	{
	//		PreviewMeleeWeaponMesh->DestroyComponent();
	//	}
	//	return false;
	//}
}

bool AC_PreviewCharacter::DetachWeaponMesh(EWeaponSlot InSlot)
{

	if (WeaponMeshes.Contains(InSlot) && WeaponMeshes[InSlot])
	{
		UPrimitiveComponent* Comp = Cast<UPrimitiveComponent>(WeaponMeshes[InSlot]);
		if (SceneCapture)
		{
			SceneCapture->ShowOnlyComponents.Remove(Comp);
		}

		Comp->DestroyComponent();
		WeaponMeshes[InSlot] = nullptr;
		return true;
	}

	return false;
	//switch (InSlot)
	//{
	//case EWeaponSlot::MAIN_GUN:
	//	if (PreviewMainWeaponMesh)
	//	{
	//		WeaponMeshes[InSlot]->DestroyComponent();
	//		WeaponMeshes[InSlot] = nullptr; // 맵에서 제거
	//		PreviewMainWeaponMesh->DestroyComponent();
	//		PreviewMainWeaponMesh = nullptr;
	//		return true;
	//	}
	//	break;
	//case EWeaponSlot::SUB_GUN:
	//	if (PreviewSubWeaponMesh)
	//	{
	//		WeaponMeshes[InSlot]->DestroyComponent();
	//		WeaponMeshes[InSlot] = nullptr; // 맵에서 제거
	//		PreviewSubWeaponMesh->DestroyComponent();
	//		PreviewSubWeaponMesh = nullptr;
	//		return true;
	//	}
	//	break;
	//case EWeaponSlot::MELEE_WEAPON:
	//	if (PreviewMeleeWeaponMesh)
	//	{
	//		PreviewMeleeWeaponMesh->DestroyComponent();
	//		PreviewMeleeWeaponMesh = nullptr;
	//		return true;
	//	}
	//	break;
	//default:
	//	return false;
	//}
	//return true; //TODO : 뭘로 리턴 할지 고민
}

bool AC_PreviewCharacter::DetachHelmetMesh()
{
	UC_InvenComponent* InvenComp = OwnerPlayer->GetInvenComponent();
	if (!InvenComp) return false;
	AC_EquipableItem* curHelmet = InvenComp->GetEquipmentItems()[EEquipSlot::HELMET];

	if (PreviewHelmetMesh)
	{
		PreviewHelmetMesh->DestroyComponent();
		PreviewHelmetMesh = nullptr;
		return true;
	}
	return false;
}

bool AC_PreviewCharacter::DetachArmorMesh()
{
	UC_InvenComponent* InvenComp = OwnerPlayer->GetInvenComponent();
	if (!InvenComp) return false;
	AC_EquipableItem* curAmor = InvenComp->GetEquipmentItems()[EEquipSlot::VEST];

	if (PreviewArmorMesh)
	{
		PreviewArmorMesh->DestroyComponent();
		PreviewArmorMesh = nullptr;
		return true;
	}
	return false;
}

bool AC_PreviewCharacter::DetachBackMesh()
{
	UC_InvenComponent* InvenComp = OwnerPlayer->GetInvenComponent();
	if (!InvenComp) return false;
	AC_EquipableItem* curBackPack = InvenComp->GetEquipmentItems()[EEquipSlot::BACKPACK];

	if (PreviewBackpackMesh)
	{
		PreviewBackpackMesh->DestroyComponent();
		PreviewBackpackMesh = nullptr;
		return true;
	}
	return false;
}

bool AC_PreviewCharacter::UpdateWeaponMesh(EWeaponSlot InSlot)
{
	if (!OwnerPlayer) return false;

	UC_EquippedComponent* EquippedComponent = OwnerPlayer->GetEquippedComponent();
	if (!EquippedComponent) return false;

	AC_Weapon* Weapon = EquippedComponent->GetWeapons()[InSlot];
	if (!Weapon)
	{
		return DetachWeaponMesh(InSlot); // 무기가 없으면 제거
	}

	// 근접 무기 예외 처리
	if (InSlot == EWeaponSlot::MELEE_WEAPON)
	{
		return AttachMeleeWeaponMesh(Weapon);
	}

	// 무기 메시 가져오기
	USkeletalMeshComponent* WeaponMesh = Cast<USkeletalMeshComponent>(Weapon->GetWeaponMeshComp());
	if (!WeaponMesh || !WeaponMesh->SkeletalMesh)
	{
		UC_Util::Print("Weapon Mesh is nullptr");
		return false;
	}

	// 기존 메시 제거
	DetachWeaponMesh(InSlot);

	// 프리뷰 메시 생성
	FName MeshName = (InSlot == EWeaponSlot::MAIN_GUN) ? TEXT("PreviewMainWeaponMesh") : TEXT("PreviewSubWeaponMesh");
	USkeletalMeshComponent* NewMesh = NewObject<USkeletalMeshComponent>(this, MeshName);
	if (!NewMesh) return false;

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

	NewMesh->RegisterComponent();
	NewMesh->SetSkeletalMesh(WeaponMesh->SkeletalMesh);
	NewMesh->AttachToComponent(previewCharacterMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), SocketName);

	// SceneCapture에 추가
	if (SceneCapture)
	{
		SceneCapture->ShowOnlyComponents.Add(NewMesh);
	}

	// TMap에 등록
	WeaponMeshes.Add(InSlot, NewMesh);
	return true;
}

bool AC_PreviewCharacter::UpdateEquippedMesh(EEquipSlot InSlot)
{
	return false;
}

