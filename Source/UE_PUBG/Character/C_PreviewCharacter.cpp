// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/C_PreviewCharacter.h"
#include "C_PreviewCharacter.h"
#include "Item/Equipment/C_EquipableItem.h"
#include "Item/Weapon/C_Weapon.h"
#include "Item/Weapon/Gun/C_Gun.h"
#include "Item/Weapon/Gun/C_SR.h"
#include "Item/Weapon/MeleeWeapon/C_MeleeWeapon.h"
#include "Item/Weapon/ThrowingWeapon/C_ThrowingWeapon.h"
#include "Character/Component/C_EquippedComponent.h"
#include "Character/Component/C_InvenComponent.h"
#include "Character/C_Player.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Singleton/C_GameInstance.h"
#include "Utility/C_Util.h"
// Sets default values
AC_PreviewCharacter::AC_PreviewCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture"));
	
	//SceneCapture->SetupAttachment(GetMesh());
	SceneCapture->SetupAttachment(RootComponent);

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

	UC_GameInstance* GameInstance = Cast<UC_GameInstance>(GetGameInstance());
	if (!SpawnLocations.Contains(GameInstance->GetCurrentSelectedLevelType())) return;
	SetActorLocation(SpawnLocations[GameInstance->GetCurrentSelectedLevelType()]);
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

void AC_PreviewCharacter::SetWeaponMesh(EWeaponSlot InSlot, AC_Weapon* Weapon)
{
	AC_Weapon* PrevSlotWeapon = OwnerPlayer->GetEquippedComponent()->GetWeapons()[InSlot];

	if (PrevSlotWeapon)
	{
		// 이전 무기 메시 제거
		DetachWeaponMesh(InSlot);
	}
	if (Weapon)
	{
		if (WeaponMeshes.Contains(InSlot) && WeaponMeshes[InSlot])
		{
			// 기존 메시가 있다면 제거
			WeaponMeshes[InSlot] = Weapon->GetItemMeshComp();
		}
		else return;
	}

	// 무기 메시가 nullptr이 아니면 새로운 메시를 장착


}

bool AC_PreviewCharacter::AttachMeleeWeaponMesh(FName InSocket)
{
	UC_EquippedComponent* EquippedComp = OwnerPlayer->GetEquippedComponent();

	// 기존 메시 제거
	DetachWeaponMesh(EWeaponSlot::MELEE_WEAPON);

	AC_MeleeWeapon* MeleeWeapon = Cast<AC_MeleeWeapon>(EquippedComp->GetWeapons()[EWeaponSlot::MELEE_WEAPON]);
	if (!MeleeWeapon) return false; // 무기가 없으면 false 리턴

	UStaticMeshComponent* SourceMesh = Cast<UStaticMeshComponent>(MeleeWeapon->GetItemMeshComp());
	if (!SourceMesh || !SourceMesh->GetStaticMesh()) return false;

	// Preview용 StaticMeshComponent 생성
	UStaticMeshComponent* NewMesh = NewObject<UStaticMeshComponent>(this, TEXT("PreviewMeleeWeaponMesh"));
	if (!NewMesh) return false;

	NewMesh->SetStaticMesh(SourceMesh->GetStaticMesh());
	
	EHandState HandState = OwnerPlayer->GetHandState();

	UC_Util::Print(FString::Printf(TEXT("HandState: %s"), *UEnum::GetValueAsString(HandState)), FColor::Blue, 10.f);

	FName SocketName = InSocket;

	UC_Util::Print(SocketName.ToString(), FColor::Blue, 10.f);

	// 부착
	NewMesh->AttachToComponent(previewCharacterMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), SocketName);

	NewMesh->RegisterComponent();
	// SceneCapture에 추가
	if (SceneCapture)
	{
		SceneCapture->ShowOnlyComponents.Add(NewMesh);
	}

	// TMap에 저장
	WeaponMeshes.Add(EWeaponSlot::MELEE_WEAPON, NewMesh);
	return true;
}

bool AC_PreviewCharacter::AttachGunMesh(EWeaponSlot InSlot, FName InSocket)
{
	if (!OwnerPlayer) return false;

	UC_EquippedComponent* EquippedComponent = OwnerPlayer->GetEquippedComponent();
	if (!EquippedComponent) return false;

	AC_Weapon* Weapon = EquippedComponent->GetWeapons()[InSlot];
	if (!Weapon)
	{
		return DetachWeaponMesh(InSlot); // 무기가 없으면 제거
	}

	//if (WeaponMeshes.Contains(InSlot) && WeaponMeshes[InSlot])
	//{
	//	WeaponMeshes[InSlot]->DestroyComponent();
	//	WeaponMeshes[InSlot] = nullptr;
	//}
	
	// 기존 메시 제거
	//DetachWeaponMesh(InSlot);

	// 무기 메시 가져오기
	USkeletalMeshComponent* WeaponMesh = Cast<USkeletalMeshComponent>(Weapon->GetItemMeshComp());
	if (!WeaponMesh || !WeaponMesh->SkeletalMesh)
	{
		UC_Util::Print("Weapon Mesh is nullptr");
		return false;
	}


	// 프리뷰 메시 생성
	FName MeshName = (InSlot == EWeaponSlot::MAIN_GUN) ? TEXT("PreviewMainWeaponMesh") : TEXT("PreviewSubWeaponMesh");
	USkeletalMeshComponent* NewMesh = NewObject<USkeletalMeshComponent>(this, MeshName);
	if (!NewMesh) return false;

	AC_EquipableItem* curBackPack = OwnerPlayer->GetInvenComponent()->GetEquipmentItems()[EEquipSlot::BACKPACK];

	AC_Gun* Gun = Cast<AC_Gun>(Weapon);


	//InSlot == EWeaponSlot::MAIN_GUN ? Gun->ChangeGunState(EGunState::MAIN_GUN) : Gun->ChangeGunState(EGunState::SUB_GUN);
	//EGunState CurState = Gun->GetCurrentWeaponState();
	
	NewMesh->SetSkeletalMesh(WeaponMesh->SkeletalMesh);


	if (AC_SR* SR = Cast<AC_SR>(Weapon))
	{
		//WeaponMesh->SetWorldScale3D(FVector(.6f, .6f, .6f));
		NewMesh->SetRelativeScale3D(FVector(0.6f));
		//NewMesh->SetWorldScale3D(FVector(.6f));
	}

	//NewMesh->AttachToComponent(previewCharacterMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), Weapon->GetAttachParentSocketName());
	//NewMesh->AttachToComponent(previewCharacterMesh,
	//	FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
	//	Weapon->GetAttachParentSocketName());

	NewMesh->AttachToComponent(previewCharacterMesh,
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		InSocket);

	NewMesh->RegisterComponent();

	// SceneCapture에 추가
	if (SceneCapture)
	{
		SceneCapture->ShowOnlyComponents.Add(NewMesh);
	}

	// TMap에 등록
	WeaponMeshes.Add(InSlot, NewMesh);


	return true;
}

bool AC_PreviewCharacter::AttachThrowableWeaponMesh(FName InSocket)
{
	UC_EquippedComponent* EquippedComp = OwnerPlayer->GetEquippedComponent();

	// 기존 메시 제거
	DetachWeaponMesh(EWeaponSlot::THROWABLE_WEAPON);

	AC_ThrowingWeapon* ThrowingWeapon = Cast<AC_ThrowingWeapon>(EquippedComp->GetWeapons()[EWeaponSlot::THROWABLE_WEAPON]);
	if (!ThrowingWeapon) return false; // 무기가 없으면 false 리턴

	UStaticMeshComponent* SourceMesh = Cast<UStaticMeshComponent>(ThrowingWeapon->GetItemMeshComp());
	if (!SourceMesh || !SourceMesh->GetStaticMesh()) return false;

	// Preview용 StaticMeshComponent 생성
	UStaticMeshComponent* NewMesh = NewObject<UStaticMeshComponent>(this, TEXT("PreviewThrowingWeaponMesh"));
	if (!NewMesh) return false;

	NewMesh->SetStaticMesh(SourceMesh->GetStaticMesh());

	EHandState HandState = OwnerPlayer->GetHandState();

	UC_Util::Print(FString::Printf(TEXT("HandState: %s"), *UEnum::GetValueAsString(HandState)), FColor::Blue, 10.f);

	FName SocketName = InSocket;

	NewMesh->AttachToComponent(previewCharacterMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), SocketName);
	NewMesh->SetVisibility(true);
	NewMesh->RegisterComponent();
	// SceneCapture에 추가
	if (SceneCapture)
	{
		SceneCapture->ShowOnlyComponents.Add(NewMesh);
	}

	// TMap에 저장
	WeaponMeshes.Add(EWeaponSlot::THROWABLE_WEAPON, NewMesh);

	return true;
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
}

bool AC_PreviewCharacter::DetachEquippedMesh(EEquipSlot InSlot)
{
	if (UStaticMeshComponent* curMesh = EquipMeshes.FindRef(InSlot))
	{
		if (SceneCapture)
		{
			SceneCapture->ShowOnlyComponents.Remove(curMesh);
		}

		curMesh->DestroyComponent();
		EquipMeshes.Remove(InSlot);
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
	//if (InSlot == EWeaponSlot::MELEE_WEAPON)
	//{
	//	return AttachMeleeWeaponMesh(InSlot);
	//}

	// 무기 메시 가져오기
	USkeletalMeshComponent* WeaponMesh = Cast<USkeletalMeshComponent>(Weapon->GetItemMeshComp());
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

	AC_Gun* Gun = Cast<AC_Gun>(Weapon);

	InSlot == EWeaponSlot::MAIN_GUN ? Gun->ChangeGunState(EGunState::MAIN_GUN) : Gun->ChangeGunState(EGunState::SUB_GUN);

	EGunState CurState = Gun->GetCurrentWeaponState();

	FName SocketName = NAME_None;

	if (!curBackPack)
	{
		SocketName = (CurState == EGunState::MAIN_GUN) ? MAIN_HOLSTER_SOCKET_NAME : SUB_HOLSTER_SOCKET_NAME;
	}
	else
	{
		SocketName = (CurState == EGunState::MAIN_GUN) ? MAIN_HOLSTER_BAG_SOCKET_NAME : SUB_HOLSTER_BAG_SOCKET_NAME;
	}
	SocketName = Weapon->GetAttachParentSocketName();
	NewMesh->SetSkeletalMesh(WeaponMesh->SkeletalMesh);
	

	if (AC_SR* SR = Cast<AC_SR>(Weapon))
	{
		//WeaponMesh->SetWorldScale3D(FVector(.6f, .6f, .6f));
		NewMesh->SetRelativeScale3D(FVector(0.6f));
		//NewMesh->SetWorldScale3D(FVector(.6f));
	}


	//NewMesh->AttachToComponent(previewCharacterMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), Weapon->GetAttachParentSocketName());
	//NewMesh->AttachToComponent(previewCharacterMesh,
	//	FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
	//	Weapon->GetAttachParentSocketName());
	
	NewMesh->AttachToComponent(previewCharacterMesh,
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		Weapon->GetAttachParentSocketName());

	NewMesh->RegisterComponent();

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
	if (!OwnerPlayer) return false;

	UC_InvenComponent* InvenComp = OwnerPlayer->GetInvenComponent();
	if (!InvenComp) return false;

	AC_EquipableItem* Item = InvenComp->GetEquipmentItems()[InSlot];
	if (!Item) return DetachEquippedMesh(InSlot);  // 장착된 아이템 없으면 제거

	// 기존 메시 제거
	DetachEquippedMesh(InSlot);

	// 메시 가져오기
	UStaticMeshComponent* SourceMesh = Cast<UStaticMeshComponent>(Item->GetItemMeshComp());
	if (!SourceMesh || !SourceMesh->GetStaticMesh()) return false;

	// Preview 메시 생성
	FName CompName = *FString::Printf(TEXT("PreviewEquip_%d"), static_cast<int32>(InSlot));
	UStaticMeshComponent* PreviewMesh = NewObject<UStaticMeshComponent>(this, CompName);
	if (!PreviewMesh) return false;

	PreviewMesh->SetStaticMesh(SourceMesh->GetStaticMesh());

	// 부착
	PreviewMesh->AttachToComponent(previewCharacterMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), Item->GetSocketName());

	PreviewMesh->RegisterComponent();
	// SceneCapture에 추가
	if (SceneCapture)
	{
		SceneCapture->ShowOnlyComponents.Add(PreviewMesh);
	}

	// TMap에 저장
	EquipMeshes.Add(InSlot, PreviewMesh);
	return true;
}

bool AC_PreviewCharacter::Update(EHandState InState, EWeaponSlot InSlot, FName InSocket)
{
	switch (InState)
	{
	case EHandState::UNARMED:
		DetachWeaponMesh(OwnerPlayer->GetEquippedComponent()->GetCurWeaponType());
		break;
	case EHandState::WEAPON_GUN:
		AttachGunMesh(InSlot, InSocket);
		break;
	case EHandState::WEAPON_MELEE:
		AttachMeleeWeaponMesh(InSocket);
		break;
	case EHandState::WEAPON_THROWABLE:
		AttachThrowableWeaponMesh(InSocket);
		break;
	case EHandState::HANDSTATE_MAX:
		break;
	default:
		break;
	}
	UpdateHandPose(InState);
	return true;
}

bool AC_PreviewCharacter::SwapSlotsWhileGunHandState()
{
	UC_EquippedComponent* EquipComp = OwnerPlayer->GetEquippedComponent();
	if (!EquipComp) return false;

	EWeaponSlot CurWeaponType = EquipComp->GetCurWeaponType();

	// 기존 Mesh 포인터
	USceneComponent* CurMainGunMesh = WeaponMeshes[EWeaponSlot::MAIN_GUN];
	USceneComponent* CurSubGunMesh = WeaponMeshes[EWeaponSlot::SUB_GUN];

	// 기존 메시 정리
	if (CurMainGunMesh) CurMainGunMesh->DestroyComponent();
	if (CurSubGunMesh)  CurSubGunMesh->DestroyComponent();

	WeaponMeshes[EWeaponSlot::MAIN_GUN] = nullptr;
	WeaponMeshes[EWeaponSlot::SUB_GUN] = nullptr;

	// 스왑 로직
	if (CurWeaponType == EWeaponSlot::MAIN_GUN)
	{
		AC_Gun* SubGun = Cast<AC_Gun>(EquipComp->GetWeapons()[EWeaponSlot::SUB_GUN]);
		if (!SubGun) return false; //SubGun이 없으면 새 메시 만들지 않음
		return AttachGunMesh(EWeaponSlot::SUB_GUN, SubGun->GetEQUIPPED_SOCKET_NAME());
	}
	else if (CurWeaponType == EWeaponSlot::SUB_GUN)
	{
		AC_Gun* MainGun = Cast<AC_Gun>(EquipComp->GetWeapons()[EWeaponSlot::MAIN_GUN]);
		if (!MainGun) return false; //MainGun이 없으면 새 메시 만들지 않음
		return AttachGunMesh(EWeaponSlot::MAIN_GUN, MainGun->GetEQUIPPED_SOCKET_NAME());
	}

	return false;
}


