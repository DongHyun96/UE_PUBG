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

#include "I_ExplodeStrategy.h"
#include "C_GrenadeExplode.h"
#include "C_FlashBangExplode.h"
#include "C_SmokeGrndExplode.h"

#include "Utility/C_Util.h"


TArray<AC_ThrowingWeapon*> AC_ThrowingWeapon::ThrowablePool{};

USkeletalMeshComponent* AC_ThrowingWeapon::OwnerMeshTemp{};

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
	ItemDatas.ItemType = EItemTypes::THROWABLE;

	ExplosionSphere = CreateDefaultSubobject<USphereComponent>("ExplosionSphere");
}

void AC_ThrowingWeapon::BeginPlay()
{
	Super::BeginPlay();

	ProjectileMovement->Deactivate();

	// Spawn actor를 실행한 다음에야 OwnerCharacter를 Setting하는중이라 시점이 안맞음
	//if (OwnerCharacter)
	//	OwnerCharacter->Delegate_OnPoseTransitionFin.AddUObject(this, &AC_ThrowingWeapon::OnOwnerCharacterPoseTransitionFin);
}

void AC_ThrowingWeapon::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (EndPlayReason == EEndPlayReason::Destroyed) return;

	if (OwnerMeshTemp)
	{
		OwnerMeshTemp->DestroyComponent();
		OwnerMeshTemp = nullptr;
	}

	for (auto& item : ThrowablePool)
		item->Destroy();

	ThrowablePool.Empty();
}

void AC_ThrowingWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
			
	if (!IsValid(OwnerCharacter))
		return;

	CurDrawMontage			= DrawMontages[OwnerCharacter->GetPoseState()];
	CurSheathMontage		= SheathMontages[OwnerCharacter->GetPoseState()];
	CurThrowProcessMontages = ThrowProcessMontages[OwnerCharacter->GetPoseState()];
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

	ProjectileMovement->Deactivate();
	ClearSpline();

	OwnerCharacter->SetHandState(EHandState::WEAPON_THROWABLE);
	
	// Self init
	bIsCharging = false;
	bIsOnThrowProcess = false;

	UC_Util::Print("Left Grenade Count : " + FString::FromInt(ThrowablePool.Num() + 1));

	return AttachToComponent
	(
		InParent,
		FAttachmentTransformRules(EAttachmentRule::KeepRelative, true),
		EQUIPPED_SOCKET_NAME
	);
}

void AC_ThrowingWeapon::PickUpItem(AC_BasicCharacter* Character)
{
	//여기서하는 용량체크는 인벤에서 이미 한번 처리 되었지만 혹시몰라 넣어 놓은것으로 확인후 제거할 것.
	//인벤에서 체크하지 않고 아이템에서 체크하는 방식으로 가야 할듯.
	
	//EquipToCharacter(Character);
	MoveToSlot(Character);
}

void AC_ThrowingWeapon::DropItem(AC_BasicCharacter* Character)
{
}

void AC_ThrowingWeapon::SetItemStack(uint8 ItemStack)
{
	ItemDatas.ItemCurStack = ItemStack;
}

void AC_ThrowingWeapon::EquipToCharacter(AC_BasicCharacter* Character)
{
	UC_InvenComponent* invenComp = Character->GetInvenComponent();

	UC_EquippedComponent* equipComp = Character->GetEquippedComponent();

	AC_Item* unEquipItem = nullptr;

	AC_Item* inItem = nullptr;

	//투척칸이 비어있는지 검사
	if (equipComp->GetWeapons()[EWeaponSlot::THROWABLE_WEAPON])
	{
		//장착된 투척류가 존재하고 있다면 실행.
		uint8 nextVolume = 
			invenComp->GetCurVolume() 
			+ equipComp->GetWeapons()[EWeaponSlot::THROWABLE_WEAPON]->GetItemDatas().ItemVolume
			- ItemDatas.ItemVolume;

		if (nextVolume > invenComp->GetMaxVolume()) //교체후 인벤의 용량을 초과한다면 교체 불가능, 
		{
			//인벤의 공간을 체크해서 인벤으로
			if (invenComp->CheckVolume(this))
			{
				inItem = invenComp->FindMyItem(this);
				if (inItem)
				{
					inItem->AddItemStack();
					SetActorHiddenInGame(true);
					SetActorEnableCollision(false);
					this->Destroy();
					return; //인벤으로 아이템이 들어갔으므로 종료.
				}
			}
		}
	

		//초과하지 않는다면 투척류 교체. unEquipItem = 교체당한 아이템.
		unEquipItem = equipComp->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, this);

		//더블포인터라 확인 한번 할 것. 
		inItem = invenComp->FindMyItem(unEquipItem);

		if (inItem)
		{
			//교체당한 아이템이 인벤에 존재한다면. 인벤의 아이템의 스택을 + 1.
			inItem->AddItemStack();
			unEquipItem->SetActorHiddenInGame(true);
			unEquipItem->SetActorEnableCollision(false);
			unEquipItem->Destroy();
		}
		else
		{
			//교체당한 아이템이 인벤에 존재하지 않는다면. 인벤으로 아이템을 이동.
			invenComp->AddItemToMyList(unEquipItem);

			SetActorHiddenInGame(true);
			SetActorEnableCollision(false);
		}
	}
	else
	{
		//장착된 투척류가 존재하지 않는다면 실행. 바로 장착.
		equipComp->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, this);
		
		//AroundItemList에서 해당 아이템이 안사라진다면 긴급조치 할 것.
		//invenComp->RemoveAroundItem(this);
		
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
	}
}

bool AC_ThrowingWeapon::MoveToInven(AC_BasicCharacter* Character)
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

	if (ItemDatas.ItemCurStack == ItemStackCount)
	{
		//아이템 전부를 인벤에 넣을 수 있을 때.
		if (IsValid(FoundItem))
		{
			//인벤에 해당 아이템이 존재 할 때.
			FoundItem->SetItemStack(FoundItem->GetItemDatas().ItemCurStack + ItemStackCount);
			//invenComp->GetCurVolume() += FoundItem->GetItemDatas().ItemVolume * ItemStackCount;
			//TODO : destroy를 해도 잔상이 남는것을 대비해서 해놓음 만약 없이도 잔상이 안남는다면 지울 것.
			invenComp->AddInvenCurVolume(this->ItemDatas.ItemVolume * ItemStackCount);

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
			     this->SetItemStack(ItemDatas.ItemCurStack - ItemStackCount);
			FoundItem->SetItemStack(FoundItem->GetItemDatas().ItemCurStack + ItemStackCount);

			invenComp->AddInvenCurVolume(this->ItemDatas.ItemVolume * ItemStackCount);

			return true;
		}
		else
		{
			AC_Weapon* NewItem = Cast<AC_Weapon>(SpawnItem(Character));//아이템 복제 생성
			NewItem->SetItemStack(ItemStackCount);
			   this->SetItemStack(ItemDatas.ItemCurStack - ItemStackCount);

			invenComp->AddItemToMyList(NewItem);

			NewItem->SetActorHiddenInGame(true);
			//collider 관련 설정 추가해야 할 수 있음.
			//만약 추가해야 된다면 MoveToInven에서 SetActorEnableCollision을 꺼주고 던질 때 켜주는 방식으로.
			return true;
		}
	}
}

bool AC_ThrowingWeapon::MoveToAround(AC_BasicCharacter* Character)
{
	if (!Character) return false;
	if (this->GetItemDatas().ItemPlace == EItemPlace::INVEN)
		Character->GetInvenComponent()->RemoveItemToMyList(this);
	else if (this->GetItemDatas().ItemPlace == EItemPlace::SLOT)
	{
		Character->GetEquippedComponent()->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, nullptr);
		//this->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
	}
	
	//DetachmentItem();
	//TODO: 분할해서 버리는 경우 새로 스폰해주어야함.
	ItemDatas.ItemPlace = EItemPlace::AROUND;
	SetOwnerCharacter(nullptr);
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	Collider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	
	//바닥 레이 캐스팅 받아와서 바닥에 아이템 생성하기.
	SetActorLocation(GetGroundLocation(Character) + RootComponent->Bounds.BoxExtent.Z);
	
	//SetActorRotation(FQuat(0,0,0));

	return true;
}

bool AC_ThrowingWeapon::MoveToSlot(AC_BasicCharacter* Character)
{
	UC_EquippedComponent* equipComp = Character->GetEquippedComponent();
	UC_InvenComponent* invenComp = Character->GetInvenComponent();

	AC_Item* unEquipItem = nullptr;

	AC_Item* inItem = nullptr;

	AC_Weapon* curWeapaon = equipComp->GetWeapons()[EWeaponSlot::THROWABLE_WEAPON];


	if (IsValid(curWeapaon))
	{
		AC_Weapon* OutToSlotWeapon = nullptr;
		AC_Item* FoundItem = nullptr;
		//int nextVolume = invenComp->GetCurVolume() - ItemDatas.ItemVolume + curWeapaon->GetItemDatas().ItemVolume; //이건 인벤에 존재하는 아이템을 옮길때만 유효 한거 같은데?
		int nextVolume = 0;

		if (this->ItemDatas.ItemPlace == EItemPlace::INVEN)
			nextVolume = invenComp->GetCurVolume() - ItemDatas.ItemVolume + curWeapaon->GetItemDatas().ItemVolume;
		else
			nextVolume = invenComp->GetCurVolume() + curWeapaon->GetItemDatas().ItemVolume;

		if (nextVolume > invenComp->GetMaxVolume())
		{
			UC_Util::Print("Not Enough Volume");
			return false;
		}
		else
		{
			//SetActorEnableCollision(false);
			if (ItemDatas.ItemCurStack == 1)
			{
				if (this->ItemDatas.ItemPlace == EItemPlace::INVEN)       invenComp->RemoveItemToMyList(this);//아마 InvenUI를 초기화 시켜주는 작업이 추가적으로 필요할것.
				else if (this->ItemDatas.ItemPlace == EItemPlace::AROUND) invenComp->RemoveItemToAroundList(this);

				OutToSlotWeapon = equipComp->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, this);
			}
			else
			{
				AC_Weapon* InToSlotWeapon = Cast<AC_Weapon>(SpawnItem(Character));
				InToSlotWeapon->SetItemStack(1);
				this->SetItemStack(ItemDatas.ItemCurStack - 1);

				OutToSlotWeapon = equipComp->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, InToSlotWeapon);
			}

			//if (OutToSlotWeapon->MoveToInven(Character)) return true;

			FoundItem = invenComp->FindMyItem(OutToSlotWeapon); //인벤에 해당 아이템이 존재하는지 확인

			if (IsValid(FoundItem))
			{
				//FoundItem->SetItemStack(FoundItem->GetItemDatas().ItemStack + 1);
				FoundItem->AddItemStack();
				//TODO:잔상이 생긴다면 추가 작업 필요.
				OutToSlotWeapon->Destroy();
				return true;
			}
			else
			{
				invenComp->AddItemToMyList(OutToSlotWeapon);
				return true;
			}
		}
	}
	else
	{
		//슬롯에 아이템이 없다면 실행.
		if (ItemDatas.ItemCurStack == 1)
		{
			//this의 stack이 1 이라면 실행.
			//this를 슬롯에 장착 하고 목록에서 제거

			if (ItemDatas.ItemPlace == EItemPlace::AROUND)
			{
				invenComp->RemoveItemNearList(this);
				//invenComp->RemoveItemToAroundList(this);

				//return true;
			}
			else if (ItemDatas.ItemPlace == EItemPlace::INVEN)
			{
				invenComp->RemoveItemToMyList(this);
				//return true;
			}
			equipComp->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, this);
			return true;
		}
		else
		{
			//this의 스택이 1이 아니라면 실행
			//this를 복제 생성해서 하나는 장착하고 나머지는 갯수를 수정
			//AC_Weapon* NewWeapon = DuplicateObject(this, Character);
			AC_Weapon* NewWeapon = Cast<AC_Weapon>(SpawnItem(Character));
			NewWeapon->SetItemStack(1);
			     this->SetItemStack(ItemDatas.ItemCurStack - 1);
			
			equipComp->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, NewWeapon); //슬롯에 장착. 아래의 NewWeapon과 다름.

			if (ItemDatas.ItemPlace == EItemPlace::AROUND) return this->MoveToInven(Character);//하나를 뺴서 장착시킨 아이템을 MoveToInven으로 인벤에 넣는 작업 실행.
			else if (ItemDatas.ItemPlace == EItemPlace::INVEN) return true;
			else return false;
		}
	}

	//return false;
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

	switch (ItemDatas.ItemPlace)
	{
	case EItemPlace::SLOT:

	case EItemPlace::AROUND:
		if (curWeapaon) return MoveToInven(Character);
	case EItemPlace::INVEN:
		return MoveToSlot(Character);
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
	AC_Weapon* SpawnItem = GetWorld()->SpawnActor<AC_Weapon>(this->GetClass(), Character->GetActorLocation() - FVector(0,0,50), Character->GetActorRotation(), SpawnParams);
	//SpawnItem->SetItemStack(1);
	//SpawnItem->SetActorHiddenInGame(true);
	SpawnItem->SetActorEnableCollision(false);//생성될 때 무조건 OverlapBegine에 반응해서 우선 꺼뒀음.
	return SpawnItem;
}

void AC_ThrowingWeapon::InitTestPool(AC_BasicCharacter* InOwnerCharacter, UClass* Class, UC_EquippedComponent* EquippedComponent)
{
	ThrowablePool.Empty();

	// 배낭에 있는 것처럼 세팅
	for (int i = 0; i < TESTPOOLCNT; i++)
	{
		FActorSpawnParameters Param{};
		Param.Owner = InOwnerCharacter;

		AC_ThrowingWeapon* ThrowWeapon = EquippedComponent->GetWorld()->SpawnActor<AC_ThrowingWeapon>(Class, Param);

		if (ThrowWeapon)
		{
			ThrowWeapon->SetOwnerCharacter(InOwnerCharacter);
			//ThrowWeapon->OwnerCharacter->Delegate_OnPoseTransitionFin.AddUObject(ThrowWeapon, &AC_ThrowingWeapon::OnOwnerCharacterPoseTransitionFin);
			ThrowablePool.Add(ThrowWeapon);
		}
	}
}

void AC_ThrowingWeapon::OnRemovePinFin()
{
	OwnerCharacter->PlayAnimMontage(CurThrowProcessMontages.ThrowReadyMontage);
}

void AC_ThrowingWeapon::OnThrowReadyLoop()
{
	// Player HUD Opened 관련 예외처리
	if (AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharacter))
	{
		// UI 창이 켜졌을 때
		if (OwnerPlayer->GetInvenSystem()->GetIsPanelOpend() ||
			OwnerPlayer->GetHUDWidget()->GetMainMapWidget()->GetIsPanelOpened())
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
		// Charging 중 처리
		HandlePredictedPath();
		return;
	}

	// Charging이 풀렸을 때 다음 던지기 동작 실행
	ClearSpline();
	OwnerCharacter->PlayAnimMontage(CurThrowProcessMontages.ThrowMontage);
}

void AC_ThrowingWeapon::OnThrowThrowable()
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	//OwnerCharacter->GetEquippedComponent()->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, nullptr);
	// Direction 구하는 방법 1
	/*FVector ActorForward = FRotationMatrix(OwnerCharacter->GetActorRotation()).GetUnitAxis(EAxis::X);

	UC_AnimBasicCharacter* OwnerAnim = Cast<UC_AnimBasicCharacter>(OwnerCharacter->GetMesh()->GetAnimInstance());
	
	if (!IsValid(OwnerAnim))
	{
		FString DebugMessage = "AC_ThrowingWeapon::OnThrowThrowable : OwnerAnim not valid!";
		GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, *DebugMessage);

		Direction = ActorForward;
	}
	else
	{
		FRotator SpineRotation	  = OwnerAnim->GetCSpineRotation();
		FVector SpineForward	  = FRotationMatrix(SpineRotation).GetUnitAxis(EAxis::X);
		FVector CombinedDirection = FRotationMatrix(OwnerCharacter->GetActorRotation()).TransformVector(SpineForward);
		Direction = CombinedDirection;
	}*/

	UpdateProjectileLaunchValues();

	//UC_Util::Print(ProjStartLocation);

	Collider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	//ECollisionResponse Response = Collider->GetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic);

	//if (Response == ECollisionResponse::ECR_Block)
	//	UC_Util::Print("Response Block", FColor::Red, 10.f);


	this->SetActorLocation(ProjStartLocation);
	
	ProjectileMovement->Velocity = ProjLaunchVelocity;
	ProjectileMovement->Activate();

	if (!bIsCooked) StartCooking();
}

void AC_ThrowingWeapon::OnThrowProcessEnd()
{
	//UC_Util::Print("OnThrowProcessEnd", FColor::Cyan, 5.f);

	bIsOnThrowProcess = false;

	bIsCharging = false;

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
		TargetThrowWeapon->MoveToSlot(PrevOwnerCharacter);

		// 바로 다음 투척류 꺼내기
		OwnerEquippedComponent->SetNextWeaponType(EWeaponSlot::THROWABLE_WEAPON);
		PrevOwnerCharacter->PlayAnimMontage(TargetThrowWeapon->GetCurDrawMontage());
		return;
	}

	// 다른 투척류 조사
	for (auto& Pair : THROWABLETYPE_ITEMNAME_MAP)
	{
		if (Pair.Key == this->ThrowableType) continue;

		ThrowWeapon = OwnerCharacter->GetInvenComponent()->FindMyItem(Pair.Value);

		UC_Util::Print("1");

		if (!ThrowWeapon) continue;

		UC_Util::Print("2");

		AC_ThrowingWeapon* TargetThrowWeapon = Cast<AC_ThrowingWeapon>(ThrowWeapon);

		if (!TargetThrowWeapon)
		{
			UC_Util::Print("OnProcessEnd Change to Last New Throwing Weapon casting failed!", FColor::Red, 10.f);
			return;
		}

		TargetThrowWeapon->MoveToSlot(PrevOwnerCharacter);

		// 바로 다음 투척류 꺼내기
		OwnerEquippedComponent->SetNextWeaponType(EWeaponSlot::THROWABLE_WEAPON);
		PrevOwnerCharacter->PlayAnimMontage(TargetThrowWeapon->GetCurDrawMontage());
		return;
	}

	//남아 있는 투척류가 없다면 주무기1, 주무기2 순으로 장착 시도, 없다면 UnArmed인 상태로 돌아가기
	if (OwnerEquippedComponent->ChangeCurWeapon(EWeaponSlot::MAIN_GUN))		return;
	if (OwnerEquippedComponent->ChangeCurWeapon(EWeaponSlot::SUB_GUN))		return;

	OwnerEquippedComponent->ChangeCurWeapon(EWeaponSlot::NONE);

}

void AC_ThrowingWeapon::InitExplodeStrategy(EThrowableType InThrowableType)
{
	this->ThrowableType = InThrowableType;

	switch (ThrowableType)
	{
	case EThrowableType::GRENADE:
		ExplodeStrategy = NewObject<AC_GrenadeExplode>(this);
		return;
	case EThrowableType::FLASH_BANG:
		ExplodeStrategy = NewObject<AC_FlashBangExplode>(this);
		return;
	case EThrowableType::SMOKE:
		ExplodeStrategy = NewObject<AC_SmokeGrndExplode>(this);
		return;
	default:
		break;
	}
}

void AC_ThrowingWeapon::StartCooking()
{
	bIsCooked = true;

	// TODO : Ready 상태에서 Cooking 시작하면 남은 시간 HUD 띄우기
	//UC_Util::Print("Throwable Starts cooking");

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AC_ThrowingWeapon::Explode, CookingTime, false);
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
		UC_Util::Print("From AC_ThrowingWeapon::Explode : Explode Strategy nullptr!");
		return;
	}

	bool Exploded = ExplodeStrategy->UseStrategy(this);

	if (GetAttachParentActor()) ReleaseOnGround(); // 손에서 아직 떠나지 않았을 때
		
	//if (Exploded) this->Destroy();
	if (Exploded)
	{
		this->SetActorHiddenInGame(true);
		
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
			{
				this->Destroy();
			}, 10.f, false);
	}
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
	ProjectilePathParams.ProjectileRadius		= 5.f;
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

void AC_ThrowingWeapon::HandlePredictedPath()
{
	// 플레이어일 경우에만 그리기 (추후, GameManager 멤버변수의 Player와 객체 대조해볼 것)
	AC_Player* Player = Cast<AC_Player>(OwnerCharacter);
	if (!IsValid(Player)) return;

	// 현재 OwnerCharacter의 손에 장착된 상황인지 확인
	if (!IsValid(this->GetAttachParentActor()) || this->GetAttachParentSocketName() != EQUIPPED_SOCKET_NAME) return;

	if (!OwnerCharacter->GetMesh()->GetAnimInstance()->Montage_IsPlaying(CurThrowProcessMontages.ThrowReadyMontage.AnimMontage))
		return;

	UpdateProjectileLaunchValues();
	DrawPredictedPath();
	//DrawDebugPredictedPath();
}

void AC_ThrowingWeapon::UpdateProjectileLaunchValues()
{
	// TODO : Init ProjStartLocation & ProjLaunchVelocity
	// TODO : Enemy AI의 경우 수류탄 던지는 방향을 다른 방법으로 정해줘야 함

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


	//CurDrawMontage = DrawMontages[OwnerCharacter->GetPoseState()];

	//// ThrowProcessEnd 이후 OnDrawEnd가 제대로 호출이 안되었을 경우
	//// 조건식이 안걸릴 때가 있음
	//if (
	//	OwnerCharacter->GetHandState() == EHandState::WEAPON_THROWABLE &&
	//	OwnerCharacter->GetEquippedComponent()->GetNextWeaponType() != EWeaponSlot::NONE //&&
	//	//!OwnerCharacter->GetMesh()->GetAnimInstance()->Montage_IsPlaying(CurDrawMontage.AnimMontage)
	//	)
	//{
	//	UC_Util::Print("Call OnDrawEnd Manually", FColor::Cyan, 5.f);
	//	OwnerCharacter->GetEquippedComponent()->OnDrawEnd();
	//}

}

void AC_ThrowingWeapon::ClearSpline()
{
	PredictedEndPoint->SetVisibility(false);

	for (auto& item : SplineMeshes) item->DestroyComponent();
	SplineMeshes.Empty();

	PathSpline->ClearSplinePoints();
}
