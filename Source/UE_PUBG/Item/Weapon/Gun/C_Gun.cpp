

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
#include "Character/C_PreviewCharacter.h"
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
#include "GameFramework/SpringArmComponent.h"

#include "HUD/C_HUDWidget.h"
#include "HUD/C_AmmoWidget.h"

#include "Singleton/C_GameSceneManager.h"
#include "Singleton/C_GameInstance.h"

#include "InvenUI/Panel/ItemPanel/EquipmentPanel/C_EquipmentPanelWidget.h"
#include "InvenUI/BasicItemSlot/WeaponSlot/GunSlot/C_GunSlotWidget.h"
#include "InvenUI/BasicItemSlot/WeaponSlot/GunSlot/C_MainGunSlotWidget.h"
#include "InvenUI/BasicItemSlot/WeaponSlot/GunSlot/C_SubGunSlotWidget.h"


#include "Item/ItemManager/C_ItemManager.h"

FTutorialStageGoalCheckerDelegate AC_Gun::WeaponTutorialDelegate{};

/* 무기집 Socket 이름들 */
const FName AC_Gun::SUB_HOLSTER_SOCKET_NAME		 = "SubGunSocket_NoBag";	
const FName AC_Gun::MAIN_HOLSTER_SOCKET_NAME	 = "MainGunSocket_NoBag";	

const FName AC_Gun::SUB_HOLSTER_BAG_SOCKET_NAME  = "SubGunSocket_Bag";  	
const FName AC_Gun::MAIN_HOLSTER_BAG_SOCKET_NAME = "MainGunSocket_Bag"; 	
const FName AC_Gun::MAGAZINE_SOCKET_NAME		 = "Magazine_Socket";

const FName AC_Gun::SUB_DRAW_SOCKET_NAME		 = "DrawRifleSocket"; // 무기가 손에 부착될 socket 이름

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

	CurDrawMontage = DrawMontages[OwnerCharacter->GetPoseState()].Montages[CurGunSlotState];
	CurSheathMontage = SheathMontages[OwnerCharacter->GetPoseState()].Montages[CurGunSlotState];
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
	CheckPlayerIsRunning();
	//CheckBackPackLevelChange();
	HandleAimWidgetShowAndHideWhileAiming();
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

	// Player이라면, CrossHair 모양 수정
	if (AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharacter))
		OwnerPlayer->GetCrosshairWidgetComponent()->SetCrosshairState(ECrosshairState::NORIFLE);

	const bool bHasBackpack = OwnerCharacter->GetInvenComponent()->GetEquipmentItems()[EEquipSlot::BACKPACK] != nullptr;

	// 배낭을 메고 있는지 여부 & MainGun인지 SubGun인지에 따라 AttackSocket 이름 지정
	const FName AttachSocketName = (CurGunSlotState == EGunState::MAIN_GUN) ?
		(bHasBackpack ? MAIN_HOLSTER_BAG_SOCKET_NAME : MAIN_HOLSTER_SOCKET_NAME) :
		(bHasBackpack ? SUB_HOLSTER_BAG_SOCKET_NAME  : SUB_HOLSTER_SOCKET_NAME);

	const bool IsAttached = AttachToComponent
		(
			InParent,
			FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
			AttachSocketName
		);

	UpdatePreviewWeaponMesh(this->GetWeaponSlot(), AttachSocketName);
	return IsAttached;
}

bool AC_Gun::AttachToHand(USceneComponent* InParent)
{
	AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharacter);

	if (OwnerPlayer)
	{
		// 총기 HUD 켜주기
		UC_AmmoWidget* AmmoWidget = OwnerPlayer->GetHUDWidget()->GetAmmoWidget();

		/*AC_Item_Bullet* CurBullet = Cast<AC_Item_Bullet>(OwnerCharacter->GetInvenComponent()->FindMyItemByName(GetCurrentBulletTypeName()));
		int LeftAmmoCount = IsValid(CurBullet) ? CurBullet->GetItemCurStack() : 0;*/

		AmmoWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible, true);
		AmmoWidget->SetShootingMode(CurrentShootingMode);
		//AmmoWidget->SetLeftAmmoText(LeftAmmoCount);
		AmmoWidget->SetMagazineText(CurMagazineBulletCount);

		// CrossHair 모양 수정
		OwnerPlayer->GetCrosshairWidgetComponent()->SetCrosshairState(ECrosshairState::RIFLE);
		OwnerPlayer->SetRecoilTimelineValues(GunDataRef->BulletRPM);
	}

	OwnerCharacter->SetHandState(EHandState::WEAPON_GUN);
	
	bool SuccessToAttach = AttachToComponent
	(
		InParent,
		FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
		EquippedSocketName
	);

	if (SuccessToAttach)
	{
		FName CurSocketName = (Cast<AC_SR>(this)) ? "Pre_Sniper_Equip" : EquippedSocketName;
		UpdatePreviewWeaponMesh(this->GetWeaponSlot(), CurSocketName);
		//UpdatePreviewWeaponMesh(this->GetWeaponSlot(), EQUIPPED_SOCKET_NAME);
	}

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
	UC_Util::Print("BackToMainCamera", FColor::MakeRandomColor(), 10.f);
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

void AC_Gun::UpdatePreviewWeaponMesh(EWeaponSlot InSlot, FName InSocket)
{
	AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharacter);

	if (!OwnerPlayer) return;

	if (IsValid(OwnerPlayer)) OwnerPlayer->GetPreviewCharacter()->AttachGunMesh(InSlot, InSocket);
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

FPriorityAnimMontage AC_Gun::GetReloadMontage(EPoseState InPoseState, EGunState InGunState) const
{
	if (InPoseState == EPoseState::POSE_MAX) return FPriorityAnimMontage();
	return ReloadMontages[InPoseState].Montages[InGunState];
}

bool AC_Gun::GetIsPlayingMontagesOfAny()
{
	UAnimInstance* CurAnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	UAnimMontage* DrawMontage = DrawMontages[OwnerCharacter->GetPoseState()].Montages[CurGunSlotState].AnimMontage;
	//UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	UAnimMontage* SheathMontage = SheathMontages[OwnerCharacter->GetPoseState()].Montages[CurGunSlotState].AnimMontage;
	UAnimMontage* ReloadMontage = ReloadMontages[OwnerCharacter->GetPoseState()].Montages[CurGunSlotState].AnimMontage;
	
	bool IsPlayingMontagesOfAny = 
		CurAnimInstance->Montage_IsPlaying(DrawMontage)   || 
		CurAnimInstance->Montage_IsPlaying(SheathMontage) ||
		CurAnimInstance->Montage_IsPlaying(ReloadMontage);
	
	return IsPlayingMontagesOfAny;
}

bool AC_Gun::ExecuteMagazineReloadMontage()
{
	if (!IsValid(OwnerCharacter)) return false;
	
	int InvenLeftAmmoCount = 0;
	AC_Item_Bullet* CurBullet = Cast<AC_Item_Bullet>(OwnerCharacter->GetInvenComponent()->FindMyItemByName(GetCurrentBulletTypeName()));
	if (IsValid(CurBullet)) InvenLeftAmmoCount = CurBullet->GetItemCurStack();

	// Inven에 남은 총알이 없을 때
	// Player의 경우만 확인, Enemy의 경우 Inven의 총알로 재장전하지 않고 임의의 새로운 총알을 부여받아 장전 처리
	if (Cast<AC_Player>(OwnerCharacter)) if (InvenLeftAmmoCount == 0) return false;

	// 탄창 최대 장탄 수를 모두 채우고 있을 때
	if (CurMagazineBulletCount == MaxMagazineBulletCount) return false;

	// 이미 장전 모션이 실행 중일 때(이미 장전이 실행 중인 상태일 때)
	if (OwnerCharacter->GetMesh()->GetAnimInstance()->Montage_IsPlaying(ReloadMontages[OwnerCharacter->GetPoseState()].Montages[CurGunSlotState].AnimMontage))
		return false;
	
	if (!OwnerCharacter->GetCanMove()) return false;
	
	SetMagazineVisibility(false);
	OwnerCharacter->SetIsReloadingBullet(true);
	OwnerCharacter->PlayAnimMontage(ReloadMontages[OwnerCharacter->GetPoseState()].Montages[CurGunSlotState]); // 장전 모션 실행
	
	BackToMainCamera();
	
	return true;
}

bool AC_Gun::MoveAroundToInven(AC_BasicCharacter* Character, int32 InStack)
{
	return false;
}

bool AC_Gun::MoveAroundToSlot(AC_BasicCharacter* Character, int32 InStack)
{
	UC_EquippedComponent* EquipComp = Character->GetEquippedComponent();
	UC_InvenComponent* InvenComp = Character->GetInvenComponent();		

	AC_Gun* CurrentMainGun = Cast<AC_Gun>(EquipComp->GetWeapons()[EWeaponSlot::MAIN_GUN]);
	AC_Gun* CurrentSubGun  = Cast<AC_Gun>(EquipComp->GetWeapons()[EWeaponSlot::SUB_GUN]);

	// WeaponTutorial 무기 파밍 Delegate notify 처리 (파밍 주체가 Player인지 체크도 추가)
	if (WeaponTutorialDelegate.IsBound() && Cast<AC_Player>(Character))
		WeaponTutorialDelegate.Execute(0, 0);	

	if (!CurrentMainGun)
	{
		EquipComp->SetSlotWeapon(EWeaponSlot::MAIN_GUN, this);

		//SetRelativeTranformToInitial();
		//AttachToHolster(OwnerCharacter->GetMesh());

		InvenComp->RemoveItemToAroundList(this);
		if (AC_Player* Player = Cast<AC_Player>(Character)) // Gun장착시 AroundItemList에서 제거
		{
			//Player->GetInvenSystem()->GetInvenUI()->RemoveItemInList(this);
		}
		return true;
	}

	if (!CurrentSubGun)
	{
		EquipComp->SetSlotWeapon(EWeaponSlot::SUB_GUN, this);
		//SetRelativeTranformToInitial();
		//AttachToHolster(OwnerCharacter->GetMesh());

		InvenComp->RemoveItemToAroundList(this);
		if (AC_Player* Player = Cast<AC_Player>(Character)) // Gun장착시 AroundItemList에서 제거
		{
			//Player->GetInvenSystem()->GetInvenUI()->RemoveItemInList(this);
		}
		return true;
	}
	else
	{
		AC_Weapon* DroppedGun = EquipComp->SetSlotWeapon(EWeaponSlot::SUB_GUN, this);
		//SetRelativeTranformToInitial();
		//AttachToHolster(OwnerCharacter->GetMesh());
		 
		InvenComp->RemoveItemToAroundList(this);
		if (AC_Player* Player = Cast<AC_Player>(Character)) // Gun장착시 AroundItemList에서 제거
		{
			//Player->GetInvenSystem()->GetInvenUI()->RemoveItemInList(this);
		}
		
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
		int32 LeftBulletCount = GetCurMagazineBulletCount();
		
		if (LeftBulletCount == 0) return true;
		
		AC_Item* LeftBulletItem = GAMESCENE_MANAGER->GetItemManager()->SpawnItem(GetCurrentBulletTypeName(), GetGroundLocation(Character), LeftBulletCount);
		if (!LeftBulletItem)
		{
			UC_Util::Print("LeftBulletItem is nullptr");
			return false;
		}
		
		LeftBulletItem->SetActorEnableCollision(false);
		LeftBulletItem->MoveToInven(Character, LeftBulletCount);
		LeftBulletItem->SetActorEnableCollision(true);
		
		SetCurMagazineBulletCount(0);

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


	FVector FireLocation{}, FireVelocity{}, HitLocation{};
	
	bool HasHit;
	if (!SetBulletVelocity(FireLocation, FireVelocity, HitLocation, HasHit)) return false;

	//UC_Util::Print(FireLocation);
	//UC_Util::Print(FireDirection);
	
	AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharacter); 
	// const bool ApplyGravity = OwnerPlayer->GetIsWatchingSight() || !HasHit;
	const bool ApplyGravity = !HasHit;
	
	int BulletCount = 0;

	for (auto& Bullet : OwnerPlayer->GetBullets())
	{
		if (Bullet->GetIsActive()) continue; 			

		BulletCount++;
		//UC_Util::Print("FIRE!!!!!!!");
		CurMagazineBulletCount--;
		OwnerPlayer->RecoilController();
		if (HasHit)
		{
			bool FireSucceeded = Bullet->Fire(this, FireLocation, FireVelocity, ApplyGravity, HitLocation);
			//if (Cast<AC_Player>(OwnerCharacter))
			if (OwnerCharacter->IsLocallyControlled())
			{
				if (GunSoundData->FireSound) UGameplayStatics::PlaySound2D(this, GunSoundData->FireSound);
			}
			else
			{
				if (GunSoundData->FireSound) UGameplayStatics::PlaySoundAtLocation(this, GunSoundData->FireSound, GetActorLocation());
			}
			
			if (FireSucceeded) 
			{ 
				OwnerPlayer->GetHUDWidget()->GetAmmoWidget()->SetMagazineText(CurMagazineBulletCount, true); 

				if (OwnerPlayer->GetEquippedComponent()->GetCurWeaponType() == EWeaponSlot::MAIN_GUN )
					OwnerPlayer->GetInvenSystem()->GetInvenUI()->GetEquipmentPanel()->GetMainGunSlot()->UpdateCurAmmo(this);
				else if (OwnerPlayer->GetEquippedComponent()->GetCurWeaponType() == EWeaponSlot::SUB_GUN)
					OwnerPlayer->GetInvenSystem()->GetInvenUI()->GetEquipmentPanel()->GetSubGunSlot()->UpdateCurAmmo(this);
			}
			
			if (IsValid(MuzzleFlameEffectParticle))
			{
				UGameplayStatics::SpawnEmitterAttached
				(
					MuzzleFlameEffectParticle,  // 파티클 시스템
					GunMesh,					// 부착할 메쉬 (총)
					TEXT("MuzzleSocket"),		// 총구 소켓에 부착
					FVector(0.f),
					FRotator(0.f),
					FVector(0.2f)
				);
			}
			return FireSucceeded;
		}

		// Not has hit during Setting Bullet velocity
		
		const bool FireSucceeded = Bullet->Fire(this, FireLocation, FireVelocity, ApplyGravity);
		//if (Cast<AC_Player>(OwnerCharacter))
		if (OwnerCharacter->IsLocallyControlled())
		{
			if (GunSoundData->FireSound) UGameplayStatics::PlaySound2D(this, GunSoundData->FireSound);
		}
		else
		{
			if (GunSoundData->FireSound) UGameplayStatics::PlaySoundAtLocation(this, GunSoundData->FireSound, GetActorLocation());
		}

		if (FireSucceeded)
		{
			OwnerPlayer->GetHUDWidget()->GetAmmoWidget()->SetMagazineText(CurMagazineBulletCount, true);

			if (OwnerPlayer->GetEquippedComponent()->GetCurWeaponType() == EWeaponSlot::MAIN_GUN)
				OwnerPlayer->GetInvenSystem()->GetInvenUI()->GetEquipmentPanel()->GetMainGunSlot()->UpdateCurAmmo(this);
			else if (OwnerPlayer->GetEquippedComponent()->GetCurWeaponType() == EWeaponSlot::SUB_GUN)
				OwnerPlayer->GetInvenSystem()->GetInvenUI()->GetEquipmentPanel()->GetSubGunSlot()->UpdateCurAmmo(this);

		}
		if (IsValid(MuzzleFlameEffectParticle))
		{
			UGameplayStatics::SpawnEmitterAttached
			(
				MuzzleFlameEffectParticle,  // 파티클 시스템
				GunMesh,					// 부착할 메쉬 (총)
				TEXT("MuzzleSocket"),		// 총구 소켓에 부착
				FVector(0.f),
				FRotator(0.f),
				FVector(0.2f)
			);
			// MuzzleFlameEffectParticle->SetWorldLocation(FireLocation);
			// //MuzzleFlameEffectParticle->SetRelativeRotation(this->GetActorForwardVector().Rotation());
			// MuzzleFlameEffectParticle->SetWorldRotation(OwnerCharacter->GetActorForwardVector().Rotation());
			//
			// MuzzleFlameEffectParticle->Activate();
			
		}
		return FireSucceeded;

		//Bullet->Fire(this, FireLocation, FireDirection);
		//if (BulletCount > 100)
		//	return true;
	}
	// UC_Util::Print("No More Bullets in Pool");

	return false;
}

bool AC_Gun::ReloadMagazine()
{
	OwnerCharacter->SetIsReloadingBullet(false);
	
	AC_SR* CurrentSR = Cast<AC_SR>(this);
	if (IsValid(CurrentSR)) CurrentSR->SetIsCurrentlyReloadingSRMagazine(false);

	// 이미 최대 장탄수로 Magazine 총알이 들어있을 때
	if (CurMagazineBulletCount == MaxMagazineBulletCount) return false;

	// OwnerCharacter가 Enemy일 경우, Inven의 총알 수 조정 없이 무조건 최대 장탄 수로 Magazine 장전 처리
	if (Cast<AC_Enemy>(OwnerCharacter))
	{
		CurMagazineBulletCount = MaxMagazineBulletCount;
		return true;
	}

	// OwnerCharacter가 Player일 경우, Inven의 남은 총알 수 및 현재 Magazine에 남아있는 탄알 수에 따른 장전 처리를 요구함
	// 이 밑으로 OwnerPlayer에 대한 장전 처리

	AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharacter);
	if (!OwnerPlayer)
	{
		UC_Util::Print("From AC_Gun::ReloadMagazine : OwnerCharacter has to be Player but casting failed!", FColor::Red, 10.f);
		return false;
	}

	// Inven에 있는 남아있는 총알 개수 파악
	int LeftAmmoCount = 0;
	AC_Item_Bullet* CorrespondingBulletItemObject = Cast<AC_Item_Bullet>(OwnerCharacter->GetInvenComponent()->FindMyItemByName(GetCurrentBulletTypeName()));
	UC_InvenComponent* InvenComponent = OwnerCharacter->GetInvenComponent();
	if (IsValid(CorrespondingBulletItemObject)) LeftAmmoCount = InvenComponent->GetTotalStackByItemName(CorrespondingBulletItemObject->GetItemCode());

	// 장전할 수 있는 탄알 수가 Inven에 존재하지 않을 때
	if (LeftAmmoCount == 0) return false;

	// 예외처리 끝 / 장전 하기

	const int BeforeChangeAmmo = CurMagazineBulletCount;

	// 장전 처리한 Magazine 총알 수로 현재 MagazineBulletCount 조정
	CurMagazineBulletCount = FMath::Min(MaxMagazineBulletCount, LeftAmmoCount);

	// 실질적으로 장전되어 들어간 탄알 수
	// 해당 갯수를 파악해서 Inven의 탄알 수에서 제거 & Volume도 조정
	const int ReloadedTotalBulletCount = CurMagazineBulletCount - BeforeChangeAmmo;
	
	//장전한 총알 갯수만큼 Inven의 총알 & curVolume 조절
	InvenComponent->DecreaseItemStack(CorrespondingBulletItemObject->GetItemCode(), ReloadedTotalBulletCount);
	OwnerCharacter->GetInvenComponent()->AddInvenCurVolume(-(ReloadedTotalBulletCount * CorrespondingBulletItemObject->GetItemDatas()->ItemVolume));

	/* OwnerCharacter가 Player 일 때의 추가 처리 */
	OwnerPlayer->GetInvenSystem()->InitializeList();
	//OwnerPlayer->GetHUDWidget()->GetAmmoWidget()->SetLeftAmmoText(CurBullet->GetItemCurStack(), true);
	OwnerPlayer->GetHUDWidget()->GetAmmoWidget()->SetMagazineText(CurMagazineBulletCount, true);
	
	// WeaponTutorial 장전 Delegate
	if (WeaponTutorialDelegate.IsBound()) WeaponTutorialDelegate.Execute(1, -1);
	
	return true;
}

void AC_Gun::SetBulletSpeed()
{
	AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharacter);
	if (!IsValid(OwnerPlayer)) return;
		
	for (auto& Bullet : OwnerPlayer->GetBullets())
	{
		if (!IsValid(Bullet)) continue;

		UProjectileMovementComponent* ProjectileMovement = Bullet->FindComponentByClass<UProjectileMovementComponent>();
		if (ProjectileMovement)
		{
			ProjectileMovement->InitialSpeed = GunDataRef->BulletSpeed;
			break;
		}
	}
}

bool AC_Gun::SetBulletVelocity(FVector &OutLocation, FVector &OutVelocity, FVector &OutHitLocation, bool& OutHasHit)
{
	AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharacter);
	if (!OwnerPlayer) return false;

	APlayerCameraManager* PlayerCamera = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;

	FCollisionQueryParams CollisionParams{};
	CollisionParams.AddIgnoredActor(this);
	CollisionParams.AddIgnoredActor(OwnerCharacter);
	CollisionParams.AddIgnoredActor(PlayerCamera);
	
	for (auto AttachableMeshes : OwnerCharacter->GetAttachmentMeshComponent()->GetAttachableItemMeshs())
	{
		for (auto MeshesTuple : AttachableMeshes.Value)
		{
			for (auto Attachment : MeshesTuple.Value)
			{
				CollisionParams.AddIgnoredActor(Attachment);
			}

		}
	}
	
	FHitResult HitResult = {};
	FVector CameraLocation, RandomWorldDirection;
	FVector CenterLocation, CenterDirection;
	APlayerController* WorldController = GetWorld()->GetFirstPlayerController();
	TArray<UUserWidget*> FoundWidgets;

	UImage* CrosshairImage = OwnerPlayer->GetCrosshairWidgetComponent()->GetBaseCrosshairImage();
	if (!IsValid(CrosshairImage)) return false;

	UCanvasPanelSlot* ImageSlot = Cast<UCanvasPanelSlot>(CrosshairImage->Slot);
	if (!ImageSlot) return false;
	
	const FVector2D ImageSize = ImageSlot->GetSize();
	const FVector2D RandomPoint = FMath::RandPointInCircle(ImageSize.X * 0.5f * 0.373f);
		
	FVector2D ViewportSize{};
	GEngine->GameViewport->GetViewportSize(ViewportSize);
	
	FVector2D RandomPointOnScreen{};

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
	
	WorldController->DeprojectScreenPositionToWorld(RandomPointOnScreen.X, RandomPointOnScreen.Y, CameraLocation, RandomWorldDirection);
	FVector DestLocation = CameraLocation + RandomWorldDirection * 100000; // Camera Location으로부터 1km 떨어진 Random location
	
	if (GetIsPartAttached(EPartsName::SCOPE))
	{
		if (
			GetAttachedItemName(EPartsName::SCOPE) == EAttachmentNames::SCOPE4 ||
			GetAttachedItemName(EPartsName::SCOPE) == EAttachmentNames::SCOPE8
			)
		{
			const FVector  AttachmentLocation = AttachedItem[EPartsName::SCOPE]->GetActorLocation();
			const FRotator AttachmentRotation = AttachedItem[EPartsName::SCOPE]->GetActorRotation();
			const FVector  AttachmentForward  = AttachmentRotation.Vector().GetSafeNormal();
			
			DestLocation = AttachmentLocation + AttachmentForward * 100000;
		}
	}

	bool HasHit = GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, DestLocation, ECC_Visibility, CollisionParams);

	FVector FireLocation = GunMesh->GetSocketLocation(FName("MuzzleSocket"));
	FVector FireDirection{};
	
	WorldController->DeprojectScreenPositionToWorld(0.5 * ViewportSize.X, 0.5 * ViewportSize.Y, CenterLocation, CenterDirection);
	
	/*if (HitResult.Distance <= 1000 && HitResult.Distance >0)
		FireDirection = DestLocation - CameraLocation;
	else FireDirection = DestLocation - FireLocation;*/

	FireDirection = HasHit ? HitResult.Location - FireLocation : DestLocation - FireLocation;
	FireDirection = FireDirection.GetSafeNormal();
	
	//FRotator LocalRotation(0, 0, 5.6);
	float RadianValue{};
	if (HitResult.Distance >= 200) RadianValue= FMath::DegreesToRadians(0.06f);

	// 기존 벡터의 크기를 저장
	float OriginalLength = FireDirection.Size();

	// Z 값을 변화시키기 위해 현재 XY 평면에서의 크기를 유지
	FireDirection.Z += FMath::Tan(RadianValue);

	// 벡터를 다시 정규화하여 크기를 유지
	FireDirection = FireDirection.GetSafeNormal();
	// UC_Util::Print(FireDirection, FColor::Blue);

	// DrawDebugSphere(GetWorld(), HitResult.Location, 1.0f, 12, FColor::Red, true);
	// DrawDebugLine(GetWorld(), FireLocation, FireLocation + FireDirection * 1000.f, FColor::Red, true);
	
	OutLocation		= FireLocation;
	OutVelocity		= FireDirection * GunDataRef->BulletSpeed * 100.f;
	OutHasHit		= HasHit;
	OutHitLocation	= HitResult.Location;
	return true;
}

void AC_Gun::SetAimSightWidget()
{
	AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharacter);
	if (OwnerPlayer)
		AimWidget = OwnerPlayer->GetCrosshairWidgetComponent()->GetAimWidget();
}

void AC_Gun::HandleAimWidgetShowAndHideWhileAiming()
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

	AimWidget->SetVisibility(HasHit ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void AC_Gun::LoadMagazine()
{
	if (GunDataRef->CurGunType == EGunType::SR) return;
	// 디렉토리 쿠킹 시작
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
	if (IsValid(Magazine)) Magazine->SetMeshVisibility(InIsVisible);
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

void AC_Gun::SetSightCameraSpringArmLocation(const FVector4& InLocationAndArmLength)
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
	// 기존에는 장전을 Inven에 있는 총알을 사용해서 장전함
	// 수정 후 -> 장전용 총알은 무제한으로 두고 Inven의 총알은 안건드리는 식으로 수정
	return CanAIAttack(InTargetCharacter);
}

bool AC_Gun::ExecuteAIAttackTickTask(AC_BasicCharacter* InTargetCharacter, const float& DeltaTime)
{
	if (!CanAIAttack(InTargetCharacter)) return false;

	// 탄창에 남은 총알이 없을 때
	if (CurMagazineBulletCount == 0)
	{
		ExecuteMagazineReloadMontage(); // Template primitive method (탄창 재장전)
		return false; // 장전 후 Attack Tick Task 우선은 종료
	}
	
	const FVector  EnemyLocation = InTargetCharacter->GetActorLocation();
	const FVector  Direction     = (EnemyLocation - GetActorLocation()).GetSafeNormal();
	const FRotator LookRotation  = Direction.Rotation();

	// Enemy TargetCharacter 쪽으로 서서히 Lerp를 통해 OwnerCharacter 몸체 회전 시키기
	static const float InterpSpeed     = 10.0f;
	const FRotator	   CurrentRotation = OwnerCharacter->GetActorRotation();
	
	FRotator NewRotation	 = FMath::RInterpTo(CurrentRotation, LookRotation, DeltaTime, InterpSpeed);
	NewRotation.Pitch		 = 0.f;
	NewRotation.Roll		 = 0.f;
	
	OwnerCharacter->SetActorRotation(NewRotation);
	
	AIFireTimer += DeltaTime;

	// 아직 발사 Delay 시간까지 남아있거나 몸체가 TargetCharacter를 향해 적정량 돌지 않았다면 continue
	if (AIFireTimer < AIAttackIntervalTime || abs(NewRotation.Yaw - LookRotation.Yaw) > 10.f) return true;

	AIFireTimer -= AIAttackIntervalTime;
	
	// 총알이 제대로 발사되었다면 TickTask 지속 | 아니라면 AIAttackTickTask 종료
	return AIFireBullet(InTargetCharacter); // Template primitive method
}

bool AC_Gun::CanAIAttack(AC_BasicCharacter* InTargetCharacter)
{
	if (!IsValid(InTargetCharacter)) return false;
	if (InTargetCharacter->GetMainState() == EMainState::DEAD) return false;
	
	bool OnScreen = (OwnerCharacter->GetNextSpeed() < 600) && OwnerCharacter->GetCanMove();
	if (!OnScreen) return false;
		
	AC_Enemy* OwnerEnemy = Cast<AC_Enemy>(OwnerCharacter);
	if (!OwnerEnemy->GetController<AC_EnemyAIController>()->IsCurrentlyOnSight(InTargetCharacter))
		return false;

	AC_EnemyAIController* EnemyAIController = Cast<AC_EnemyAIController>(OwnerCharacter->GetController());
	if (!EnemyAIController->IsCurrentlyOnSight(InTargetCharacter)) return false;
	
	return true;
}

void AC_Gun::CancelReload()
{
	if (!IsValid(OwnerCharacter)) return;

	OwnerCharacter->StopReloadBulletSound();
	OwnerCharacter->StopReloadMagazineSound();
}

void AC_Gun::OnReloadEnd()
{
	this->ReloadMagazine();
	this->SetMagazineVisibility(true);
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



