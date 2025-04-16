  // Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/ThrowingWeapon/C_ThrowingWeapon.h"

#include "Item/Weapon/WeaponStrategy/I_WeaponButtonStrategy.h"
#include "Item/Weapon/WeaponStrategy/C_ThrowingWeaponStrategy.h"

#include "Components/ShapeComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"

#include "GameFramework/ProjectileMovementComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "NiagaraFunctionLibrary.h"

#include "Components/SplineMeshComponent.h"
#include "Components/SplineComponent.h"

#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"

#include "Character/Component/C_InvenComponent.h"
#include "Character/Component/C_InvenSystem.h"
#include "Character/Component/C_EquippedComponent.h"
#include "Character/Component/C_InvenSystem.h"	
#include "Character/C_AnimBasicCharacter.h"

#include "HUD/C_HUDWidget.h"
#include "HUD/C_MainMapWidget.h"
#include "HUD/C_AmmoWidget.h"

#include "I_ExplodeStrategy.h"
#include "C_GrenadeExplode.h"
#include "C_FlashBangExplode.h"
#include "C_SmokeGrndExplode.h"
#include "AIThrowableAttackStrategy/C_AIFlashBangAttackStrategy.h"
#include "AIThrowableAttackStrategy/C_AIGrenadeAttackStrategy.h"
#include "AIThrowableAttackStrategy/C_AISmokeGrenadeAttackStrategy.h"
#include "AIThrowableAttackStrategy/I_AIThrowableAttackStrategy.h"
#include "Character/C_Enemy.h"

#include "Utility/C_Util.h"

#include "Singleton/C_GameSceneManager.h"

const float AC_ThrowingWeapon::UP_DIR_BOOST_OFFSET = 500.f;

int AC_ThrowingWeapon::ThrowingWeaponCount{};

TMap<EThrowableType, II_ExplodeStrategy*> AC_ThrowingWeapon::ExplodeStrategies{};

const TMap<EThrowableType, FName> AC_ThrowingWeapon::THROWABLETYPE_ITEMNAME_MAP =
{
	{EThrowableType::GRENADE,		"Item_Weapon_Grenade_C"},
	{EThrowableType::FLASH_BANG,	"Item_Weapon_FlashBang_C"},
	{EThrowableType::SMOKE,			"Item_Weapon_SmokeBomb_C"},
};

USkeletalMeshComponent* AC_ThrowingWeapon::OwnerMeshTemp{};

const TMap<EThrowableType, FName> AC_ThrowingWeapon::EQUIPPED_SOCKET_NAMES =
{
	{EThrowableType::GRENADE,		"GrenadeEquipped"},
	{EThrowableType::FLASH_BANG,	"FlashBangEquipped"},
	{EThrowableType::SMOKE,			"SmokeEquipped"}
};

const FName AC_ThrowingWeapon::HOLSTER_SOCKET_NAME		= "Throwable_Holster";
const FName AC_ThrowingWeapon::THROW_START_SOCKET_NAME	= "Throwable_ThrowStart";

const float AC_ThrowingWeapon::PROJECTILE_RADIUS		= 5.f;

TMap<EThrowableType, class II_AIThrowableAttackStrategy*> AC_ThrowingWeapon::AIAttackStrategies{};

AC_ThrowingWeapon::AC_ThrowingWeapon()
{
	WeaponButtonStrategy = CreateDefaultSubobject<AC_ThrowingWeaponStrategy>("ThrowingWeaponStrategy");

	//RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRootComponent"));

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");

	Collider = CreateDefaultSubobject<UCapsuleComponent>("Capsule");
	Collider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetRootComponent(Collider);

	PathSpline = CreateDefaultSubobject<USplineComponent>("PredictedPathSpline");
	PredictedEndPoint = CreateDefaultSubobject<UStaticMeshComponent>("PredictedPathEndPointMesh");
	PredictedEndPoint->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PredictedEndPoint->SetVisibility(false);

	//ItemType 설정.
	//ItemDataRef.ItemType = EItemTypes::THROWABLE;

	ExplosionSphere = CreateDefaultSubobject<USphereComponent>("ExplosionSphere");
}

void AC_ThrowingWeapon::BeginPlay()
{
	Super::BeginPlay();

	ProjectileMovement->Deactivate();

	ThrowingWeaponCount++;

	// Init ExplodeStrategies
	if (ExplodeStrategies.IsEmpty())
	{
		II_ExplodeStrategy* GrenadeExplode = NewObject<AC_GrenadeExplode>();
		GrenadeExplode->_getUObject()->AddToRoot(); // GC 방지

		II_ExplodeStrategy* FlashBangExplode = NewObject<AC_FlashBangExplode>();
		FlashBangExplode->_getUObject()->AddToRoot();

		II_ExplodeStrategy* SmokeExplode = NewObject<AC_SmokeGrndExplode>();
		SmokeExplode->_getUObject()->AddToRoot();

		GAMESCENE_MANAGER->AddGCProtectedObject(GrenadeExplode->_getUObject());
		GAMESCENE_MANAGER->AddGCProtectedObject(FlashBangExplode->_getUObject());
		GAMESCENE_MANAGER->AddGCProtectedObject(SmokeExplode->_getUObject());

		ExplodeStrategies.Add(EThrowableType::GRENADE,	GrenadeExplode);
		ExplodeStrategies.Add(EThrowableType::FLASH_BANG,	FlashBangExplode);
		ExplodeStrategies.Add(EThrowableType::SMOKE,		SmokeExplode);
	}

	// Init Explode Strategy
	ExplodeStrategy = ExplodeStrategies[ThrowableType];

	// Init AI Attack Strategies
	if (AIAttackStrategies.IsEmpty())
	{
		II_AIThrowableAttackStrategy* GrenadeAttackStrategy = NewObject<UC_AIGrenadeAttackStrategy>();
		GrenadeAttackStrategy->_getUObject()->AddToRoot();

		II_AIThrowableAttackStrategy* FlashBangAttackStrategy = NewObject<UC_AIFlashBangAttackStrategy>();
		FlashBangAttackStrategy->_getUObject()->AddToRoot();
		
		II_AIThrowableAttackStrategy* SmokeGrenadeAttackStrategy = NewObject<UC_AISmokeGrenadeAttackStrategy>();
		SmokeGrenadeAttackStrategy->_getUObject()->AddToRoot();


		GAMESCENE_MANAGER->AddGCProtectedObject(GrenadeAttackStrategy->_getUObject());
		GAMESCENE_MANAGER->AddGCProtectedObject(FlashBangAttackStrategy->_getUObject());
		GAMESCENE_MANAGER->AddGCProtectedObject(SmokeGrenadeAttackStrategy->_getUObject());

		AIAttackStrategies.Add(EThrowableType::GRENADE,		GrenadeAttackStrategy);
		AIAttackStrategies.Add(EThrowableType::FLASH_BANG,	FlashBangAttackStrategy);
		AIAttackStrategies.Add(EThrowableType::SMOKE,		SmokeGrenadeAttackStrategy);
	}

	// 자기 자신의 AIAttack 전략 init
	AIAttackStrategy = AIAttackStrategies[ThrowableType];
}

void AC_ThrowingWeapon::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (EndPlayReason == EEndPlayReason::Destroyed)
	{
		if (--ThrowingWeaponCount <= 0) // World에 배치된 마지막 ThrowingWeapon이 Destroy되었을 때
		{
			if (OwnerMeshTemp)
			{
				OwnerMeshTemp->DestroyComponent();
				OwnerMeshTemp = nullptr;
			}

			if (!ExplodeStrategies.IsEmpty()) ExplodeStrategies.Empty(); // GC는 GameSceneManager에서 처리
			if (!AIAttackStrategies.IsEmpty()) AIAttackStrategies.Empty(); // GC는 GSMgr에서 처리
		}
		return;
	}

	ThrowingWeaponCount = 0;

	if (OwnerMeshTemp)
	{
		OwnerMeshTemp->DestroyComponent();
		OwnerMeshTemp = nullptr;
	}

	// GC는 GameSceneManager에서 처리
	if (!ExplodeStrategies.IsEmpty())  ExplodeStrategies.Empty(); 
	if (!AIAttackStrategies.IsEmpty()) AIAttackStrategies.Empty();
}

void AC_ThrowingWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateCurMontagesToOwnerCharacterPoseState();
}

bool AC_ThrowingWeapon::UpdateCurMontagesToOwnerCharacterPoseState()
{
	if (!IsValid(OwnerCharacter)) return false;

	CurDrawMontage			= DrawMontages[OwnerCharacter->GetPoseState()];
	CurSheathMontage		= SheathMontages[OwnerCharacter->GetPoseState()];
	CurThrowProcessMontages = ThrowProcessMontages[OwnerCharacter->GetPoseState()];

	return true;
}

  bool AC_ThrowingWeapon::AttachToHolster(USceneComponent* InParent)
{
	// 투척류를 핀까지만 뽑았고 쿠킹을 안했을 시 다시 집어넣음
	// 투척류를 안전손잡이까지 뽑았다면 현재 위치에 현재 투척류 그냥 바닥에 떨굼

	SetActorHiddenInGame(true);
	

	ProjectileMovement->Deactivate();

	return AttachToComponent
	(
		InParent,
		FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
		HOLSTER_SOCKET_NAME
	);
}

bool AC_ThrowingWeapon::AttachToHand(USceneComponent* InParent)
{
	//this->SetHidden(false);

	SetActorHiddenInGame(false);
	//SetActorHiddenInGame(true);


	ProjectileMovement->Deactivate();
	ClearSpline();

	OwnerCharacter->SetHandState(EHandState::WEAPON_THROWABLE);
	
	// Self init
	bIsCharging = false;
	bIsOnThrowProcess = false;

	if (AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharacter))
	{
		UC_AmmoWidget* AmmoWidget = OwnerPlayer->GetHUDWidget()->GetAmmoWidget();
		AmmoWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible, false);
		
		AC_Item* InvenThrowable = OwnerCharacter->GetInvenComponent()->FindMyItemByName(ItemCode);
		int MagazineCount = !InvenThrowable ? 1 : InvenThrowable->GetItemCurStack() + 1;
		AmmoWidget->SetMagazineText(MagazineCount);
	}

	return AttachToComponent
	(
		InParent,
		// FAttachmentTransformRules(EAttachmentRule::KeepRelative, true),
		FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
		EQUIPPED_SOCKET_NAMES[ThrowableType]
	);
}

void AC_ThrowingWeapon::InitializeItem(FName NewItemCode)
{
	Super::InitializeItem(NewItemCode);

	static const FString ContextString(TEXT("GunItem Lookup"));
	
	//TODO : 나중에 ItemManager를 통해 아이템을 모두 관리하게 되면 ItemManager를 통해서 GunSoundData 정의해 주기.
	UDataTable* GunSoundDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Project_PUBG/Common/Item/ItemDataTables/DT_ThrowingWeaponSoundData.DT_ThrowingWeaponSoundData"));

	if (GunSoundDataTable)
	{
		const FThrowingWeaponSoundData* ItemData = GunSoundDataTable->FindRow<FThrowingWeaponSoundData>(ItemCode, ContextString);
		if (ItemData)
		{
			ThrowingWeaponSoundData = ItemData;  // 원본 참조 저장
		}
	}
}

  FName AC_ThrowingWeapon::GetThrowableItemName(EThrowableType TargetType)
  {
	if (TargetType == EThrowableType::MAX)
	{
		UC_Util::Print("From AC_ThrowingWeapon::GetThrowableItemName : Type Max received!", FColor::Red, 10.f);
		return "";
	}
	
	return THROWABLETYPE_ITEMNAME_MAP[TargetType];
  }

  void AC_ThrowingWeapon::PickUpItem(AC_BasicCharacter* Character)
{
	//여기서하는 용량체크는 인벤에서 이미 한번 처리 되었지만 혹시몰라 넣어 놓은것으로 확인후 제거할 것.
	//인벤에서 체크하지 않고 아이템에서 체크하는 방식으로 가야 할듯.
	
	//EquipToCharacter(Character);
	LegacyMoveToSlot(Character);
}

void AC_ThrowingWeapon::DropItem(AC_BasicCharacter* Character)
{
	
	//TODO : 아이템이 장착(Attach)되었던 상태를 해제하는 작업에 관한 처리 생각
	//TODO : 분할해서 버리는 경우 새로 스폰해주어야함.
	
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	
	ItemPlace = EItemPlace::AROUND;
	SetOwnerCharacter(nullptr);               //OwnerCharacter 해제
	SetActorHiddenInGame(false);			  //모습이 보이도록 Hidden 해제.
	SetActorEnableCollision(true);			  //Overlap가능 하도록 Collision On
	Collider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	//바닥 레이 캐스팅 받아와서 바닥에 아이템 생성하기.
	SetActorLocation(GetGroundLocation(Character) + RootComponent->Bounds.BoxExtent.Z);
}

bool AC_ThrowingWeapon::LegacyMoveToInven(AC_BasicCharacter* Character)
{

	UC_InvenComponent*    invenComp = Character->GetInvenComponent();
	UC_EquippedComponent* equipComp = Character->GetEquippedComponent();

	uint8 ItemStackCount = invenComp->LoopCheckVolume(this);

	if (ItemStackCount == 0)
	{
		UC_Util::Print("Not Enough Volume");
		return false; //인벤에 넣을 수 있는 아이템의 갯수가 0 이면 넣을 수 없으므로 return false;
	}

	AC_Item* FoundItem = invenComp->FindMyItem(this); //인벤에 같은 아이템을 찾아옴, 없다면 nullptr;

	/* 이 밑으로 모두 MoveToInven 가능한 상황 */

	// 현재 손에 들고있는 투척류가 this라면 HandState NONE으로 전환
	if (equipComp->GetCurWeapon() == this) equipComp->ChangeCurWeapon(EWeaponSlot::NONE);

	if (ItemCurStack == ItemStackCount)
	{
		//아이템 전부를 인벤에 넣을 수 있을 때.
		if (IsValid(FoundItem))
		{
			//인벤에 해당 아이템이 존재 할 때.
			FoundItem->SetItemStack(FoundItem->GetItemCurStack() + ItemStackCount);
			//invenComp->GetCurVolume() += FoundItem->GetItemDatas().ItemVolume * ItemStackCount;
			//TODO : destroy를 해도 잔상이 남는것을 대비해서 해놓음 만약 없이도 잔상이 안남는다면 지울 것.
			invenComp->AddInvenCurVolume(this->ItemDataRef->ItemVolume * ItemStackCount);

			this->SetActorEnableCollision(false);
			this->SetActorHiddenInGame(true);

			this->Destroy();
			return true;
		}
		else
		{
			//인벤에 해당 아이템이 존재하지 않을 때.
			invenComp->AddItemToMyList(this);
			//인게임에서 보이는 것과 collision문제 때문에 임시로 꺼둠.
			//this->SetActorEnableCollision(false);
			this->SetActorHiddenInGame(true);
			//던질 때 켜짐. 이걸로 만약 아이템의 오버랩이 안끝난다면 다른 방법 고민->ToInven에서 SetActorEnableCollision를 꺼주고 던질때 혹은 ToAround에서 켜주기.
			Collider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			return true;
		}
	}
	else
	{
		//아이템의 일부만 인벤에 넣을 수 있을 때.
		if (IsValid(FoundItem))
		{
			     this->SetItemStack(ItemCurStack - ItemStackCount);
			FoundItem->SetItemStack(FoundItem->GetItemCurStack() + ItemStackCount);

			invenComp->AddInvenCurVolume(this->ItemDataRef->ItemVolume * ItemStackCount);

			return true;
		}
		else
		{
			AC_Weapon* NewItem = Cast<AC_Weapon>(SpawnItem(Character));//아이템 복제 생성
			NewItem->SetItemStack(ItemStackCount);
			   this->SetItemStack(ItemCurStack - ItemStackCount);

			invenComp->AddItemToMyList(NewItem);

			NewItem->SetActorHiddenInGame(true);
			//collider 관련 설정 추가해야 할 수 있음.
			//만약 추가해야 된다면 MoveToInven에서 SetActorEnableCollision을 꺼주고 던질 때 켜주는 방식으로.
			return true;
		}
	}
}

bool AC_ThrowingWeapon::LegacyMoveToAround(AC_BasicCharacter* Character)
{
	if (!Character) return false;

	UC_EquippedComponent* EquippedComponent = Character->GetEquippedComponent();

	// Slot에 장착된 무기를 MoveToAround처리하는 상황
	if (EquippedComponent->GetWeapons()[EWeaponSlot::THROWABLE_WEAPON] == this)
	{
		// 만약 현재 손에 들고 있던 무기를 MoveToAround처리 했을 때
		if (EquippedComponent->GetCurWeapon() == this)
			EquippedComponent->ChangeCurWeapon(EWeaponSlot::NONE);
		
		EquippedComponent->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, nullptr);
	}

	if (ItemPlace == EItemPlace::INVEN)
		Character->GetInvenComponent()->RemoveItemToMyList(this);
	else if (ItemPlace == EItemPlace::SLOT)
	{
		Character->GetEquippedComponent()->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, nullptr);
		//this->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
	}
	
	//DetachmentItem();

	//TODO: 분할해서 버리는 경우 새로 스폰해주어야함.
	ItemPlace = EItemPlace::AROUND;
	SetOwnerCharacter(nullptr);
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	Collider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	
	//바닥 레이 캐스팅 받아와서 바닥에 아이템 생성하기.
	SetActorLocation(GetGroundLocation(Character) + RootComponent->Bounds.BoxExtent.Z);
	
	//SetActorRotation(FQuat(0,0,0));

	return true;
}

bool AC_ThrowingWeapon::LegacyMoveToSlot(AC_BasicCharacter* Character)
{
	UC_EquippedComponent*	equipComp = Character->GetEquippedComponent();
	UC_InvenComponent*		invenComp = Character->GetInvenComponent();
	AC_Weapon*				curWeapon = equipComp->GetWeapons()[EWeaponSlot::THROWABLE_WEAPON];

	AC_Weapon* curWeapaon = equipComp->GetWeapons()[EWeaponSlot::THROWABLE_WEAPON];

	// 슬롯에 무기가 장착되어 있을 때
	if (IsValid(curWeapon))
	{
		AC_ThrowingWeapon* CurSlotThrowWeapon = Cast<AC_ThrowingWeapon>(curWeapon);

		if (!CurSlotThrowWeapon)
		{
			UC_Util::Print("From AC_ThrowingWeapon::MoveToSlot : Casting failed!", FColor::Red, 10.f);
			return false;
		}

		// 기존에 slot에 장착된 ThrowableWeapon이 ThrowProcess 중이라면 return false
		if (CurSlotThrowWeapon->GetIsOnThrowProcess()) return false;

		AC_Weapon* PrevSlotWeapon = nullptr;
		AC_Item* FoundItem		  = nullptr;
		//int nextVolume = invenComp->GetCurVolume() - ItemDatas.ItemVolume + curWeapaon->GetItemDatas().ItemVolume; //이건 인벤에 존재하는 아이템을 옮길때만 유효 한거 같은데?
		int nextVolume = 0;

		nextVolume = (this->ItemPlace == EItemPlace::INVEN) ? invenComp->GetCurVolume() -
																		ItemDataRef->ItemVolume +
																		curWeapon->GetItemDatas()->ItemVolume 
																		:
																		invenComp->GetCurVolume() + curWeapon->GetItemDatas()->ItemVolume;

		if (nextVolume > invenComp->GetMaxVolume())
		{
			UC_Util::Print("Not Enough Volume");
			return false;
		}

		//SetActorEnableCollision(false);
		if (ItemCurStack == 1)
		{
			if (this->ItemPlace == EItemPlace::INVEN)       invenComp->RemoveItemToMyList(this);//아마 InvenUI를 초기화 시켜주는 작업이 추가적으로 필요할것.
			else if (this->ItemPlace == EItemPlace::AROUND) invenComp->RemoveItemToAroundList(this);

			PrevSlotWeapon = equipComp->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, this);
		}
		else
		{
			AC_Weapon* InToSlotWeapon = Cast<AC_Weapon>(SpawnItem(Character));
			InToSlotWeapon->SetItemStack(1);
			this->SetItemStack(ItemCurStack - 1);

			//if (ItemDatas.ItemPlace == EItemPlace::AROUND) return this->LegacyMoveToInven(Character);//하나를 뺴서 장착시킨 아이템을 MoveToInven으로 인벤에 넣는 작업 실행.
			//else if (ItemDatas.ItemPlace == EItemPlace::INVEN) return true;
			//else return false;

			PrevSlotWeapon = equipComp->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, InToSlotWeapon);

		}

		// 손에 현 slot 무기를 장착 중일 때 예외 처리
		if (Character->GetHandState() == EHandState::WEAPON_THROWABLE)
		{
			// 기존 투척류 AttachToHolster 처리하고 새로운 투척류 꺼내기
			UC_Util::Print("HandState Throwable set slot Exception Handling", FColor::Red, 10.f);
			PrevSlotWeapon->AttachToHolster(Character->GetMesh());
			equipComp->SetNextWeaponType(EWeaponSlot::THROWABLE_WEAPON);
			Character->PlayAnimMontage(this->GetCurDrawMontage());
		}

		//if (OutToSlotWeapon->MoveToInven(Character)) return true;

		FoundItem = invenComp->FindMyItem(PrevSlotWeapon); //인벤에 해당 아이템이 존재하는지 확인

		if (IsValid(FoundItem))
		{
			//FoundItem->SetItemStack(FoundItem->GetItemDatas().ItemStack + 1);
			FoundItem->AddItemStack();
			//TODO:잔상이 생긴다면 추가 작업 필요.
			PrevSlotWeapon->Destroy();
			return true;
		}

		invenComp->AddItemToMyList(PrevSlotWeapon);
		return true;
	}

	//슬롯에 아이템이 없을 때

	if (ItemCurStack == 1)
	{
		//this의 stack이 1 이라면 실행.
		//this를 슬롯에 장착 하고 목록에서 제거

		if      (ItemPlace == EItemPlace::AROUND) invenComp->RemoveItemToAroundList(this);
		else if (ItemPlace == EItemPlace::INVEN)  invenComp->RemoveItemToMyList(this);

		equipComp->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, this);
		return true;
	}

	//this의 스택이 1이 아닐 때
	
	//this를 복제 생성해서 하나는 장착하고 나머지는 갯수를 수정
	AC_Weapon* NewWeapon = Cast<AC_Weapon>(SpawnItem(Character));
	NewWeapon->SetItemStack(1);
	     this->SetItemStack(ItemCurStack - 1);
	
	equipComp->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, NewWeapon); //슬롯에 장착. 아래의 NewWeapon과 다름.
	return false;
	//return  (ItemPlace == EItemPlace::AROUND) ? this->MoveToInven(Character) : //하나를 뺴서 장착시킨 아이템을 MoveToInven으로 인벤에 넣는 작업 실행.
	//		(ItemPlace == EItemPlace::INVEN)  ? true : false;
}

bool AC_ThrowingWeapon::Interaction(AC_BasicCharacter* Character)
{
	//if (Character->GetInvenSystem()->GetInvenUI()->GetIsDragging())
	//{
	//	switch (ItemDatas.ItemPlace)
	//	{
	//	case EItemPlace::AROUND:
	//		break;
	//	default:
	//		break;
	//	}
	//	
	//	return MoveToAround(Character);
	//}
	AC_Weapon* curWeapaon = Character->GetEquippedComponent()->GetWeapons()[EWeaponSlot::THROWABLE_WEAPON];

	//if (curWeapaon)
	//{
	//	return MoveToInven(Character);
	//}
	//else
	//{
	//	return MoveToSlot(Character);
	//}

	switch (ItemPlace)
	{
	case EItemPlace::SLOT:

	case EItemPlace::AROUND:
		if (curWeapaon) return MoveToInven(Character, curWeapaon->GetItemCurStack());
	case EItemPlace::INVEN:
		return MoveToSlot(Character, this->GetItemCurStack());
		break;
	default:
		break;
	}
	return false;
	//if (Character)
	//{
	//	MoveToSlot(Character);
	//	return true;
	//}
	////MoveToSlot(Character);

	//return false;
}

AC_Item* AC_ThrowingWeapon::SpawnItem(AC_BasicCharacter* Character)
{
	FActorSpawnParameters SpawnParams;
	//SpawnParams.Owner = Character;
	//location, rotation을 this의 것을 쓰는 것도 생각, 왜냐하면 지금 이상하게 날라가는 이유가 이것일 수 도 있음. -> 섬광탄이 터지고 충돌체가 남아있음.
	//AC_Weapon* SpawnItem = GetWorld()->SpawnActor<AC_Weapon>(this->GetClass(), Character->GetActorLocation() - FVector(0,0,50), Character->GetActorRotation(), SpawnParams);
	AC_Weapon* SpawnItem = GetWorld()->SpawnActor<AC_Weapon>(this->GetClass(), GetGroundLocation(Character) + RootComponent->Bounds.BoxExtent.Z, Character->GetActorRotation(), SpawnParams);

	//SpawnItem->SetItemStack(1);
	//SpawnItem->SetActorHiddenInGame(true);
	SpawnItem->SetActorEnableCollision(false);//생성될 때 무조건 OverlapBegine에 반응해서 우선 꺼뒀음.
	return SpawnItem;
}

bool AC_ThrowingWeapon::MoveSlotToAround(AC_BasicCharacter* Character, int32 InStack)
{
	UC_EquippedComponent* equipComp = Character->GetEquippedComponent();//TODO : 안쓰는건 삭제하기.
	UC_InvenComponent* invenComp = Character->GetInvenComponent();		//TODO : 안쓰는건 삭제하기.

	//Slot에 있다는건 장착된 상태라는 것. Around로 간다는건 장착을 해제하고 아이템을 땅에 떨군다는 뜻.
	AC_Weapon* curWeapon = equipComp->GetWeapons()[EWeaponSlot::THROWABLE_WEAPON];
	
	if (curWeapon != this) return false; //장착된 아이템이 자신이 아니라면 return.

	// 장착된 무기의 Sheath나 Draw Montage가 이미 재생 중이라면 return false
	UAnimInstance* CharacterAnimInstance = Character->GetMesh()->GetAnimInstance();
	if (CharacterAnimInstance->Montage_IsPlaying(CurDrawMontage.AnimMontage) ||
		CharacterAnimInstance->Montage_IsPlaying(CurSheathMontage.AnimMontage))
	{
		UC_Util::Print("Throwable SlotToAround Failed due to Sheath/draw Montage is currently playing!",
			FColor::Red,
			10.f);
		return false; 
	}
	
	equipComp->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, nullptr); //장착된 아이템이 자신이면, 장착해제를 진행.

	DropItem(Character);
	return true;
}

bool AC_ThrowingWeapon::MoveSlotToInven(AC_BasicCharacter* Character, int32 InStack)
{
	UC_EquippedComponent* equipComp = Character->GetEquippedComponent();//TODO : 안쓰는건 삭제하기.
	UC_InvenComponent* invenComp = Character->GetInvenComponent();		//TODO : 안쓰는건 삭제하기.
	//if (invenComp->GetMaxVolume() > invenComp->GetCurVolume() + GetOnceVolume())
	if (!invenComp->CheckVolume(this)) return false; //인벤에 this가 들어가서 curVolume > MaxVolume이 된다면 return.

	AC_Weapon* curWeapon = equipComp->GetWeapons()[EWeaponSlot::THROWABLE_WEAPON];

	if (curWeapon != this) return false; //장착된 아이템이 자신이 아니라면 return.

	equipComp->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, nullptr); //장착된 아이템이 자신이면, 장착해제를 진행.

	invenComp->AddItemToMyList(this);//장착 해제된 아이템을 내 아이템 리스트에 추가.
	
	return true;
}

bool AC_ThrowingWeapon::MoveInvenToAround(AC_BasicCharacter* Character, int32 InStack)
{
	UC_EquippedComponent* equipComp = Character->GetEquippedComponent();//TODO : 안쓰는건 삭제하기.
	UC_InvenComponent* invenComp = Character->GetInvenComponent();		//TODO : 안쓰는건 삭제하기.

	if (!invenComp->FindMyItem(this)) return false;

	// Inven에서 Around로 버리려 할 때, Sheath Montage나 DrawMontage가 재생중이라면 return false
	UAnimInstance* CharacterAnimInstance = Character->GetMesh()->GetAnimInstance();
	if (CharacterAnimInstance->Montage_IsPlaying(CurDrawMontage.AnimMontage) ||
		CharacterAnimInstance->Montage_IsPlaying(CurSheathMontage.AnimMontage))
	{
		UC_Util::Print("Throwable SlotToAround Failed due to Sheath/draw Montage is currently playing!",
			FColor::Red,
			10.f);
		return false; 
	}

	invenComp->RemoveItemToMyList(this);				 //내 아이템 리스트에서 아이템 제거.
														 
	//invenComp->AddInvenCurVolume(-this->GetAllVolume()); //버리는 아이템만큼 curVolume 조절하기. TODO : Inven에서 아이템 버릴 때 문제 생기면 체크하기.

	DropItem(Character);

	return true;
}

bool AC_ThrowingWeapon::MoveInvenToSlot(AC_BasicCharacter* Character, int32 InStack)
{
	UC_EquippedComponent* equipComp = Character->GetEquippedComponent();//TODO : 안쓰는건 삭제하기.
	UC_InvenComponent* invenComp = Character->GetInvenComponent();		//TODO : 안쓰는건 삭제하기.

	AC_Weapon* curWeapon = equipComp->GetWeapons()[EWeaponSlot::THROWABLE_WEAPON];

	if (curWeapon)
	{
		float prevVolume = invenComp->GetCurVolume() + curWeapon->GetOnceVolume() - this->GetOnceVolume();
		if (prevVolume > invenComp->GetMaxVolume()) return false; //교체하는 아이템이 인벤에 들어오면서 MaxVolume을 넘으면 return.
	}

	if (ItemCurStack == 1)
	{
		curWeapon = equipComp->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, this);
		invenComp->RemoveItemToMyList(this);
	}
	else
	{
		this->DeductItemStack();
		AC_ThrowingWeapon* SwapItem = Cast<AC_ThrowingWeapon>(SpawnItem(Character));
		SwapItem->SetItemStack(1);
		invenComp->AddInvenCurVolume(-SwapItem->GetItemDatas()->ItemVolume);
		curWeapon = equipComp->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, SwapItem);
	}

	// 손에 Throwable이 들려있는 상황에서 새로운 투척류로 slot을 교체했을 때
	if (Character->GetHandState() == EHandState::WEAPON_THROWABLE)
	{
		// curWeapon == prevSlotWeapon
		AC_ThrowingWeapon* prevThrowWeapon = Cast<AC_ThrowingWeapon>(curWeapon);
		if (!prevThrowWeapon) return false;

		// 이전과 같은 종류의 Throwable일 때, this를 손에 붙임
		if (prevThrowWeapon->ThrowableType == this->ThrowableType)
			this->AttachToHand(Character->GetMesh());
		else
		{
			// 이전과 같은 종류의 무기가 아닐 때에는 무기 전환 animation 처리
			curWeapon->AttachToHolster(Character->GetMesh());
			equipComp->SetNextWeaponType(EWeaponSlot::THROWABLE_WEAPON);
			Character->PlayAnimMontage(this->GetCurDrawMontage());
		}
	}

	//Character->PlayAnimMontage(this->GetCurDrawMontage());
	invenComp->AddItemToMyList(curWeapon);//내부에서 매개변수 nullptr가 들어오면 return시켜버림. TODO : curWeapon을 정의한 뒤에 equipComp->GetWeapons()[EWeaponSlot::THROWABLE_WEAPON]의 값이 바뀌었으므로 역참조를 하면 문제가 생김. 확인 할 것.
	//if (curWeapon) //TODO : InvenComp->AddItemToMyList(nullptr)이 문제 생기면 활성화 
	
	return true;
}

bool AC_ThrowingWeapon::MoveAroundToInven(AC_BasicCharacter* Character, int32 InStack)
{
	UC_EquippedComponent* equipComp = Character->GetEquippedComponent();//TODO : 안쓰는건 삭제하기.
	UC_InvenComponent* invenComp = Character->GetInvenComponent();		//TODO : 안쓰는건 삭제하기.

	uint8 ItemStackCount = invenComp->LoopCheckVolume(this); //아이템Stack을 몇개까지 인벤에 넣을 수 있는가?

	if (ItemStackCount == 0)
	{
		UC_Util::Print("Not Enough Volume");
		return false; //인벤에 넣을 수 있는 아이템의 갯수가 0 이면 넣을 수 없으므로 return false;
	}

	if (ItemStackCount == ItemCurStack)
	{
		//아이템을 전부 인벤에 넣을 수 있는 경우.
		invenComp->AddItemToMyList(this);
		return true;
	}
	else 
	{
		//아이템을 전부 넣을 수 없는 경우.
		this->SetItemStack(ItemCurStack - ItemStackCount);//현재 객체의 stack을 조절
		AC_Weapon* SpawnedItem = Cast<AC_Weapon>(SpawnItem(Character));  //동일한 아이템 객체를 생성
		SpawnedItem->SetItemStack(ItemStackCount);						 //생성한 아이템 stack을 설정
		invenComp->AddItemToMyList(SpawnedItem);						 //inven에 추가.
		return true;
	}
}

bool AC_ThrowingWeapon::MoveAroundToSlot(AC_BasicCharacter* Character, int32 InStack)
{
	UC_EquippedComponent* equipComp = Character->GetEquippedComponent();//TODO : 안쓰는건 삭제하기.
	UC_InvenComponent* invenComp = Character->GetInvenComponent();		//TODO : 안쓰는건 삭제하기.

	AC_ThrowingWeapon* curWeapon = Cast<AC_ThrowingWeapon>(equipComp->GetWeapons()[EWeaponSlot::THROWABLE_WEAPON]);

	if (curWeapon)
	{
		if (!curWeapon->MoveSlotToInven(Character,InStack)) //이곳에서 curWeapon의 장착을 해제함.
			curWeapon->MoveSlotToAround(Character, InStack);
	}

	if (ItemCurStack == 1)
	{
		equipComp->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, this);
		invenComp->RemoveItemToAroundList(this);
	}
	else
	{
		this->DeductItemStack();
		AC_ThrowingWeapon* SwapItem = Cast<AC_ThrowingWeapon>(SpawnItem(Character));
		SwapItem->SetItemStack(1);
		equipComp->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, SwapItem);
	}
	return true;
}

void AC_ThrowingWeapon::OnRemovePinFin()
{
	OwnerCharacter->PlayAnimMontage(CurThrowProcessMontages.ThrowReadyMontage);

	if (ThrowingWeaponSoundData->PinPullSound) UGameplayStatics::PlaySoundAtLocation(this, ThrowingWeaponSoundData->PinPullSound, GetActorLocation());
}

void AC_ThrowingWeapon::OnThrowReadyLoop()
{
	// Player HUD Opened 관련 예외처리
	if (AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharacter))
	{
		// UI 창이 켜졌을 때
		if (OwnerPlayer->GetInvenSystem()->GetInvenUI()->GetIsPanelOpened() ||
			OwnerPlayer->GetMainMapWidget()->GetIsPanelOpened())
		{
			ClearSpline();

			if (bIsCooked) // cooking 되었다면 던져버림
			{
				OwnerCharacter->PlayAnimMontage(CurThrowProcessMontages.ThrowMontage);
				return;
			}

			// Cooking 되어있지 않다면 다시 Idle 자세로 돌아옴
			OwnerPlayer->GetMesh()->GetAnimInstance()->Montage_Stop(0.2f, CurThrowProcessMontages.ThrowReadyMontage.AnimMontage);
			bIsOnThrowProcess	= false;
			bIsCharging			= false;
			return;
		}
	}

	if (bIsCharging)
	{
		// Charging 중 처리 -> Player일 경우, 예상 경로 그려주기
		if (Cast<AC_Player>(OwnerCharacter)) HandlePlayerPredictedPath();
		return;
	}

	// Charging이 풀렸을 때 다음 던지기 동작 실행
	ClearSpline();
	OwnerCharacter->PlayAnimMontage(CurThrowProcessMontages.ThrowMontage);
}

void AC_ThrowingWeapon::OnThrowThrowable()
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	if (Cast<AC_Player>(OwnerCharacter)) UpdatePlayerProjectileLaunchValues();

	Collider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	this->SetActorLocation(ProjStartLocation);
	
	ProjectileMovement->Velocity = ProjLaunchVelocity;
	ProjectileMovement->Activate();

	if (!bIsCooked) StartCooking();
}

void AC_ThrowingWeapon::OnThrowProcessEnd()
{
	bIsOnThrowProcess	= false;
	bIsCharging			= false;
	
	AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharacter);

	SetActorEnableCollision(true);//장착 할 때 껏던 충돌 켜주기.

	// Ready 도중 ProcessEnd가 될 수 있기 때문에 Predicted Path spline 모두 지워주기
	ClearSpline();

	// 현재 Throw AnimMontage 멈추기 (우선순위 때문에 멈춰야 함)
	//OwnerCharacter->GetMesh()->GetAnimInstance()->Montage_Stop(0.2f, CurThrowProcessMontages.ThrowMontage.AnimMontage);

	UAnimInstance* OwnerAnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();

	//OwnerAnimInstance->StopAllMontages(0.2f);

	// 중간에 자세를 바꿀 수 있기 때문에 모든 자세별 AnimMontage playing 체크
	for (uint8 p = 0; p < static_cast<uint8>(EPoseState::POSE_MAX); p++)
	{
		EPoseState	  PoseState	   = static_cast<EPoseState>(p);
		UAnimMontage* ThrowMontage = ThrowProcessMontages[PoseState].ThrowMontage.AnimMontage;
		UAnimMontage* ReadyMontage = ThrowProcessMontages[PoseState].ThrowReadyMontage.AnimMontage;

		UAnimMontage* TargetMontage = OwnerAnimInstance->Montage_IsPlaying(ThrowMontage) ? ThrowMontage :
									  OwnerAnimInstance->Montage_IsPlaying(ReadyMontage) ? ReadyMontage : nullptr;

		if (TargetMontage)
		{
			OwnerAnimInstance->Montage_Stop(0.2f, TargetMontage);
			break;
		}
	}

	UC_EquippedComponent* OwnerEquippedComponent = OwnerCharacter->GetEquippedComponent();

	// 현재 투척류 장착 해제 바로 하기 -> 이미 던진 무기에 대한 PoseTransitionEnd Delegate 해제 이루어짐
	AC_BasicCharacter* PrevOwnerCharacter = OwnerCharacter;

	OwnerEquippedComponent->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, nullptr);

	if (OwnerCharacter->GetMainState() == EMainState::DEAD) return;

	// 동일한 종류의 투척류 있는지 조사
	AC_Item* ThrowWeapon = OwnerCharacter->GetInvenComponent()->FindMyItem(this);
	if (ThrowWeapon)
	{
		AC_ThrowingWeapon* TargetThrowWeapon = Cast<AC_ThrowingWeapon>(ThrowWeapon);

		if (!TargetThrowWeapon)
		{
			UC_Util::Print("OnProcessEnd Change to Last New Throwing Weapon casting failed!", FColor::Red, 10.f);
			return;
		}
		TargetThrowWeapon->MoveToSlot(PrevOwnerCharacter,TargetThrowWeapon->GetItemCurStack());

		if (OwnerPlayer) OwnerPlayer->GetInvenSystem()->GetInvenUI()->UpdateWidget();

		// 바로 다음 투척류 꺼내기
		OwnerEquippedComponent->SetNextWeaponType(EWeaponSlot::THROWABLE_WEAPON);
		PrevOwnerCharacter->PlayAnimMontage(TargetThrowWeapon->GetCurDrawMontage());
		OwnerEquippedComponent->SetIsCurrentlyChangingWeapon(true);
		return;
	}

	// 다른 투척류 조사
	for (auto& Pair : THROWABLETYPE_ITEMNAME_MAP)
	{
		if (Pair.Key == this->ThrowableType) continue;

		ThrowWeapon = OwnerCharacter->GetInvenComponent()->FindMyItemByName(Pair.Value);
		if (!ThrowWeapon) continue;

		AC_ThrowingWeapon* TargetThrowWeapon = Cast<AC_ThrowingWeapon>(ThrowWeapon);

		if (!TargetThrowWeapon)
		{
			UC_Util::Print("OnProcessEnd Change to Last New Throwing Weapon casting failed!", FColor::Red, 10.f);
			return;
		}

		TargetThrowWeapon->MoveToSlot(PrevOwnerCharacter, TargetThrowWeapon->GetItemCurStack());

		if (OwnerPlayer) OwnerPlayer->GetInvenSystem()->GetInvenUI()->UpdateWidget();

		// 바로 다음 투척류 꺼내기
		OwnerEquippedComponent->SetNextWeaponType(EWeaponSlot::THROWABLE_WEAPON);
		PrevOwnerCharacter->PlayAnimMontage(TargetThrowWeapon->GetCurDrawMontage());
		OwnerEquippedComponent->SetIsCurrentlyChangingWeapon(true);
		return;
	}

	//남아 있는 투척류가 없다면 주무기1, 주무기2 순으로 장착 시도, 없다면 UnArmed인 상태로 돌아가기
	if (OwnerEquippedComponent->ChangeCurWeapon(EWeaponSlot::MAIN_GUN))		return;
	if (OwnerEquippedComponent->ChangeCurWeapon(EWeaponSlot::SUB_GUN))		return;

	OwnerEquippedComponent->ChangeCurWeapon(EWeaponSlot::NONE);
	if (OwnerPlayer) OwnerPlayer->GetHUDWidget()->GetAmmoWidget()->SetVisibility(ESlateVisibility::Hidden);
}

void AC_ThrowingWeapon::StartCooking()
{
	bIsCooked = true;

	// TODO : Player -> Ready 상태에서 Cooking 시작하면 남은 시간 HUD 띄우기
	GetWorld()->GetTimerManager().SetTimer(CookingTimerHandle, this, &AC_ThrowingWeapon::Explode, CookingTime, false);
	
	if (ThrowingWeaponSoundData->CookingSound) UGameplayStatics::PlaySoundAtLocation(this, ThrowingWeaponSoundData->CookingSound, GetActorLocation());
}

bool AC_ThrowingWeapon::ReleaseOnGround()
{
	
	if (!bIsCooked) return false;						// Cooking이 안되었다면 (안전손잡이가 날아가지 않았다면)
	if (ProjectileMovement->IsActive()) return false;	// 이미 던졌었다면

	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	Collider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	ProjectileMovement->Velocity = FVector(0.f, 0.f, 0.f); // 0.f의 속력으로 날리기
	ProjectileMovement->Activate();

	OnThrowProcessEnd();

	return true;
}

void AC_ThrowingWeapon::Explode()
{
	if (!ExplodeStrategy)
	{
		UC_Util::Print("From AC_ThrowingWeapon::Explode : Explode Strategy nullptr!", FColor::Red, 10.f);
		return;
	}

	bool Exploded = ExplodeStrategy->UseStrategy(this);
	if (GetAttachParentActor()) ReleaseOnGround(); // 손에서 아직 떠나지 않았을 때
	
	if (Exploded && ThrowingWeaponSoundData->ExplosionSound)
		UGameplayStatics::PlaySoundAtLocation(this, ThrowingWeaponSoundData->ExplosionSound, GetActorLocation());
}

FVector AC_ThrowingWeapon::GetPredictedThrowStartLocation()
{
	if (!IsValid(OwnerMeshTemp))
	{
		OwnerMeshTemp = NewObject<USkeletalMeshComponent>(OwnerCharacter);

		if (!IsValid(OwnerMeshTemp))
		{
			UC_Util::Print("From AC_ThrowingWeapon::GetPredictedThrowStartLocation : OwnerTempMesh Not inited!");
			return FVector::ZeroVector;
		}

		OwnerMeshTemp->SetVisibility(false);
		OwnerMeshTemp->RegisterComponent();
		OwnerMeshTemp->SetSkeletalMesh(OwnerCharacter->GetMesh()->SkeletalMesh);
		OwnerMeshTemp->SetAnimInstanceClass(OwnerCharacter->GetMesh()->GetAnimInstance()->GetClass());
		OwnerMeshTemp->SetWorldTransform(OwnerCharacter->GetMesh()->GetComponentTransform());
	}

	// 자세에 맞는 Montage가 재생중이지 않다면, 해당 Montage로 변경 뒤에 멈추기
	static EPoseState PredictedPoseState = EPoseState::POSE_MAX;

	UAnimInstance* MeshAnimInstance = OwnerMeshTemp->GetAnimInstance();

	// 현재 posing이 다르다면 예측 경로 시작점도 달라짐
	
	float ThrowThrowableTime{};

	for (const FAnimNotifyEvent& NotifyEvent : CurThrowProcessMontages.ThrowMontage.AnimMontage->Notifies)
	{
		if (NotifyEvent.NotifyName == "AN_OnThrowThrowable_C")
		{
			ThrowThrowableTime = NotifyEvent.GetTime();
			break;
		}
	}

	if (PredictedPoseState != OwnerCharacter->GetPoseState() ||
		MeshAnimInstance->Montage_GetPosition(CurThrowProcessMontages.ThrowMontage.AnimMontage) != ThrowThrowableTime)
	{
		PredictedPoseState = OwnerCharacter->GetPoseState();
		
		MeshAnimInstance->Montage_Play(CurThrowProcessMontages.ThrowMontage.AnimMontage);
		MeshAnimInstance->Montage_SetPosition(CurThrowProcessMontages.ThrowMontage.AnimMontage, ThrowThrowableTime);
		MeshAnimInstance->Montage_Pause();

		OwnerMeshTemp->TickPose(GetWorld()->GetDeltaSeconds(), true);
		OwnerMeshTemp->RefreshBoneTransforms();
		OwnerMeshTemp->RefreshFollowerComponents();
		OwnerMeshTemp->UpdateComponentToWorld();
	}

	OwnerMeshTemp->SetWorldTransform(OwnerCharacter->GetMesh()->GetComponentTransform());

	return OwnerMeshTemp->GetSocketLocation(THROW_START_SOCKET_NAME);
}

void AC_ThrowingWeapon::DrawDebugPredictedPath()
{
	static const float MaxSimTime	= 1.0f; // Max time to simulate
	static const float TimeStep		= 0.1f; // Time step for simulation
	FVector Gravity					= GetWorld()->GetGravityZ() * FVector::UpVector;

	TArray<FVector> PathPoints{};

	FVector CurrentLocation = GetPredictedThrowStartLocation();
	FVector CurrentVelocity = ProjLaunchVelocity;

	PathPoints.Add(CurrentLocation);

	for (float Time = 0; Time < MaxSimTime; Time += TimeStep)
	{
		FVector NextLocation = CurrentLocation + CurrentVelocity * TimeStep + 0.5f * Gravity * TimeStep * TimeStep;
		CurrentVelocity += Gravity * TimeStep;

		PathPoints.Add(NextLocation);
		CurrentLocation = NextLocation;

		if (PathPoints.Num() > 1)
		{
			DrawDebugLine
			(
				GetWorld(),
				PathPoints[PathPoints.Num() - 2], PathPoints[PathPoints.Num() - 1],
				FColor::Green,
				false,
				TimeStep * 2.0f,
				0,
				1.0f
			);
		}
	}
}

void AC_ThrowingWeapon::DrawPredictedPath()
{
	FPredictProjectilePathParams ProjectilePathParams{};
	ProjectilePathParams.StartLocation			= GetPredictedThrowStartLocation();
	ProjectilePathParams.LaunchVelocity			= ProjLaunchVelocity;
	ProjectilePathParams.bTraceWithCollision	= true;
	ProjectilePathParams.ProjectileRadius		= PROJECTILE_RADIUS;
	ProjectilePathParams.MaxSimTime				= 1.f;
	ProjectilePathParams.bTraceWithChannel		= true;
	ProjectilePathParams.TraceChannel			= ECollisionChannel::ECC_Visibility;
	ProjectilePathParams.ObjectTypes			= {};
	ProjectilePathParams.ActorsToIgnore			= { this };
	ProjectilePathParams.SimFrequency			= 20.f;
	ProjectilePathParams.OverrideGravityZ		= ProjectileMovement->GetGravityZ();
	ProjectilePathParams.DrawDebugType			= EDrawDebugTrace::None;
	ProjectilePathParams.DrawDebugTime			= 1.f;
	ProjectilePathParams.bTraceComplex			= false;

	
	FPredictProjectilePathResult Result{};

	bool IsHit = UGameplayStatics::PredictProjectilePath(GetWorld(), ProjectilePathParams, Result);

	// UGameplayStatics::SuggestProjectileVelocity()
	
	ClearSpline();

	TArray<FPredictProjectilePathPointData> PathData = Result.PathData;

	if (IsHit)
	{
		PredictedEndPoint->SetWorldLocation(PathData.Last().Location);
		PredictedEndPoint->SetVisibility(true);
	}

	for (auto& pointData : PathData) 
		PathSpline->AddSplinePoint(pointData.Location, ESplineCoordinateSpace::Local);

	PathSpline->SetSplinePointType(PathData.Num() - 1, ESplinePointType::CurveClamped);
	
	for (int i = 0; i < PathSpline->GetNumberOfSplinePoints() - 2; i++)
	{
		FVector FirstLocation{};
		FVector SecondLocation{};
		FVector FirstTangent{};
		FVector SecondTangent{};

		PathSpline->GetLocationAndTangentAtSplinePoint(i, FirstLocation, FirstTangent, ESplineCoordinateSpace::Local);
		PathSpline->GetLocationAndTangentAtSplinePoint(i + 1, SecondLocation, SecondTangent, ESplineCoordinateSpace::Local);

		USplineMeshComponent* SplineMeshComponent = NewObject<USplineMeshComponent>(this);
		SplineMeshComponent->SetStaticMesh(SplineMesh);
		SplineMeshComponent->SetStartAndEnd(FirstLocation, FirstTangent, SecondLocation, SecondTangent);
		SplineMeshComponent->SetMobility(EComponentMobility::Movable);
		SplineMeshComponent->RegisterComponentWithWorld(GetWorld());
		SplineMeshComponent->AttachToComponent(PathSpline, FAttachmentTransformRules::KeepRelativeTransform);
		//SplineMeshComponent->SetMaterial();
		
		SplineMeshes.Add(SplineMeshComponent);
	}
}

void AC_ThrowingWeapon::HandlePlayerPredictedPath()
{
	// 현재 OwnerCharacter의 손에 장착된 상황인지 확인
	if (!IsValid(this->GetAttachParentActor()) || this->GetAttachParentSocketName() != EQUIPPED_SOCKET_NAMES[ThrowableType]) return;

	if (!OwnerCharacter->GetMesh()->GetAnimInstance()->Montage_IsPlaying(CurThrowProcessMontages.ThrowReadyMontage.AnimMontage))
		return;

	UpdatePlayerProjectileLaunchValues();
	DrawPredictedPath();
	//DrawDebugPredictedPath();
}

void AC_ThrowingWeapon::UpdatePlayerProjectileLaunchValues()
{
	if (!Cast<AC_Player>(OwnerCharacter)) return;
	
	FRotator CameraRotation = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetCameraRotation();

	FVector Direction     = FRotationMatrix(CameraRotation).GetUnitAxis(EAxis::X).GetSafeNormal();	
	ProjStartLocation     = OwnerCharacter->GetMesh()->GetSocketLocation(THROW_START_SOCKET_NAME);
	ProjLaunchVelocity    = Direction * Speed;
	ProjLaunchVelocity.Z += UP_DIR_BOOST_OFFSET;
}

void AC_ThrowingWeapon::OnOwnerCharacterPoseTransitionFin()
{
	if (bIsCharging)
	{
		// 새로운 자세에 맞는 Anim Montage로 다시 재생
		OwnerCharacter->PlayAnimMontage(CurThrowProcessMontages.ThrowReadyMontage);
		return;
	}
}

void AC_ThrowingWeapon::ClearSpline()
{
	PredictedEndPoint->SetVisibility(false);

	for (auto& item : SplineMeshes) item->DestroyComponent();
	SplineMeshes.Empty();

	PathSpline->ClearSplinePoints();
}

bool AC_ThrowingWeapon::ExecuteAIAttack(AC_BasicCharacter* InTargetCharacter)
{
	return AIAttackStrategy->ExecuteAIAttack(this, InTargetCharacter);
}

bool AC_ThrowingWeapon::ExecuteAIAttackTickTask(class AC_BasicCharacter* InTargetCharacter, const float& DeltaTime)
{
	return AIAttackStrategy->ExecuteAIAttackTickTask(this, InTargetCharacter, DeltaTime);
}


