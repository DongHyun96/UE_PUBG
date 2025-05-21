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
#include "HUD/C_ThrowableProgressBar.h"

#include "Utility/C_Util.h"

#include "Singleton/C_GameSceneManager.h"
#include "Singleton/C_GameInstance.h"

const float AC_ThrowingWeapon::UP_DIR_BOOST_OFFSET = 500.f;

int AC_ThrowingWeapon::ThrowingWeaponCount{};

TMap<EThrowableType, II_ExplodeStrategy*> AC_ThrowingWeapon::ExplodeStrategies{};

const TMap<EThrowableType, FName> AC_ThrowingWeapon::THROWABLETYPE_ITEMNAME_MAP =
{
	{EThrowableType::GRENADE,		"Item_Weapon_Grenade_C"},
	{EThrowableType::FLASH_BANG,	"Item_Weapon_FlashBang_C"},
	{EThrowableType::SMOKE,			"Item_Weapon_SmokeBomb_C"},
};

const TMap<EThrowableType, FName> AC_ThrowingWeapon::EQUIPPED_SOCKET_NAMES =
{
	{EThrowableType::GRENADE,		"GrenadeEquipped"},
	{EThrowableType::FLASH_BANG,	"FlashBangEquipped"},
	{EThrowableType::SMOKE,			"SmokeEquipped"}
};

const FName AC_ThrowingWeapon::HOLSTER_SOCKET_NAME		= "Throwable_Holster";
const FName AC_ThrowingWeapon::THROW_START_SOCKET_NAME	= "Throwable_ThrowStart";

const float AC_ThrowingWeapon::PROJECTILE_RADIUS		= 5.f;

TMap<EThrowableType, II_AIThrowableAttackStrategy*> AC_ThrowingWeapon::AIAttackStrategies{};

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

		ExplodeStrategies.Add(EThrowableType::GRENADE,		GrenadeExplode);
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
			if (!ExplodeStrategies.IsEmpty())	ExplodeStrategies.Empty(); // GC는 GameSceneManager에서 처리
			if (!AIAttackStrategies.IsEmpty())	AIAttackStrategies.Empty(); // GC는 GSMgr에서 처리
		}
		return;
	}

	ThrowingWeaponCount = 0;

	// GC는 GameSceneManager에서 처리
	if (!ExplodeStrategies.IsEmpty())  ExplodeStrategies.Empty(); 
	if (!AIAttackStrategies.IsEmpty()) AIAttackStrategies.Empty();
}

void AC_ThrowingWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!IsValid(OwnerCharacter)) return;
	UpdateCurMontagesToOwnerCharacterPoseState();
	HandleAfterCooked(DeltaTime);
}

bool AC_ThrowingWeapon::UpdateCurMontagesToOwnerCharacterPoseState()
{
	if (!IsValid(OwnerCharacter)) return false;

	CurDrawMontage			= DrawMontages[OwnerCharacter->GetPoseState()];
	CurSheathMontage		= SheathMontages[OwnerCharacter->GetPoseState()];
	CurThrowProcessMontages = ThrowProcessMontages[OwnerCharacter->GetPoseState()];

	return true;
}

void AC_ThrowingWeapon::HandleAfterCooked(float DeltaTime)
{
	if (!bIsCooked || bExploded) return;
	
	CookingTimer += DeltaTime;

	// Set ThrowableProgressBar Percent
	if (AC_Player* Player = Cast<AC_Player>(OwnerCharacter))
	{
		UC_ThrowableProgressBar* ThrowableProgressBar = Player->GetHUDWidget()->GetThrowableProgressBar(); 
		
		if (GetAttachParentActor()) // 손에서 아직 떠나지 않았을 때
		{
			ThrowableProgressBar->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			// 조금 더 인위적으로 보이게끔 CookingTime 총 시간을 늘림
			const float CookingTimeExtend = FMath::GetMappedRangeValueClamped(FVector2D(2.f, 5.f), FVector2D(0.1f, 0.3f), CookingTime); 
			ThrowableProgressBar->SetPercent(CookingTimer / (CookingTime + CookingTimeExtend));
		}
	}
	
	if (CookingTimer > CookingTime) Explode(); // Time to explode
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

		UpdateAmmoWidgetMagazineText(OwnerPlayer);
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
	//UDataTable* GunSoundDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Project_PUBG/Common/Item/ItemDataTables/DT_ThrowingWeaponSoundData.DT_ThrowingWeaponSoundData"));

	UC_GameInstance* GI = Cast<UC_GameInstance>(GetGameInstance());

	UDataTable* GunSoundDataTable = GI->GetDataTables()[EDataTableType::ThrowingWeaponSound];

	if (GunSoundDataTable)
	{
		const FThrowingWeaponSoundData* ItemData = GunSoundDataTable->FindRow<FThrowingWeaponSoundData>(ItemCode, ContextString);
		if (ItemData)
		{
			ThrowingWeaponSoundData = ItemData;  // 원본 참조 저장
		}
		else UC_Util::Print("From AC_ThrowingWeapon::InitializeItem : ItemData not found in row(nullptr)", FColor::Red, 10.f);
	}
	else UC_Util::Print("From AC_ThrowingWeapon::InitializeItem : GunSoundDataTable not found(nullptr)", FColor::Red, 10.f);
		
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

//  void AC_ThrowingWeapon::PickUpItem(AC_BasicCharacter* Character)
//{
//	//여기서하는 용량체크는 인벤에서 이미 한번 처리 되었지만 혹시몰라 넣어 놓은것으로 확인후 제거할 것.
//	//인벤에서 체크하지 않고 아이템에서 체크하는 방식으로 가야 할듯.
//	
//	//EquipToCharacter(Character);
//	LegacyMoveToSlot(Character);
//}

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
	AC_Weapon* SpawnItem = GetWorld()->SpawnActor<AC_Weapon>(
		this->GetClass(), 
		GetGroundLocation(Character) + RootComponent->Bounds.BoxExtent.Z, 
		Character->GetActorRotation(), 
		SpawnParams
	);

	//SpawnItem->SetItemStack(1);
	//SpawnItem->SetActorHiddenInGame(true);
	SpawnItem->SetActorEnableCollision(false);//생성될 때 무조건 OverlapBegine에 반응해서 우선 꺼뒀음.
	GAMESCENE_MANAGER->AddSpawnedItemToContainer(this);
	return SpawnItem;
}

bool AC_ThrowingWeapon::MoveSlotToAround(AC_BasicCharacter* Character, int32 InStack)
{
	UC_EquippedComponent* EquippedComponent = Character->GetEquippedComponent();

	//Slot에 있다는건 장착된 상태라는 것. Around로 간다는건 장착을 해제하고 아이템을 땅에 떨군다는 뜻.
	AC_Weapon* curWeapon = EquippedComponent->GetWeapons()[EWeaponSlot::THROWABLE_WEAPON];
	
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
	
	EquippedComponent->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, nullptr); //장착된 아이템이 자신이면, 장착해제를 진행.

	DropItem(Character);
	return true;
}

bool AC_ThrowingWeapon::MoveSlotToInven(AC_BasicCharacter* Character, int32 InStack)
{
	UC_EquippedComponent* EquippedComponent = Character->GetEquippedComponent();
	UC_InvenComponent*    InvenComponent = Character->GetInvenComponent();		
	//if (invenComp->GetMaxVolume() > invenComp->GetCurVolume() + GetOnceVolume())
	if (!InvenComponent->CheckVolume(this)) return false; //인벤에 this가 들어가서 curVolume > MaxVolume이 된다면 return.

	AC_Weapon* curWeapon = EquippedComponent->GetWeapons()[EWeaponSlot::THROWABLE_WEAPON];

	if (curWeapon != this) return false; //장착된 아이템이 자신이 아니라면 return.

	EquippedComponent->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, nullptr); //장착된 아이템이 자신이면, 장착해제를 진행.

	InvenComponent->AddItemToMyList(this);//장착 해제된 아이템을 내 아이템 리스트에 추가.
	
	return true;
}

bool AC_ThrowingWeapon::MoveInvenToAround(AC_BasicCharacter* Character, int32 InStack)
{
	UC_InvenComponent* InvenComponent = Character->GetInvenComponent();

	if (!InvenComponent->FindMyItem(this)) return false;

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

	InvenComponent->RemoveItemToMyList(this);				 //내 아이템 리스트에서 아이템 제거.
														 
	//invenComp->AddInvenCurVolume(-this->GetAllVolume()); //버리는 아이템만큼 curVolume 조절하기. TODO : Inven에서 아이템 버릴 때 문제 생기면 체크하기.

	DropItem(Character);
	TryUpdateAmmoWidgetMagazineTextIfNecessary(Cast<AC_Player>(Character));
	return true;
}

bool AC_ThrowingWeapon::MoveInvenToSlot(AC_BasicCharacter* Character, int32 InStack)
{
	UC_EquippedComponent*	EquippedComponent = Character->GetEquippedComponent();
	UC_InvenComponent*		InvenComponent = Character->GetInvenComponent();		

	AC_Weapon* curWeapon = EquippedComponent->GetWeapons()[EWeaponSlot::THROWABLE_WEAPON];

	if (curWeapon)
	{
		float prevVolume = InvenComponent->GetCurVolume() + curWeapon->GetOnceVolume() - this->GetOnceVolume();
		if (prevVolume > InvenComponent->GetMaxVolume()) return false; //교체하는 아이템이 인벤에 들어오면서 MaxVolume을 넘으면 return.
	}

	if (ItemCurStack == 1)
	{
		curWeapon = EquippedComponent->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, this);
		InvenComponent->RemoveItemToMyList(this);
	}
	else
	{
		this->DeductItemStack();
		AC_ThrowingWeapon* SwapItem = Cast<AC_ThrowingWeapon>(SpawnItem(Character));
		SwapItem->SetItemStack(1);
		InvenComponent->AddInvenCurVolume(-SwapItem->GetItemDatas()->ItemVolume);
		curWeapon = EquippedComponent->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, SwapItem);
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
			EquippedComponent->SetNextWeaponType(EWeaponSlot::THROWABLE_WEAPON);
			Character->PlayAnimMontage(this->GetCurDrawMontage());
		}
	}

	//Character->PlayAnimMontage(this->GetCurDrawMontage());
	InvenComponent->AddItemToMyList(curWeapon);//내부에서 매개변수 nullptr가 들어오면 return시켜버림.
	// TODO : curWeapon을 정의한 뒤에 equipComp->GetWeapons()[EWeaponSlot::THROWABLE_WEAPON]의 값이 바뀌었으므로 역참조를 하면 문제가 생김. 확인 할 것.
	//if (curWeapon) //TODO : InvenComp->AddItemToMyList(nullptr)이 문제 생기면 활성화 
	
	return true;
}

bool AC_ThrowingWeapon::MoveAroundToInven(AC_BasicCharacter* Character, int32 InStack)
{
	UC_InvenComponent* InvenComponent = Character->GetInvenComponent();	

	uint8 ItemStackCount = InvenComponent->LoopCheckVolume(this); //아이템Stack을 몇개까지 인벤에 넣을 수 있는가?

	if (ItemStackCount == 0)
	{
		UC_Util::Print("Not Enough Volume");
		return false; //인벤에 넣을 수 있는 아이템의 갯수가 0 이면 넣을 수 없으므로 return false;
	}

	if (ItemStackCount == ItemCurStack)
	{
		//아이템을 전부 인벤에 넣을 수 있는 경우.
		InvenComponent->AddItemToMyList(this);
		TryUpdateAmmoWidgetMagazineTextIfNecessary(Cast<AC_Player>(Character));
		return true;
	}
	
	//아이템을 전부 넣을 수 없는 경우.
	this->SetItemStack(ItemCurStack - ItemStackCount);//현재 객체의 stack을 조절
	AC_Weapon* SpawnedItem = Cast<AC_Weapon>(SpawnItem(Character));  //동일한 아이템 객체를 생성
	SpawnedItem->SetItemStack(ItemStackCount);						 //생성한 아이템 stack을 설정
	InvenComponent->AddItemToMyList(SpawnedItem);						 //inven에 추가.
	TryUpdateAmmoWidgetMagazineTextIfNecessary(Cast<AC_Player>(Character));
	return true;
}

bool AC_ThrowingWeapon::MoveAroundToSlot(AC_BasicCharacter* Character, int32 InStack)
{
	UC_EquippedComponent*	EquippedComponent = Character->GetEquippedComponent();
	UC_InvenComponent*		InvenComponent = Character->GetInvenComponent();		

	AC_ThrowingWeapon* CurWeapon = Cast<AC_ThrowingWeapon>(EquippedComponent->GetWeapons()[EWeaponSlot::THROWABLE_WEAPON]);

	if (CurWeapon)
	{
		if (!CurWeapon->MoveSlotToInven(Character,InStack)) //이곳에서 curWeapon의 장착을 해제함.
			CurWeapon->MoveSlotToAround(Character, InStack);
	}

	if (ItemCurStack == 1)
	{
		EquippedComponent->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, this);
		InvenComponent->RemoveItemToAroundList(this);
	}
	else
	{
		this->DeductItemStack();
		AC_ThrowingWeapon* SwapItem = Cast<AC_ThrowingWeapon>(SpawnItem(Character));
		SwapItem->SetItemStack(1);
		EquippedComponent->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, SwapItem);
	}
	TryUpdateAmmoWidgetMagazineTextIfNecessary(Cast<AC_Player>(Character));
	return true;
}

void AC_ThrowingWeapon::OnRemovePinFin()
{
	OwnerCharacter->PlayAnimMontage(CurThrowProcessMontages.ThrowReadyMontage);

	if (!ThrowingWeaponSoundData)
	{
		UC_Util::Print("From AC_ThrowingWeapon::OnRemovePinFin : ThrowingWeaponSoundData nullptr!", FColor::Red, 10.f);
		return;
	}
	
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

	if (AC_Player* Player = Cast<AC_Player>(OwnerCharacter))
		Player->GetHUDWidget()->GetThrowableProgressBar()->SetVisibility(ESlateVisibility::Hidden);
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

	/*FTimerHandle& TimerHandle = GAMESCENE_MANAGER->GetTimerHandle();
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AC_ThrowingWeapon::Explode, CookingTime, false);*/

	if (!ThrowingWeaponSoundData) return;
	if (!ThrowingWeaponSoundData->CookingSound) return;
	
	UGameplayStatics::PlaySoundAtLocation(this, ThrowingWeaponSoundData->CookingSound, GetActorLocation());
}

bool AC_ThrowingWeapon::ReleaseOnGround()
{
	
	if (!bIsCooked) return false;						// Cooking이 안되었다면 (안전손잡이가 날아가지 않았다면)
	if (ProjectileMovement->IsActive()) return false;	// 이미 던졌었다면

	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	Collider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	ProjectileMovement->Velocity = FVector(0.f, 0.f, 0.f); // 0.f의 속력으로 날리기
	ProjectileMovement->Activate();

	if (AC_Player* Player = Cast<AC_Player>(OwnerCharacter))
		Player->GetHUDWidget()->GetThrowableProgressBar()->SetVisibility(ESlateVisibility::Hidden);

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

	bExploded = ExplodeStrategy->UseStrategy(this);
	if (GetAttachParentActor()) ReleaseOnGround(); // 손에서 아직 떠나지 않았을 때
	
	if (bExploded && ThrowingWeaponSoundData->ExplosionSound)
		UGameplayStatics::PlaySoundAtLocation(this, ThrowingWeaponSoundData->ExplosionSound, GetActorLocation());
}

FVector AC_ThrowingWeapon::GetPredictedThrowStartLocation()
{
	const FName PredictedThrowStartSocketName = (OwnerCharacter->GetPoseState() == EPoseState::CRAWL) ?
		"CrawlThrowablePredictedStartLocation" : "StandThrowablePredictedStartLocation"; // 예측 처리용 ThrowStart 위치 Socket
	return OwnerCharacter->GetMesh()->GetSocketLocation(PredictedThrowStartSocketName);
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
	// 새로운 자세에 맞는 Anim Montage로 다시 재생
	if (bIsCharging) OwnerCharacter->PlayAnimMontage(CurThrowProcessMontages.ThrowReadyMontage);
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

 void AC_ThrowingWeapon::TryUpdateAmmoWidgetMagazineTextIfNecessary(AC_Player* Player)
 {
	if (!Player) return;
	// 손에 쥐고 있는 무기가 ThrowingWeapon인지 체크
	if (AC_ThrowingWeapon* CurHandThrowingWeapon = Cast<AC_ThrowingWeapon>(Player->GetEquippedComponent()->GetCurWeapon()))
	{
		// 손에 쥐고 있는 무기가 같은 Throwable 종류이면, Ammo Magazine 개수 업데이트 시키기
		if (CurHandThrowingWeapon->GetThrowableType() == this->ThrowableType)
			UpdateAmmoWidgetMagazineText(Player);
	}
 }

 void AC_ThrowingWeapon::UpdateAmmoWidgetMagazineText(AC_Player* Player)
 {
	if (!Player) return;
	UC_Util::Print("UpdateAmmoWidgetMagazineText", GAMESCENE_MANAGER->GetTickRandomColor(), 10.f);

	AC_Item* InvenThrowable = Player->GetInvenComponent()->FindMyItemByName(ItemCode);
	int MagazineCount = !InvenThrowable ? 1 : InvenThrowable->GetItemCurStack() + 1;
	Player->GetHUDWidget()->GetAmmoWidget()->SetMagazineText(MagazineCount);
 }


