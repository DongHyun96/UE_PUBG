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

	//ItemType ����.
	ItemDatas.ItemType = EItemTypes::THROWABLE;

	ExplosionSphere = CreateDefaultSubobject<USphereComponent>("ExplosionSphere");
}

void AC_ThrowingWeapon::BeginPlay()
{
	Super::BeginPlay();

	ProjectileMovement->Deactivate();

	// Spawn actor�� ������ �������� OwnerCharacter�� Setting�ϴ����̶� ������ �ȸ���
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
	// ��ô���� �ɱ����� �̾Ұ� ��ŷ�� ������ �� �ٽ� �������
	// ��ô���� ���������̱��� �̾Ҵٸ� ���� ��ġ�� ���� ��ô�� �׳� �ٴڿ� ����

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
	//���⼭�ϴ� �뷮üũ�� �κ����� �̹� �ѹ� ó�� �Ǿ����� Ȥ�ø��� �־� ���������� Ȯ���� ������ ��.
	//�κ����� üũ���� �ʰ� �����ۿ��� üũ�ϴ� ������� ���� �ҵ�.
	
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

	//��ôĭ�� ����ִ��� �˻�
	if (equipComp->GetWeapons()[EWeaponSlot::THROWABLE_WEAPON])
	{
		//������ ��ô���� �����ϰ� �ִٸ� ����.
		uint8 nextVolume = 
			invenComp->GetCurVolume() 
			+ equipComp->GetWeapons()[EWeaponSlot::THROWABLE_WEAPON]->GetItemDatas().ItemVolume
			- ItemDatas.ItemVolume;

		if (nextVolume > invenComp->GetMaxVolume()) //��ü�� �κ��� �뷮�� �ʰ��Ѵٸ� ��ü �Ұ���, 
		{
			//�κ��� ������ üũ�ؼ� �κ�����
			if (invenComp->CheckVolume(this))
			{
				inItem = invenComp->FindMyItem(this);
				if (inItem)
				{
					inItem->AddItemStack();
					SetActorHiddenInGame(true);
					SetActorEnableCollision(false);
					this->Destroy();
					return; //�κ����� �������� �����Ƿ� ����.
				}
			}
		}
	

		//�ʰ����� �ʴ´ٸ� ��ô�� ��ü. unEquipItem = ��ü���� ������.
		unEquipItem = equipComp->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, this);

		//���������Ͷ� Ȯ�� �ѹ� �� ��. 
		inItem = invenComp->FindMyItem(unEquipItem);

		if (inItem)
		{
			//��ü���� �������� �κ��� �����Ѵٸ�. �κ��� �������� ������ + 1.
			inItem->AddItemStack();
			unEquipItem->SetActorHiddenInGame(true);
			unEquipItem->SetActorEnableCollision(false);
			unEquipItem->Destroy();
		}
		else
		{
			//��ü���� �������� �κ��� �������� �ʴ´ٸ�. �κ����� �������� �̵�.
			invenComp->AddItemToMyList(unEquipItem);

			SetActorHiddenInGame(true);
			SetActorEnableCollision(false);
		}
	}
	else
	{
		//������ ��ô���� �������� �ʴ´ٸ� ����. �ٷ� ����.
		equipComp->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, this);
		
		//AroundItemList���� �ش� �������� �Ȼ�����ٸ� �����ġ �� ��.
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
		return false; //�κ��� ���� �� �ִ� �������� ������ 0 �̸� ���� �� �����Ƿ� return false;
	}

	AC_Item* FoundItem = invenComp->FindMyItem(this); //�κ��� ���� �������� ã�ƿ�, ���ٸ� nullptr;

	if (ItemDatas.ItemCurStack == ItemStackCount)
	{
		//������ ���θ� �κ��� ���� �� ���� ��.
		if (IsValid(FoundItem))
		{
			//�κ��� �ش� �������� ���� �� ��.
			FoundItem->SetItemStack(FoundItem->GetItemDatas().ItemCurStack + ItemStackCount);
			//invenComp->GetCurVolume() += FoundItem->GetItemDatas().ItemVolume * ItemStackCount;
			//TODO : destroy�� �ص� �ܻ��� ���°��� ����ؼ� �س��� ���� ���̵� �ܻ��� �ȳ��´ٸ� ���� ��.
			invenComp->AddInvenCurVolume(this->ItemDatas.ItemVolume * ItemStackCount);

			this->SetActorEnableCollision(false);
			this->SetActorHiddenInGame(true);

			this->Destroy();
			return true;
		}
		else
		{
			//�κ��� �ش� �������� �������� ���� ��.
			invenComp->AddItemToMyList(this);
			//�ΰ��ӿ��� ���̴� �Ͱ� collision���� ������ �ӽ÷� ����.
			//this->SetActorEnableCollision(false);
			this->SetActorHiddenInGame(true);
			//���� �� ����. �̰ɷ� ���� �������� �������� �ȳ����ٸ� �ٸ� ��� ���->ToInven���� SetActorEnableCollision�� ���ְ� ������ Ȥ�� ToAround���� ���ֱ�.
			Collider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			return true;
		}
	}
	else
	{
		//�������� �Ϻθ� �κ��� ���� �� ���� ��.
		if (IsValid(FoundItem))
		{
			     this->SetItemStack(ItemDatas.ItemCurStack - ItemStackCount);
			FoundItem->SetItemStack(FoundItem->GetItemDatas().ItemCurStack + ItemStackCount);

			invenComp->AddInvenCurVolume(this->ItemDatas.ItemVolume * ItemStackCount);

			return true;
		}
		else
		{
			AC_Weapon* NewItem = Cast<AC_Weapon>(SpawnItem(Character));//������ ���� ����
			NewItem->SetItemStack(ItemStackCount);
			   this->SetItemStack(ItemDatas.ItemCurStack - ItemStackCount);

			invenComp->AddItemToMyList(NewItem);

			NewItem->SetActorHiddenInGame(true);
			//collider ���� ���� �߰��ؾ� �� �� ����.
			//���� �߰��ؾ� �ȴٸ� MoveToInven���� SetActorEnableCollision�� ���ְ� ���� �� ���ִ� �������.
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
	//TODO: �����ؼ� ������ ��� ���� �������־����.
	ItemDatas.ItemPlace = EItemPlace::AROUND;
	SetOwnerCharacter(nullptr);
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	Collider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	
	//�ٴ� ���� ĳ���� �޾ƿͼ� �ٴڿ� ������ �����ϱ�.
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
		//int nextVolume = invenComp->GetCurVolume() - ItemDatas.ItemVolume + curWeapaon->GetItemDatas().ItemVolume; //�̰� �κ��� �����ϴ� �������� �ű涧�� ��ȿ �Ѱ� ������?
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
				if (this->ItemDatas.ItemPlace == EItemPlace::INVEN)       invenComp->RemoveItemToMyList(this);//�Ƹ� InvenUI�� �ʱ�ȭ �����ִ� �۾��� �߰������� �ʿ��Ұ�.
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

			FoundItem = invenComp->FindMyItem(OutToSlotWeapon); //�κ��� �ش� �������� �����ϴ��� Ȯ��

			if (IsValid(FoundItem))
			{
				//FoundItem->SetItemStack(FoundItem->GetItemDatas().ItemStack + 1);
				FoundItem->AddItemStack();
				//TODO:�ܻ��� ����ٸ� �߰� �۾� �ʿ�.
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
		//���Կ� �������� ���ٸ� ����.
		if (ItemDatas.ItemCurStack == 1)
		{
			//this�� stack�� 1 �̶�� ����.
			//this�� ���Կ� ���� �ϰ� ��Ͽ��� ����

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
			//this�� ������ 1�� �ƴ϶�� ����
			//this�� ���� �����ؼ� �ϳ��� �����ϰ� �������� ������ ����
			//AC_Weapon* NewWeapon = DuplicateObject(this, Character);
			AC_Weapon* NewWeapon = Cast<AC_Weapon>(SpawnItem(Character));
			NewWeapon->SetItemStack(1);
			     this->SetItemStack(ItemDatas.ItemCurStack - 1);
			
			equipComp->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, NewWeapon); //���Կ� ����. �Ʒ��� NewWeapon�� �ٸ�.

			if (ItemDatas.ItemPlace == EItemPlace::AROUND) return this->MoveToInven(Character);//�ϳ��� ���� ������Ų �������� MoveToInven���� �κ��� �ִ� �۾� ����.
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
	//location, rotation�� this�� ���� ���� �͵� ����, �ֳ��ϸ� ���� �̻��ϰ� ���󰡴� ������ �̰��� �� �� ����. -> ����ź�� ������ �浹ü�� ��������.
	AC_Weapon* SpawnItem = GetWorld()->SpawnActor<AC_Weapon>(this->GetClass(), Character->GetActorLocation() - FVector(0,0,50), Character->GetActorRotation(), SpawnParams);
	//SpawnItem->SetItemStack(1);
	//SpawnItem->SetActorHiddenInGame(true);
	SpawnItem->SetActorEnableCollision(false);//������ �� ������ OverlapBegine�� �����ؼ� �켱 ������.
	return SpawnItem;
}

void AC_ThrowingWeapon::InitTestPool(AC_BasicCharacter* InOwnerCharacter, UClass* Class, UC_EquippedComponent* EquippedComponent)
{
	ThrowablePool.Empty();

	// �賶�� �ִ� ��ó�� ����
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
	// Player HUD Opened ���� ����ó��
	if (AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharacter))
	{
		// UI â�� ������ ��
		if (OwnerPlayer->GetInvenSystem()->GetIsPanelOpend() ||
			OwnerPlayer->GetHUDWidget()->GetMainMapWidget()->GetIsPanelOpened())
		{
			ClearSpline();

			if (bIsCooked) // cooking �Ǿ��ٸ� ��������
			{
				OwnerCharacter->PlayAnimMontage(CurThrowProcessMontages.ThrowMontage);
				return;
			}

			// Cooking �Ǿ����� �ʴٸ� �ٽ� Idle �ڼ��� ���ƿ�
			OwnerPlayer->GetMesh()->GetAnimInstance()->Montage_Stop(0.2f, CurThrowProcessMontages.ThrowReadyMontage.AnimMontage);
			bIsOnThrowProcess	= false;
			bIsCharging			= false;
			return;
		}
	}

	if (bIsCharging)
	{
		// Charging �� ó��
		HandlePredictedPath();
		return;
	}

	// Charging�� Ǯ���� �� ���� ������ ���� ����
	ClearSpline();
	OwnerCharacter->PlayAnimMontage(CurThrowProcessMontages.ThrowMontage);
}

void AC_ThrowingWeapon::OnThrowThrowable()
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	//OwnerCharacter->GetEquippedComponent()->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, nullptr);
	// Direction ���ϴ� ��� 1
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

	SetActorEnableCollision(true);//���� �� �� ���� �浹 ���ֱ�.

	// Ready ���� ProcessEnd�� �� �� �ֱ� ������ Predicted Path spline ��� �����ֱ�
	ClearSpline();

	// ���� Throw AnimMontage ���߱� (�켱���� ������ ����� ��)
	//OwnerCharacter->GetMesh()->GetAnimInstance()->Montage_Stop(0.2f, CurThrowProcessMontages.ThrowMontage.AnimMontage);

	UAnimInstance* OwnerAnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();

	//OwnerAnimInstance->StopAllMontages(0.2f);

	// �߰��� �ڼ��� �ٲ� �� �ֱ� ������ ��� �ڼ��� AnimMontage playing üũ
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

	// ���� ��ô�� ���� ���� �ٷ� �ϱ� -> �̹� ���� ���⿡ ���� PoseTransitionEnd Delegate ���� �̷����
	AC_BasicCharacter* PrevOwnerCharacter = OwnerCharacter;

	OwnerEquippedComponent->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, nullptr);

	// ������ ������ ��ô�� �ִ��� ����
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

		// �ٷ� ���� ��ô�� ������
		OwnerEquippedComponent->SetNextWeaponType(EWeaponSlot::THROWABLE_WEAPON);
		PrevOwnerCharacter->PlayAnimMontage(TargetThrowWeapon->GetCurDrawMontage());
		return;
	}

	// �ٸ� ��ô�� ����
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

		// �ٷ� ���� ��ô�� ������
		OwnerEquippedComponent->SetNextWeaponType(EWeaponSlot::THROWABLE_WEAPON);
		PrevOwnerCharacter->PlayAnimMontage(TargetThrowWeapon->GetCurDrawMontage());
		return;
	}

	//���� �ִ� ��ô���� ���ٸ� �ֹ���1, �ֹ���2 ������ ���� �õ�, ���ٸ� UnArmed�� ���·� ���ư���
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

	// TODO : Ready ���¿��� Cooking �����ϸ� ���� �ð� HUD ����
	//UC_Util::Print("Throwable Starts cooking");

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AC_ThrowingWeapon::Explode, CookingTime, false);
}

bool AC_ThrowingWeapon::ReleaseOnGround()
{
	
	if (!bIsCooked) return false;						// Cooking�� �ȵǾ��ٸ� (���������̰� ���ư��� �ʾҴٸ�)
	if (ProjectileMovement->IsActive()) return false;	// �̹� �������ٸ�

	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	Collider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	ProjectileMovement->Velocity = FVector(0.f, 0.f, 0.f); // 0.f�� �ӷ����� ������
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

	if (GetAttachParentActor()) ReleaseOnGround(); // �տ��� ���� ������ �ʾ��� ��
		
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

	// �ڼ��� �´� Montage�� ��������� �ʴٸ�, �ش� Montage�� ���� �ڿ� ���߱�
	static EPoseState PredictedPoseState = EPoseState::POSE_MAX;

	UAnimInstance* MeshAnimInstance = OwnerMeshTemp->GetAnimInstance();

	// ���� posing�� �ٸ��ٸ� ���� ��� �������� �޶���
	
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
	// �÷��̾��� ��쿡�� �׸��� (����, GameManager ��������� Player�� ��ü �����غ� ��)
	AC_Player* Player = Cast<AC_Player>(OwnerCharacter);
	if (!IsValid(Player)) return;

	// ���� OwnerCharacter�� �տ� ������ ��Ȳ���� Ȯ��
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
	// TODO : Enemy AI�� ��� ����ź ������ ������ �ٸ� ������� ������� ��

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
		// ���ο� �ڼ��� �´� Anim Montage�� �ٽ� ���
		OwnerCharacter->PlayAnimMontage(CurThrowProcessMontages.ThrowReadyMontage);
		return;
	}


	//CurDrawMontage = DrawMontages[OwnerCharacter->GetPoseState()];

	//// ThrowProcessEnd ���� OnDrawEnd�� ����� ȣ���� �ȵǾ��� ���
	//// ���ǽ��� �Ȱɸ� ���� ����
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
