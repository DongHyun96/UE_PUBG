

// Fill out your copyright notice in the Description page of Project Settings.

#include "Item/Weapon/Gun/C_Gun.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Item/Weapon/C_Weapon.h"
#include "Item/Weapon/WeaponStrategy/C_GunStrategy.h"
#include "Item/Attachment/C_AttachableItem.h"
#include "Item/AttachmentActors/AttachmentActor.h"
#include "Item/Weapon/Gun/C_Bullet.h"
#include "Item/ItemBullet/C_Item_Bullet.h"
#include "Item/Weapon/Gun/C_SR.h"

#include "UMG.h"
#include "AI/C_EnemyAIController.h"

#include "Character/Component/C_EquippedComponent.h"
#include "Character/Component/C_InvenComponent.h"
#include "Character/Component/C_InvenSystem.h"
#include "Character/Component/C_CrosshairWidgetComponent.h"
#include "Character/Component/C_AttachableItemMeshComponent.h"

#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/ChildActorComponent.h"

#include "Utility/C_Util.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"

#include "Character/C_Player.h"
#include "Character/C_Enemy.h"

#include "GameFramework/Actor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "HUD/C_HUDWidget.h"
#include "HUD/C_AmmoWidget.h"

#include "Singleton/C_GameSceneManager.h"
#include "Singleton/C_GameInstance.h"

#include "InvenUI/Panel/ItemPanel/C_InventoryItemPanelWidget.h"
#include "InvenUI/Panel/ItemPanel/EquipmentPanel/C_EquipmentPanelWdiget.h"
#include "InvenUI/BasicItemSlot/WeaponSlot/GunSlot/C_GunSlotWidget.h"
#include "InvenUI/BasicItemSlot/WeaponSlot/GunSlot/C_MainGunSlotWidget.h"
#include "InvenUI/BasicItemSlot/WeaponSlot/GunSlot/C_SubGunSlotWidget.h"


#include "Item/ItemManager/C_ItemManager.h"


AC_Gun::AC_Gun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//PrimaryActorTick.bCanEverTick = false;
	WeaponButtonStrategy = CreateDefaultSubobject<AC_GunStrategy>("GunStrategy");
	//ItemType 설정.
	//Magazine = LoadObject<AC_AttachableItem>(nullptr, TEXT("/Game/Project_PUBG/Common/Weapon/GunWeapon/Magazine/BPC_Magazine.BPC_Magazine"));

	//ItemDataRef.ItemType = EItemTypes::MAINGUN;

	SetHolsterNames();

}

void AC_Gun::BeginPlay()
{
	Super::BeginPlay();
	InitializeItem(ItemCode);

	//Add Grip for Test
	//SetHolsterNames();
	//AttachedParts[EPartsName::GRIP] = Cast<UStaticMeshComponent>(GetDefaultSubobjectByName("VertgripMesh"));
	IronSightMesh = Cast<USkeletalMeshComponent>(GetDefaultSubobjectByName("IronSightMesh"));
	//IronSightMesh->SetHiddenInGame(true);
	SetBulletSpeed();
	AimSightCamera   = FindComponentByClass<UCameraComponent>();
	AimSightSpringArm = FindComponentByClass<USpringArmComponent>();
	if (AimSightCamera)
		AimSightCamera->SetActive(false);
	//블루프린트에서 할당한 Skeletal Mesh 찾아서 변수에 저장
	GunMesh = FindComponentByClass<USkeletalMeshComponent>();
	//GunMesh->SetupAttachment(RootComponent);
	
	LoadMagazine();

	if (IsValid(Magazine))
	{

		Magazine->AttachToComponent(
			GunMesh,
			FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
			MAGAZINE_SOCKET_NAME
		);
	}
	SetSightCameraSpringArmLocation(ScopeCameraLocations[EAttachmentNames::MAX]);
}


void AC_Gun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	

	//if (IsValid(Magazine))
	//{
	//	UC_Util::Print(Magazine->GetActorLocation());	
	//}

	if (!OwnerCharacter) return;

	CurDrawMontage = DrawMontages[OwnerCharacter->GetPoseState()].Montages[CurState];
	CurSheathMontage = SheathMontages[OwnerCharacter->GetPoseState()].Montages[CurState];
	//GunMesh->SetWorldRotation(OwnerCharacter->GetControlRotation());
	if (IsValid(GunMesh))
	{
		FVector OutLocation{};
		FRotator OutRotation{};
		FTransform TempVec{};
		FRotator AdditionalRotation = FRotator(0, 0, 0); // Yaw를 45도 회전시키는 예제

		if (IsValid(AttachedItem[EPartsName::GRIP]))
			TempVec = AttachedItem[EPartsName::GRIP]->GetAttachmentMesh()->GetSocketTransform("LeftHandSocket");
		else
			TempVec = GunMesh->GetSocketTransform("LeftHandSocket");
		//TempVec = GunMesh->GetSocketTransform("LeftHandSocket");

		OwnerCharacter->GetMesh()->TransformToBoneSpace("RightHand", TempVec.GetLocation(), TempVec.GetRotation().Rotator(), OutLocation, OutRotation);
		LeftHandSocketLocation.SetLocation(OutLocation);
		LeftHandSocketLocation.SetRotation(OutRotation.Quaternion());
		//LeftHandSocketLocation.ConcatenateRotation(AdditionalRotation.Quaternion());

		//FTransform RootTrasnform = OwnerCharacter->GetMesh()->GetSocketTransform("SpineGunSocket");
		//
		//LeftHandSocketLocation = TempVec.GetRelativeTransform(RootTrasnform);
		//FString TheFloatStr = FString::SanitizeFloat(LeftHandSocketLocation.GetLocation().X);
		//GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, *TheFloatStr);
	}
	HandleSpringArmRotation();
	GetPlayerIsAimDownOrNot();
	CheckPlayerIsRunning();
	//CheckBackPackLevelChange();
	ShowAndHideWhileAiming();
}

void AC_Gun::InitializeItem(FName NewItemCode)
{
	Super::InitializeItem(NewItemCode);
	//TODO : 나중에 ItemManager를 통해 아이템을 모두 관리하게 되면 ItemManager를 통해서 GunDataRef 정의해 주기.
	static const FString ContextString(TEXT("GunItem Lookup"));
	//UDataTable* GunDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Project_PUBG/Common/Item/ItemDataTables/DT_GunData.DT_GunData"));

	UC_GameInstance* GI = Cast<UC_GameInstance>(GetGameInstance());

	UDataTable* GunDataTable = GI->GetDataTables()[EDataTableType::Gun];

	if (GunDataTable)
	{
		const FGunData* GunData = GunDataTable->FindRow<FGunData>(ItemCode, ContextString);
		if (GunData)
		{
			GunDataRef = GunData;  // 원본 참조 저장
		}
	}

	//TODO : 나중에 ItemManager를 통해 아이템을 모두 관리하게 되면 ItemManager를 통해서 GunSoundData 정의해 주기.
	//UDataTable* GunSoundDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Project_PUBG/Common/Item/ItemDataTables/DT_GunSoundData.DT_GunSoundData"));

	UDataTable* GunSoundDataTable = GI->GetDataTables()[EDataTableType::GunSound];

	if (GunSoundDataTable)
	{
		const FGunSoundData* ItemData = GunSoundDataTable->FindRow<FGunSoundData>(ItemCode, ContextString);
		if (ItemData)
		{
			GunSoundData = ItemData;  // 원본 참조 저장
		}
	}
}

bool AC_Gun::AttachToHolster(USceneComponent* InParent)
{
	if (!IsValid(OwnerCharacter)) return false;

	if (AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharacter))
		OwnerPlayer->GetCrosshairWidgetComponent()->SetCrosshairState(ECrosshairState::NORIFLE);

	EBackPackLevel BackPackLevel = OwnerCharacter->GetInvenComponent()->GetCurBackPackLevel();

	AC_EquipableItem* curBackPack = OwnerCharacter->GetInvenComponent()->GetEquipmentItems()[EEquipSlot::BACKPACK];

	if (!curBackPack)
	{
		switch (CurState)
		{
		case EGunState::MAIN_GUN:
			return AttachToComponent
			(
				InParent,
				FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
				MAIN_HOLSTER_SOCKET_NAME
			);
			break;
		case EGunState::SUB_GUN:
			return AttachToComponent
			(
				InParent,
				FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
				SUB_HOLSTER_SOCKET_NAME
			);
			break;
		default:
			return false;
			break;	
		}
	}
	else
	{
		switch (CurState)
		{
		case EGunState::MAIN_GUN:
			return AttachToComponent
			(
				InParent,
				FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
				MAIN_HOLSTER_BAG_SOCKET_NAME
			);
			break;
		case EGunState::SUB_GUN:
			return AttachToComponent
			(
				InParent,
				FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
				SUB_HOLSTER_BAG_SOCKET_NAME
			);
			break;
		default:
			return false;
			break;
		}
	}

	

}

bool AC_Gun::AttachToHand(USceneComponent* InParent)
{
	UAnimMontage* DrawMontage = DrawMontages[OwnerCharacter->GetPoseState()].Montages[CurState].AnimMontage;
	//UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();

	//if (!IsValid(DrawMontage)) return false;
	// 만일 SUB_GUN의 Draw가 실행된다면 왼손으로 먼저 Attach
	//현재 왼손으로 옮겨지고 있다면 다시 어태치 하지 않고 If문 밑으로 진행
	FName CurSocketName = GetAttachParentSocketName();

	// 총기 HUD 켜주기
	int LeftAmmoCount = 0;
	AC_Item_Bullet* CurBullet = Cast<AC_Item_Bullet>( OwnerCharacter->GetInvenComponent()->FindMyItemByName(GetCurrentBulletTypeName()));
	if (IsValid(CurBullet))
	{
		LeftAmmoCount = CurBullet->GetItemCurStack();
	}

	if (AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharacter))
	{
		UC_AmmoWidget* AmmoWidget = OwnerPlayer->GetHUDWidget()->GetAmmoWidget();

		AmmoWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible, true);
		AmmoWidget->SetShootingMode(CurrentShootingMode);
		//AmmoWidget->SetLeftAmmoText(LeftAmmoCount);
		AmmoWidget->SetMagazineText(CurBulletCount);
	}


	if (OwnerCharacter->GetMesh()->GetAnimInstance()->Montage_IsPlaying(DrawMontage) && CurSocketName != SUB_DRAW_SOCKET_NAME)
	{

		//Crawl일 때는 모션이 달라 왼손에 어태치 안함
		if (CurState == EGunState::SUB_GUN && OwnerCharacter->GetPoseState() != EPoseState::CRAWL)
		{
			return AttachToComponent
			(
				InParent,
				FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
				SUB_DRAW_SOCKET_NAME
			);
		}
	}
	OwnerCharacter->SetHandState(EHandState::WEAPON_GUN);
	if (AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharacter))
	{
		OwnerPlayer->GetCrosshairWidgetComponent()->SetCrosshairState(ECrosshairState::RIFLE);
		OwnerPlayer->SetRecoilTimelineValues(GunDataRef->BulletRPM);
	}
	FString TempSocketName = EQUIPPED_SOCKET_NAME.ToString();
	UC_Util::Print(TempSocketName);
	bool SuccessToAttach;
	SuccessToAttach = AttachToComponent(
		InParent,
		FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
		EQUIPPED_SOCKET_NAME
	);
	UC_Util::Print(SuccessToAttach);
	OwnerCharacter->SetIsReloadingBullet(false);
	return SuccessToAttach;
}

bool AC_Gun::SetAimingDown()
{
	//스프린트 중이라면 Return
	if (OwnerCharacter->GetNextSpeed() > 600) return false;
	AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharacter);
	if (!IsValid(OwnerPlayer))	              return false;
	OwnerPlayer->SetToAimDownSight();
	//CharacterMesh->HideBoneByName(FName("HeadBoneName"), EPhysBodyOp::PBO_None);

	//OwnerCharacter->bUseControllerRotationYaw = true;

	//AimDown 일 때 머리숨기기
	//TODO : 내 카메라에만 안보이고 상대방 카메라에선 보이게 만들기
	OwnerCharacter->GetMesh()->HideBoneByName(FName("Head"), EPhysBodyOp::PBO_None);
	AimSightCamera->SetActive(true);
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTargetWithBlend(this, 0.2);
	bIsAimDown = true;
	if (IsValid(AttachedItem[EPartsName::SCOPE]))
		AttachedItem[EPartsName::SCOPE]->UseMrbStrategy();
	return true;
}
//견착 조준만할 때 Player AimKePress함수로 메인카메라에서 에임 카메라로 바꿔주기
bool AC_Gun::SetAimingPress()
{
	//if (OwnerCharacter->GetInvenSystem()->GetIsPanelOpend()) return false;
	//스프린트 중이라면 Return
	if (OwnerCharacter->GetNextSpeed() > 600) return false;

	AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharacter);
	if (!IsValid(OwnerPlayer))       return false;
	if (OwnerPlayer->GetIsAimDown()) return false;
	OwnerPlayer->SetToAimKeyPress();
	
	//bIsAimDown = true;
	//OwnerCharacter->bUseControllerRotationYaw = true;

	return true;
}

bool AC_Gun::BackToMainCamera()
{
	AC_Player* Player = Cast<AC_Player>(OwnerCharacter);
	if (!IsValid(Player)) return false;
	
	AimSightCamera->SetActive(false);
	if (UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetViewTarget() == this)
	{
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTargetWithBlend(OwnerCharacter, 0.2);
	}
	OwnerCharacter->GetMesh()->UnHideBoneByName(FName("Head"));
	OwnerCharacter->bUseControllerRotationYaw = false;
	bIsAimDown = false;
	Player->BackToMainCamera();
	if (IsValid(AttachedItem[EPartsName::SCOPE]))
		AttachedItem[EPartsName::SCOPE]->UseMrbStrategy();
	return true;
	
	//FString TheFloatStr = FString::SanitizeFloat(MrbPressTimeCount);
	//GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, *TheFloatStr);
	
}

void AC_Gun::HandleSpringArmRotation()
{
	//if (!bIsAimPress) return;
	FRotator ControlRotation = OwnerCharacter->GetControlRotation();
	//UC_Util::Print(ControlRotation);
	FRotator NewRotation = FRotator(ControlRotation.Pitch, ControlRotation.Yaw, 0);
	//AimSightSpringArm->SetWorldRotation(OwnerCharacter->GetControlRotation());
	if (OwnerCharacter->GetPoseState()==EPoseState::CRAWL)
	{
		NewRotation.Pitch -= 7.0f;
		NewRotation.Yaw -= 0.5f;
	}
	AimSightSpringArm->SetWorldRotation(NewRotation);
}

void AC_Gun::GetPlayerIsAimDownOrNot()
{
	AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharacter);
	if (IsValid(OwnerPlayer))
	{
		bIsPlayerAimDownPress = OwnerPlayer->GetIsAimDown();
	}
}


void AC_Gun::SetOwnerCharacter(AC_BasicCharacter * InOwnerCharacter)
{
	Super::SetOwnerCharacter(InOwnerCharacter);
	if (!InOwnerCharacter)
	{
		if (AimWidget)
		{
			AimWidget->SetVisibility(ESlateVisibility::Hidden);
			AimWidget = nullptr;
		}
		return;
	}
	SetAimSightWidget();
}

void AC_Gun::OnOwnerCharacterPoseTransitionFin()
{
	// TODO : 장전 모션 중이었다면 -> 여기서 continue
}

void AC_Gun::CheckPlayerIsRunning()
{
	//Aim Press 혹은 Aim Down 상태일 때 스프린트 실행하면 다시 main 카메라로 돌아가기
	float NextSpeed = OwnerCharacter->GetNextSpeed();
	if (AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharacter))
	{
		if (NextSpeed > 600 && OwnerPlayer->GetIsAimDown())
			BackToMainCamera();
	}


}

bool AC_Gun::Interaction(AC_BasicCharacter* Character)
{
	switch (ItemPlace)
	{
	case EItemPlace::SLOT:
		return MoveToAround(Character, GetItemCurStack());
	case EItemPlace::AROUND:
		return MoveToSlot(Character, GetItemCurStack());
	case EItemPlace::INVEN:
		break;
	default:
		break;
	}
	return false;
}

void AC_Gun::CheckBackPackLevelChange()
{
	AC_EquipableItem* curBackPack = OwnerCharacter->GetInvenComponent()->GetEquipmentItems()[EEquipSlot::BACKPACK];

	//if (curBackPack)
	//{
	if(OwnerCharacter->GetEquippedComponent()->GetCurWeapon() != this)
		AttachToHolster(OwnerCharacter->GetMesh());
	

	PrevBackPackLevel = OwnerCharacter->GetInvenComponent()->GetCurBackPackLevel();

}

void AC_Gun::DropItem(AC_BasicCharacter* Character)
{
	if (AC_Player* Player = Cast<AC_Player>(Character))
		Player->GetHUDWidget()->GetAmmoWidget()->SetVisibility(ESlateVisibility::Hidden);
	
	DetachItem();
	ItemPlace = EItemPlace::AROUND;
}

AC_AttachableItem* AC_Gun::SetAttachableItemSlot(EPartsName InPartsName, AC_AttachableItem* InAttachableItem)
{
	AC_AttachableItem* PrevSlotAttachableItem = AttachableItem[InPartsName];

	UC_AttachableItemMeshComponent* AttachableMeshComp = OwnerCharacter->GetAttachmentMeshComponent();

	if (PrevSlotAttachableItem)
	{
		//장착된 부착물을 해체하는 작업
		//교체된 부착물은 Inven에 공간이 있으면 Inven으로, 없으면 Around로 간다.
		//if (PrevSlotAttachableItem->MoveToInven(OwnerCharacter)) {}
		//else
		//{
		//	PrevSlotAttachableItem->MoveToAround(OwnerCharacter);
		//}
		
		//PrevSlotAttachableItem->SetOwnerGun(nullptr);

		//부착물의 mesh를 총에서 장착해제
		AttachableMeshComp->DetachFromGun(this->GetGunMesh(), InPartsName, PrevSlotAttachableItem->GetAttachmentName());
	}

	AttachableItem[InPartsName] = InAttachableItem;

	if (AttachableItem[InPartsName] == nullptr) return PrevSlotAttachableItem; 
	
	//새로 장착하는 부착물의 mesh를 총에 장착

	//AttachableItem[InPartsName]->SetActorHiddenInGame(false);	//모습이 안보이도록 보이도록 
	//AttachableItem[InPartsName]->SetActorEnableCollision(true);	//Overlap 불가능 하도록 Collision Off

	//AttachableItem[InPartsName]->SetCurWeaponSlot(this->GetWeaponSlot())
	AttachableItem[InPartsName]->SetOwnerGun(this);
	AttachableItem[InPartsName]->SetOwnerCharacter(OwnerCharacter);
	
	AttachableItem[InPartsName]->SetItemPlace(EItemPlace::SLOT);

	AttachableMeshComp->AttachToGun(this->GetGunMesh(), InPartsName, AttachableItem[InPartsName]->GetAttachmentName());
	//아이템의 정확한 위치 이동은 다른 곳에서 처리하기.
	//AttachableItem[InPartsName]->MoveToSlot(OwnerCharacter);

	return PrevSlotAttachableItem;
}

bool AC_Gun::GetIsPlayingMontagesOfAny()
{
	UAnimInstance* CurAnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	UAnimMontage* DrawMontage = DrawMontages[OwnerCharacter->GetPoseState()].Montages[CurState].AnimMontage;
	//UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	UAnimMontage* SheathMontage = SheathMontages[OwnerCharacter->GetPoseState()].Montages[CurState].AnimMontage;
	UAnimMontage* ReloadMontage = ReloadMontages[OwnerCharacter->GetPoseState()].Montages[CurState].AnimMontage;
	bool IsPlayingMontagesOfAny = 
		CurAnimInstance->Montage_IsPlaying(DrawMontage)   || 
		CurAnimInstance->Montage_IsPlaying(SheathMontage) ||
		CurAnimInstance->Montage_IsPlaying(ReloadMontage);
	//UC_Util::Print(IsPlayingMontagesOfAny, FColor::Magenta, 10);
	return IsPlayingMontagesOfAny;
}

bool AC_Gun::GetCanGunAction()
{
	bool CanAim = OwnerCharacter->GetCharacterMovement()->IsFalling();
	return CanAim;
}

/*void AC_Gun::ChangeCurShootingMode()
{
	// Single_Shot 이전까지 단발, 연사, 점사를 모두 포함한 ShootingMode switching 기본 implementation
	
	int CurMode = static_cast<int>(CurrentShootingMode);
	++CurMode %= 3;
	CurrentShootingMode = static_cast<EShootingMode>(CurMode);

	if (AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharacter))
	{
		// AmmoWidget 업데이트
		OwnerPlayer->GetHUDWidget()->GetAmmoWidget()->SetShootingMode(CurrentShootingMode);
	}

	//UC_Util::Print(CurMode);
}*/

bool AC_Gun::ExecuteReloadMontage()
{
	//AC_Player* CurPlayer = Cast<AC_Player>(OwnerCharacter);
	//if (CurBulletCount == MaxBulletCount) return;
	//if (!CurPlayer->GetCanMove()) return;
	//if (CurPlayer->GetMesh()->GetAnimInstance()->Montage_IsPlaying(ReloadMontages[OwnerCharacter->GetPoseState()].Montages[CurState].AnimMontage))	return;
	//SetMagazineVisibility(false);
	//OwnerCharacter->SetIsReloadingBullet(true);
	//OwnerCharacter->PlayAnimMontage(ReloadMontages[OwnerCharacter->GetPoseState()].Montages[CurState]);
	//BackToMainCamera();	
	return true;
}

bool AC_Gun::MoveAroundToInven(AC_BasicCharacter* Character, int32 InStack)
{
	return false;
}

bool AC_Gun::MoveAroundToSlot(AC_BasicCharacter* Character, int32 InStack)
{
	UC_EquippedComponent* equipComp = Character->GetEquippedComponent();//TODO : 안쓰는건 삭제하기.
	UC_InvenComponent* invenComp = Character->GetInvenComponent();		//TODO : 안쓰는건 삭제하기.

	AC_Gun* currentMainGun = Cast<AC_Gun>(equipComp->GetWeapons()[EWeaponSlot::MAIN_GUN]);
	AC_Gun* currentSubGun  = Cast<AC_Gun>(equipComp->GetWeapons()[EWeaponSlot::SUB_GUN]);

	if (!currentMainGun)
	{
		equipComp->SetSlotWeapon(EWeaponSlot::MAIN_GUN, this);

		//SetRelativeTranformToInitial();
		//AttachToHolster(OwnerCharacter->GetMesh());

		invenComp->RemoveItemToAroundList(this);
		return true;
	}

	if (!currentSubGun)
	{
		equipComp->SetSlotWeapon(EWeaponSlot::SUB_GUN, this);
		//SetRelativeTranformToInitial();
		//AttachToHolster(OwnerCharacter->GetMesh());

		invenComp->RemoveItemToAroundList(this);
		return true;
	}
	else
	{
		AC_Weapon* DroppedGun = equipComp->SetSlotWeapon(EWeaponSlot::SUB_GUN, this);
		//SetRelativeTranformToInitial();
		//AttachToHolster(OwnerCharacter->GetMesh());

		invenComp->RemoveItemToAroundList(this);
		DroppedGun->DropItem(Character);
		return true;
	}
}

bool AC_Gun::MoveSlotToInven(AC_BasicCharacter* Character, int32 InStack)
{
	return false;
}

bool AC_Gun::MoveSlotToAround(AC_BasicCharacter* Character, int32 InStack)
{
	if (Super::MoveSlotToAround(Character, InStack))
	{
		UC_EquippedComponent* equipComp = Character->GetEquippedComponent();//TODO : 안쓰는건 삭제하기.
		UC_InvenComponent* invenComp = Character->GetInvenComponent();		//TODO : 안쓰는건 삭제하기.
		//equipComp->SetSlotWeapon(GetWeaponSlot(), nullptr);
		//invenComp->AddItemToAroundList(this);
		int32 LeftBulletCount = GetCurBulletCount();
		
		if (LeftBulletCount == 0) return true;
		
		AC_Item* LeftBulletItem = 
			GAMESCENE_MANAGER->GetItemManager()->SpawnItem(GetCurrentBulletTypeName(), GetGroundLocation(Character), LeftBulletCount);
		if (!LeftBulletItem)
		{
			UC_Util::Print("LeftBulletItem is nullptr");
			return false;
		}
		
		LeftBulletItem->SetActorEnableCollision(false);
		LeftBulletItem->MoveToInven(Character, LeftBulletCount);
		LeftBulletItem->SetActorEnableCollision(true);
		
		SetCurBulletCount(0);

		return true;
	}
	return false;
}

FVector2D AC_Gun::GetRecoilFactors()
{
	float VerticalFactor   = GunDataRef->RecoilFactorVertical * RecoilMultiplierByGripVert * RecoilMultiplierMuzzleVert;
	float HorizontalFactor = GunDataRef->RecoilFactorHorizontal * RecoilMultiplierByGripHorizon * RecoilMultiplierMuzzleHorizon;

	return FVector2D(HorizontalFactor, VerticalFactor);
}

bool AC_Gun::FireBullet()
{
	if (!IsValid(OwnerCharacter)) return false;
	bool OnScreen = (OwnerCharacter->GetNextSpeed() < 600) && OwnerCharacter->GetCanMove();
	if (!OnScreen) return false;
	//ExecuteReloadMontage();


	FVector FireLocation;
	FVector FireDirection;
	FVector HitLocation;
	bool HasHit;
	if (!SetBulletDirection(FireLocation, FireDirection,HitLocation, HasHit)) return false;

	//UC_Util::Print(FireLocation);
	//UC_Util::Print(FireDirection);
	AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharacter); // TODO : OwnerPlayer -> Enemy도 총을 쏠 수 있으니 예외처리 시켜야 함
	bool ApplyGravity = OwnerPlayer->GetIsWatchingSight() || !HasHit;
	int BulletCount = 0;
	for (auto& Bullet : OwnerPlayer->GetBullets())
	{
		if (Bullet->GetIsActive())
		{
			//UC_Util::Print("Can't fire");
			continue;
		}
		BulletCount++;
		//UC_Util::Print("FIRE!!!!!!!");
		CurBulletCount--;
		OwnerPlayer->RecoilController();
		if (HasHit)
		{
			bool Succeeded = Bullet->Fire(this, FireLocation, FireDirection, ApplyGravity, HitLocation);
			//if (Cast<AC_Player>(OwnerCharacter))
			if (OwnerCharacter->IsLocallyControlled())
			{
				if (GunSoundData->FireSound) UGameplayStatics::PlaySound2D(this, GunSoundData->FireSound);
			}
			else
			{
				if (GunSoundData->FireSound) UGameplayStatics::PlaySoundAtLocation(this, GunSoundData->FireSound, GetActorLocation());
			}
			if (Succeeded) 
			{ 
				OwnerPlayer->GetHUDWidget()->GetAmmoWidget()->SetMagazineText(CurBulletCount, true); 

				if (OwnerPlayer->GetEquippedComponent()->GetCurWeaponType() == EWeaponSlot::MAIN_GUN )
					OwnerPlayer->GetInvenSystem()->GetInvenUI()->GetEquipmentPanel()->GetMainGunSlot()->UpdateCurAmmo(this);
				else if (OwnerPlayer->GetEquippedComponent()->GetCurWeaponType() == EWeaponSlot::SUB_GUN)
					OwnerPlayer->GetInvenSystem()->GetInvenUI()->GetEquipmentPanel()->GetSubGunSlot()->UpdateCurAmmo(this);

			}
			if (IsValid(MuzzleFlameEffectParticle))
			{
				//UGameplayStatics::SpawnEmitterAtLocation(this->GetWorld(), MuzzleFlameEffectParticle, OwnerCharacter->GetActorLocation(),GunMesh->GetForwardVector().Rotation(),FVector(0.2f, 0.2f, 0.2f));
				//UGameplayStatics::SpawnEmitterAtLocation(this->GetWorld(), MuzzleFlameEffectParticle, GunMesh->GetSocketLocation(FName("MuzzleSocket")) + GunMesh->GetForwardVector() *30, FRotator::MakeFromEuler(GunMesh->GetForwardVector()),FVector(0.2f, 0.2f, 0.2f));
				//UGameplayStatics::SpawnEmitterAtLocation(this->GetWorld(), MuzzleFlameEffectParticle, GunMesh->GetSocketLocation(FName("MuzzleSocket")) , FRotator(0,0,0),FVector(0.2f, 0.2f, 0.2f));
				UGameplayStatics::SpawnEmitterAttached(
															MuzzleFlameEffectParticle,  // 파티클 시스템
															GunMesh,      // 부착할 메쉬 (총)
															TEXT("MuzzleSocket") // 총구 소켓에 부착
															,FVector(0, 0, 0)
															,FRotator(0, 0, 0),
															FVector(0.2f, 0.2f, 0.2f)
														);
																	
				
			}
			return Succeeded;
		}
		else
		{
			bool Succeeded = Bullet->Fire(this, FireLocation, FireDirection, ApplyGravity);
			//if (Cast<AC_Player>(OwnerCharacter))
			if (OwnerCharacter->IsLocallyControlled())
			{
				if (GunSoundData->FireSound) UGameplayStatics::PlaySound2D(this, GunSoundData->FireSound);
			}
			else
			{
				if (GunSoundData->FireSound) UGameplayStatics::PlaySoundAtLocation(this, GunSoundData->FireSound, GetActorLocation());
			}

			if (Succeeded)
			{
				OwnerPlayer->GetHUDWidget()->GetAmmoWidget()->SetMagazineText(CurBulletCount, true);

				if (OwnerPlayer->GetEquippedComponent()->GetCurWeaponType() == EWeaponSlot::MAIN_GUN)
					OwnerPlayer->GetInvenSystem()->GetInvenUI()->GetEquipmentPanel()->GetMainGunSlot()->UpdateCurAmmo(this);
				else if (OwnerPlayer->GetEquippedComponent()->GetCurWeaponType() == EWeaponSlot::SUB_GUN)
					OwnerPlayer->GetInvenSystem()->GetInvenUI()->GetEquipmentPanel()->GetSubGunSlot()->UpdateCurAmmo(this);

			}
			if (IsValid(MuzzleFlameEffectParticle))
			{
				//UGameplayStatics::SpawnEmitterAtLocation(this->GetWorld(), MuzzleFlameEffectParticle, GunMesh->GetSocketLocation(FName("MuzzleSocket")), FRotator(0,0,0),FVector(0.2f, 0.2f, 0.2f));
				//UGameplayStatics::SpawnEmitterAtLocation(this->GetWorld(), MuzzleFlameEffectParticle, Get, GunMesh->GetForwardVector().Rotation(),FVector(0.2f, 0.2f, 0.2f));
				UGameplayStatics::SpawnEmitterAttached(
												MuzzleFlameEffectParticle,  // 파티클 시스템
												GunMesh,      // 부착할 메쉬 (총)
												TEXT("MuzzleSocket") // 총구 소켓에 부착
												,FVector(0	)
												,FRotator(0	, 0, 0),
												FVector(0.2f, 0.2f, 0.2f)
											);
				// MuzzleFlameEffectParticle->SetWorldLocation(FireLocation);
				// //MuzzleFlameEffectParticle->SetRelativeRotation(this->GetActorForwardVector().Rotation());
				// MuzzleFlameEffectParticle->SetWorldRotation(OwnerCharacter->GetActorForwardVector().Rotation());
				//
				// MuzzleFlameEffectParticle->Activate();
				
			}
			return Succeeded;
		}

		//Bullet->Fire(this, FireLocation, FireDirection);
		//if (BulletCount > 100)
		//	return true;
	}
	// UC_Util::Print("No More Bullets in Pool");

	return false;
}

bool AC_Gun::ReloadBullet()
{
	if (CurBulletCount == MaxBulletCount) return false;
	int LeftAmmoCount = 0;
	AC_Item_Bullet* CurBullet = Cast<AC_Item_Bullet>( OwnerCharacter->GetInvenComponent()->FindMyItemByName(GetCurrentBulletTypeName()));
	UC_InvenComponent* InvenComp = OwnerCharacter->GetInvenComponent();
	if (IsValid(CurBullet))
	{
		//LeftAmmoCount = CurBullet->GetItemCurStack();
		LeftAmmoCount = InvenComp->GetTotalStackByItemName(CurBullet->GetItemCode());
	}

	OwnerCharacter->SetIsReloadingBullet(false);
	int BeforeChangeAmmo = CurBulletCount;

	UC_Util::Print(BeforeChangeAmmo);

	int RemainAmmo;
	int ChangedStack;
	//AC_Item_Bullet* CarryingBullet;
	AC_SR* CurrentSR = Cast<AC_SR>(this);

	if (IsValid(CurrentSR))
		CurrentSR->SetIsReloadingSR(false);
	if (LeftAmmoCount == 0) return false;

	CurBulletCount = FMath::Min(MaxBulletCount, LeftAmmoCount);

	RemainAmmo = -BeforeChangeAmmo + CurBulletCount;

	ChangedStack = LeftAmmoCount - RemainAmmo;
	
	InvenComp->DecreaseItemStack(CurBullet->GetItemCode(), RemainAmmo);
	//CurBullet->SetItemStack(ChangedStack); //TODO : InvenComponent에서 한번에 조절하는 기능 만들기.
	// UC_Util::Print("Reload Bullet");
	//장전한 총알 갯수만큼 curVolume 조절
	OwnerCharacter->GetInvenComponent()->AddInvenCurVolume(-(RemainAmmo * CurBullet->GetItemDatas()->ItemVolume));

	if (AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharacter))
	{
		OwnerPlayer->GetInvenSystem()->InitializeList();
		//OwnerPlayer->GetHUDWidget()->GetAmmoWidget()->SetLeftAmmoText(CurBullet->GetItemCurStack(), true);
		OwnerPlayer->GetHUDWidget()->GetAmmoWidget()->SetMagazineText(CurBulletCount, true);
	}
	return true;

}

void AC_Gun::SetBulletSpeed()
{
	AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharacter);
	if (!IsValid(OwnerPlayer))
	{
		return;
	}
	for (auto& Bullet : OwnerPlayer->GetBullets())
	{
		if (IsValid(Bullet))
		{
			// UC_Util::Print("FindBullet");

			UProjectileMovementComponent* ProjectileMovement = Bullet->FindComponentByClass<UProjectileMovementComponent>();
			if (ProjectileMovement)
			{
				ProjectileMovement->InitialSpeed = GunDataRef->BulletSpeed;

			}
		}
	}
}

bool AC_Gun::SetBulletDirection(FVector &OutLocation, FVector &OutDirection, FVector &OutHitLocation, bool& OutHasHit)
{
	FHitResult HitResult;
	//AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharacter);

	// 동현 주석 : Enemy 총기 BulletDirection에 대한 처리가 필요함)
	if (AC_Enemy* OwnerEnemy = Cast<AC_Enemy>(OwnerCharacter))
		return false;

	AController* Controller = OwnerCharacter->GetController();
	APlayerCameraManager* PlayerCamera = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
	FVector StartLocation = PlayerCamera->GetCameraLocation();
	FRotator CameraRotation = PlayerCamera->GetCameraRotation();

	FVector ForwardVector = CameraRotation.Vector().GetSafeNormal() * 10000;
	FCollisionQueryParams CollisionParams{};
	CollisionParams.AddIgnoredActor(this);
	CollisionParams.AddIgnoredActor(OwnerCharacter);
	CollisionParams.AddIgnoredActor(PlayerCamera);
	for (auto AttachableMeshs : OwnerCharacter->GetAttachmentMeshComponent()->GetAttachableItemMeshs())
	{
		for (auto MeshsTuple : AttachableMeshs.Value)
		{
			for (auto Attachment : MeshsTuple.Value)
			{
				CollisionParams.AddIgnoredActor(Attachment);
			}

		}
	}
	HitResult = {};
	FVector WorldLocation, WorldDirection;
	FVector CenterLocation, CenterDirection;
	APlayerController* WolrdContorller = GetWorld()->GetFirstPlayerController();
	TArray<UUserWidget*> FoundWidgets;
	UUserWidget* MyWidget = AimWidget;
	UImage* MyImage;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), FoundWidgets, UUserWidget::StaticClass(), true);

	MyImage = Cast<UImage>(MyWidget->WidgetTree->FindWidget(FName("Crosshair_Base")));

	UCanvasPanelSlot* ImageSlot;
	FVector2D RandomPoint;
	if (IsValid(MyImage))
	{

		ImageSlot = Cast<UCanvasPanelSlot>(MyImage->Slot);
		if (ImageSlot)
		{
			// 이때, Cast<UPanelSlot>(ImageSlot)으로 다른 타입으로 캐스팅할 수 있음
			FVector2D ImageSize = ImageSlot->GetSize();
			
			RandomPoint = FMath::RandPointInCircle(ImageSize.X * 0.5f * 0.373f);
			//UC_Util::Print(ImageSize.X);
			//UC_Util::Print(ImageSize.Y);

		}
		else
		{
			UC_Util::Print("Cant Find Slot");

			return false;
		}
	}
	else
		return false;
	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);
	FVector2D RandomPointOnScreen;

	//TODO: 캐릭터 상태에 따라 탄퍼짐 or 직선
	if (OwnerCharacter->GetIsWatchingSight())
	{
		if (GetIsPartAttached(EPartsName::SCOPE))
		{
			
			if (AttachedItemName[EPartsName::SCOPE] == EAttachmentNames::REDDOT)
			{
				RandomPointOnScreen.X = (IronSightWindowLocation.X * ViewportSize.X );
				RandomPointOnScreen.Y = (0.5f * ViewportSize.Y );
				UC_Util::Print("RandomPointOnScreen");
			}
			else
			{
				RandomPointOnScreen.X = (IronSightWindowLocation.X * ViewportSize.X );
				RandomPointOnScreen.Y = (IronSightWindowLocation.Y * ViewportSize.Y );
			}
		}
		else
		{
			RandomPointOnScreen.X = (IronSightWindowLocation.X * ViewportSize.X );
			RandomPointOnScreen.Y = (IronSightWindowLocation.Y * ViewportSize.Y );
			
		}
	}
	else
	{
		RandomPointOnScreen.X = (0.5 * ViewportSize.X + RandomPoint.X);
		RandomPointOnScreen.Y = (0.5 * ViewportSize.Y + RandomPoint.Y);
	}

	WolrdContorller->DeprojectScreenPositionToWorld(RandomPointOnScreen.X, RandomPointOnScreen.Y, WorldLocation, WorldDirection);
	FVector DestLocation = WorldLocation + WorldDirection * 100000;
	if (GetIsPartAttached(EPartsName::SCOPE))
	{
		if (
			GetAttachedItemName(EPartsName::SCOPE) == EAttachmentNames::SCOPE4 ||
			GetAttachedItemName(EPartsName::SCOPE) == EAttachmentNames::SCOPE8
			)
		{
			FVector AttachmentLocation = AttachedItem[EPartsName::SCOPE]->GetActorLocation();
			FRotator AttachmentRotation = AttachedItem[EPartsName::SCOPE]->GetActorRotation();
			FVector AttachmentForward = AttachmentRotation.Vector().GetSafeNormal();
			DestLocation = AttachmentLocation + AttachmentForward * 100000;
		}
	}

	//FVector ChildForward = TestDirection.Vector();
	//DestLocation = TestLocation + ChildForward * 100000;
	bool HasHit = GetWorld()->LineTraceSingleByChannel(HitResult, WorldLocation, DestLocation, ECC_Visibility, CollisionParams);

	DrawDebugSphere(GetWorld(), HitResult.Location, 1.0f, 12, FColor::Red, true);
	//UC_Util::Print(float(HitResult.Distance));
	FVector FireLocation = GunMesh->GetSocketLocation(FName("MuzzleSocket"));
	//FVector FireLocation2 = GunMesh->GetSocketLocation(FName("MuzzleSocke"));
	//UC_Util::Print(FireLocation);
	//UC_Util::Print(FireLocation2, FColor::Blue);
	FVector FireDirection;
	WolrdContorller->DeprojectScreenPositionToWorld(0.5 * ViewportSize.X, 0.5 * ViewportSize.Y, CenterLocation,CenterDirection);
	if (HitResult.Distance <= 1000 && HitResult.Distance >0)
	{
		//FireLocation.Z += 20;
		
		FireDirection = DestLocation - WorldLocation;
		FireDirection = FireDirection.GetSafeNormal();
		
		//UC_Util::Print(HitResult.Location, FColor::Emerald);
		//UC_Util::Print(HitResult.Distance, FColor::Cyan);
	}
	else
	{
		FireDirection = DestLocation - FireLocation;
		FireDirection = FireDirection.GetSafeNormal();
		//HasHit = false;
		//UC_Util::Print(FireDirection, FColor::Blue);

	}
	//FRotator LocalRotation(0, 0, 5.6);
	float RadianValue = 0;
	if (HitResult.Distance >= 200)
		 RadianValue= FMath::DegreesToRadians(0.06f);

	// 기존 벡터의 크기를 저장
	float OriginalLength = FireDirection.Size();

	// Z 값을 변화시키기 위해 현재 XY 평면에서의 크기를 유지
	FireDirection.Z += FMath::Tan(RadianValue);

	// 벡터를 다시 정규화하여 크기를 유지
	FireDirection = FireDirection.GetSafeNormal();
	// UC_Util::Print(FireDirection, FColor::Blue);

	FireDirection *= 100;
	FireDirection *= GunDataRef->BulletSpeed;
	OutLocation = FireLocation;
	OutDirection = FireDirection;
	OutHasHit = HasHit;
	OutHitLocation = HitResult.Location;
	return true;
}

void AC_Gun::SetAimSightWidget()
{
	AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharacter);
	if (OwnerPlayer)
		AimWidget = OwnerPlayer->GetCrosshairWidgetComponent()->GetAimWidget();
}

void AC_Gun::ShowAndHideWhileAiming()
{
	if (!OwnerCharacter) return;
	if (OwnerCharacter->GetEquippedComponent()->GetCurWeapon() != this) return;
	AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharacter);

	if (!IsValid(OwnerPlayer)) return;

	if (!OwnerPlayer->GetIsWatchingSight()) return;
	FVector StartLocation = AimSightCamera->GetComponentLocation();
	FVector ForwardVector = AimSightCamera->GetForwardVector() * 15;
	FVector EndLocation = StartLocation + ForwardVector;
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(OwnerCharacter);
	//if (GetIsPartAttached(EPartsName::SCOPE))
	//	CollisionParams.AddIgnoredActor(AttachedItem[EPartsName::SCOPE]);
	CollisionParams.AddIgnoredComponent(GunMesh);
	bool HasHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, CollisionParams);
	if (HasHit )
	{
		
		AimWidget->SetVisibility(ESlateVisibility::Visible);
		//UC_Util::Print(HitResult.GetActor()->GetName());
	}
	else
	{
		AimWidget->SetVisibility(ESlateVisibility::Hidden);

	}
		//UC_Util::Print("No Hit");

}

void AC_Gun::LoadMagazine()
{
	if (GunDataRef->CurGunType == EGunType::SR)
	{
		return;
	}
	FString ClassPath = TEXT("/Game/Project_PUBG/Common/Weapon/GunWeapon/Magazine/BPC_Magazine.BPC_Magazine_C");
	//Magazine = LoadObject<AC_AttachableItem>(nullptr, ));

	// UClass로 불러오기
	UClass* MagazineClass = LoadObject<UClass>(nullptr, *ClassPath);
	if (MagazineClass)
	{
		//	// 인스턴스 생성
		FActorSpawnParameters Param{};
		Param.Owner = this;
		//ConsumableItem = GetWorld()->SpawnActor<AC_FirstAidKit>(ConsumableItemClass, Param);
		Magazine = GetWorld()->SpawnActor<AC_AttachableItem>(MagazineClass, Param);
	}
	if (IsValid(Magazine))
	{
		// 인스턴스를 GunMesh에 Attach
		Magazine->SetOwnerCharacter(OwnerCharacter);
		Magazine->SetIsAttached(true);
		//UC_Util::Print("Success To Load Magazine", FColor::Blue, 10.0f);

	}
	else
	{
		UC_Util::Print("Failed To Load Magazine", FColor::Blue, 10.0f);
		
	}

//	else
//	{
//		UC_Util::Print("Fail To Load Magazine Class", FColor::Blue, 10.0f);
//
//	}
}

void AC_Gun::SetMagazineVisibility(bool InIsVisible)
{
	if (IsValid(Magazine))
	{
		Magazine->SetMeshVisibility(InIsVisible);
		//UC_Util::Print("ASDaisujdfhaiseuehfaiesuhfaweiufh");
	}
}

bool AC_Gun::GetGunHasGrip()
{
	return 	IsValid(AttachedItem[EPartsName::GRIP]);
}

void AC_Gun::SetHolsterNames()
{
	for (int32 i = 0; i < (int32)EAttachmentNames::MAX; ++i) // EAttachmentNames에 MAX가 있다면
	{
		EAttachmentNames AttachmentName = (EAttachmentNames)i;
		AttachmentPartsHolsterNames.Add(AttachmentName);
		ScopeCameraLocations.Add(AttachmentName);
	}


	//AttachmentPartsHolsterNames.Add(EAttachmentNames::REDDOT, FName("Red_Dot_Socket"));
	//AttachmentPartsHolsterNames.Add(EAttachmentNames::SCOPE4, FName("4X_Scope_Socket"));
	//

	//ScopeCameraLocations.Add(EAttachmentNames::REDDOT, FVector4(7.f,    0.f, 15.f, 6.f   ));
	//ScopeCameraLocations.Add(EAttachmentNames::SCOPE4, FVector4(10.89f, 0.f, 14.75f, 6.5f));
	ScopeCameraLocations.Add(EAttachmentNames::MAX);


	for (int32 i = 0; i < (int32)EPartsName::MAX; ++i) // EAttachmentNames에 MAX가 있다면
	{
		EPartsName AttachmentName = (EPartsName)i;
		AttachedItem.Add(AttachmentName, nullptr);
		IsPartAttached.Add(AttachmentName, false);
		AttachedItemName.Add(AttachmentName, EAttachmentNames::MAX);
	}
}

void AC_Gun::SetIronSightMeshHiddenInGame(bool bInIsHidden)
{
	if (!IsValid(IronSightMesh)) return;
	IronSightMesh->SetHiddenInGame(bInIsHidden);
}

void AC_Gun::SetIsPartAttached(EPartsName InAttachmentName, bool bInIsAttached)
{
	IsPartAttached[InAttachmentName] = bInIsAttached;
}

void AC_Gun::SetSightCameraSpringArmLocation(FVector4 InLocationAndArmLength)
{
	FVector Location{};
	Location.X = InLocationAndArmLength.X;
	Location.Y = InLocationAndArmLength.Y;
	Location.Z = InLocationAndArmLength.Z;
	AimSightSpringArm->SetRelativeLocation(Location);
	AimSightSpringArm->TargetArmLength = InLocationAndArmLength.W;
}

void AC_Gun::SetScopeCameraMode(EAttachmentNames InAttachmentName)
{
	switch (InAttachmentName)
	{
	case EAttachmentNames::MAX:
	case EAttachmentNames::REDDOT:
		AimSightCamera->PostProcessSettings.DepthOfFieldFocalDistance = 0.0f;
		AimSightCamera->PostProcessSettings.DepthOfFieldSensorWidth   = 0.0f;
		AimSightCamera->PostProcessSettings.DepthOfFieldFstop         = 0.0f;
		break;
	case EAttachmentNames::SCOPE4:
		AimSightCamera->PostProcessSettings.DepthOfFieldFocalDistance = 6.5f;
		AimSightCamera->PostProcessSettings.DepthOfFieldSensorWidth   = 5.0f;
		AimSightCamera->PostProcessSettings.DepthOfFieldFstop         = 4.0f;
		break;
	case EAttachmentNames::SCOPE8:
		AimSightCamera->PostProcessSettings.DepthOfFieldFocalDistance = 6.5f;
		AimSightCamera->PostProcessSettings.DepthOfFieldSensorWidth   = 5.0f;
		AimSightCamera->PostProcessSettings.DepthOfFieldFstop         = 4.0f;
		break;
	default:
		AimSightCamera->PostProcessSettings.DepthOfFieldFocalDistance = 0.f;
		AimSightCamera->PostProcessSettings.DepthOfFieldSensorWidth   = 0.f;
		AimSightCamera->PostProcessSettings.DepthOfFieldFstop         = 0.f;
		break;
	}


}

bool AC_Gun::ExecuteAIAttack(AC_BasicCharacter* InTargetCharacter)
{
	if (!CanAIAttack(InTargetCharacter))
	{
		return false;
	}
	
	//ExecuteReloadMontage();
	int BackpackBulletStack = 0;
	if (IsValid(OwnerCharacter->GetInvenComponent()->FindMyItemByName(GetCurrentBulletTypeName())))
		BackpackBulletStack = OwnerCharacter->GetInvenComponent()->FindMyItemByName(GetCurrentBulletTypeName())->GetItemCurStack();
	if (CurBulletCount == 0 &&  BackpackBulletStack== 0)
	{
		UC_Util::Print("Back To Wait Condition");
		return false;
	}
	UC_Util::Print(CurBulletCount);
	UC_Util::Print(BackpackBulletStack);

	//if (!SetBulletDirection(FireLocation, FireDirection, HitLocation, HasHit)) return false;

	//UC_Util::Print(FireLocation);
	//UC_Util::Print(FireDirection);

	return true;

}

bool AC_Gun::ExecuteAIAttackTickTask(AC_BasicCharacter* InTargetCharacter, const float& DeltaTime)
{
	return false;
	//return Super::ExecuteAIAttackTickTask(InTargetCharacter, DeltaTime);
	
}

bool AC_Gun::CanAIAttack(AC_BasicCharacter* InTargetCharacter)
{
	if (!IsValid(OwnerCharacter))
	{
		UC_Util::Print("From AC_Gun::ExecuteAIAttack : Invalid OwnerCharacter", FColor::MakeRandomColor(), 10.f);
		return false;
	}
	if (!IsValid(InTargetCharacter))
	{
		UC_Util::Print("From AC_Gun::ExecuteAIAttack : Invalid InTargetCharacter", FColor::MakeRandomColor(), 10.f);
		return false;
	}
	if (InTargetCharacter->GetMainState() == EMainState::DEAD)
	{
		UC_Util::Print("From AC_Gun::ExecuteAIAttack : Invalid InTargetCharacter", FColor::MakeRandomColor(), 10.f);

		return false;
	}
	bool OnScreen = (OwnerCharacter->GetNextSpeed() < 600) && OwnerCharacter->GetCanMove();
	if (!OnScreen)
	{
		UC_Util::Print("From AC_Gun::ExecuteAIAttack : OnScreen Failed", FColor::MakeRandomColor(), 10.f);
		return false;
	}
	AC_Enemy* OwnerEnemy = Cast<AC_Enemy>(OwnerCharacter); 
	if (!IsValid(OwnerEnemy))
	{
		UC_Util::Print("From AC_Gun::ExecuteAIAttack : Invalid OwnerEnemy", FColor::MakeRandomColor(), 10.f);
		return false;
	}
	if (!OwnerEnemy->GetEnemyAIController()->IsCurrentlyOnSight(InTargetCharacter))
		return false;

	
	return true;
}

bool AC_Gun::AIFireBullet(AC_BasicCharacter* InTargetCharacter)
{
	return false;
}

void AC_Gun::CancelReload()
{
	if (!IsValid(OwnerCharacter)) return;

	OwnerCharacter->StopReloadBulletSound();
	OwnerCharacter->StopReloadMagazineSound();
}

void AC_Gun::SetActorHiddenInGame(bool bNewHidden)
{
	Super::SetActorHiddenInGame(bNewHidden);
	// 각 Attachment에 대해서도 한 번에 visibility 처리
	for (int i = 0; i < static_cast<int>(EPartsName::MAX); ++i)
	{
		EPartsName PartsName = static_cast<EPartsName>(i);

		if (!IsPartAttached[PartsName]) continue;

		AttachedItem[PartsName]->SetActorHiddenInGame(bNewHidden);
	}

	SetMagazineVisibility(!bNewHidden);
}

FName AC_Gun::GetCurrentBulletTypeName()
{
	switch (GunDataRef->CurGunBulletType) 
	{
	case EBulletType::FIVEMM:
		return "Item_Ammo_556mm_C";
		break;
	case EBulletType::SEVENMM:
		return "Item_Ammo_762mm_C";
		break;
	case EBulletType::NONE:
		return "NONE";
		break;

	}
	return "";
}



