// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ConsumableItem/C_ConsumableItem.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "Character/C_Player.h"
#include "HUD/C_HUDWidget.h"
#include "Character/Component/C_EquippedComponent.h"
#include "Character/Component/C_InvenComponent.h"
#include "Character/Component/C_ConsumableUsageMeshComponent.h"
#include "Character/Component/C_SwimmingComponent.h"
#include "Character/Component/C_InvenSystem.h"
#include "Character/Component/C_InputComponent.h"
#include "Item/Weapon/C_Weapon.h"
#include "Item/Weapon/Gun/C_Gun.h"
//#include "Item/ConsumableItem/C_ConsumableItem.h"
#include "InvenUI/ItemBar/C_BasicItemBarWidget.h"
//#include "InvenUserInterface/C_ItemBarWidget.h"

#include "Character/C_Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Utility/C_Util.h"

#include "HUD/C_InformWidget.h"

#include "Singleton/C_GameInstance.h"
const TMap<EConsumableItemType, FName> AC_ConsumableItem::ConsumableItemNameMap =
{
	{EConsumableItemType::MEDKIT,			"Item_Heal_MedKit_C"},
	{EConsumableItemType::FIRST_AID_KIT,	"Item_Heal_FirstAid_C"},
	{EConsumableItemType::BANDAGE,			"Item_Heal_Bandage_C"},
	{EConsumableItemType::PAIN_KILLER,		"Item_Boost_PainKiller_C"},
	{EConsumableItemType::ENERGY_DRINK,		"Item_Boost_EnergyDrink_C"},
};

FTutorialStageGoalCheckerDelegate AC_ConsumableItem::HealingTutorialDelegate{};

AC_ConsumableItem::AC_ConsumableItem()
{
	PrimaryActorTick.bCanEverTick = true;

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));

	AudioComponent->bAutoActivate = false; //자동 실행 방지
}

void AC_ConsumableItem::BeginPlay()
{
	Super::BeginPlay();


}

void AC_ConsumableItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UsingTimer += DeltaTime;

	/*if (!LinkedItemBarWidget)
		UC_Util::Print("LinkedItemBarWidget is Nullptr");*/

	switch (ConsumableItemState)
	{
	case EConsumableItemState::IDLE:
		UsingTimer = 0.f;
		return;
	case EConsumableItemState::ACTIVATING:
	{
		AC_Player* Player = Cast<AC_Player>(ItemUser);
		
		if (UsingTimer < UsageTime)
		{

			HandleActivatingState(); // Pure virtual primitive operation (자식 단계에서 세부적인 처리가 나뉘어지는 단계)

			//방해 받았는지 체크해서 방해를 받았다면 Activating Cancel 시키기

			if (ItemUser->GetCharacterMovement()->IsFalling() || ItemUser->GetSwimmingComponent()->IsSwimming())
			{
				if (Player) Player->GetHUDWidget()->GetInformWidget()->AddPlayerWarningLog("ITEM USE INTERRUPTED");
				CancelActivating();
				return;
			}

			UAnimInstance* UserAnimInstance = ItemUser->GetMesh()->GetAnimInstance();

			for (auto& Pair : UsingMontageMap)
			{
				if (UserAnimInstance->Montage_IsPlaying(Pair.Value.AnimMontage)) // 방해 받지 않았을 때 (즉, 활성화 Animation이 진행 중일 때)
				{
					if (Player && LinkedItemBarWidget)
					{
						//UseProgress = UsingTimer / UsageTime; 
						LinkedItemBarWidget->SetPercent(UsingTimer, UsageTime);
					}
					return;
				}
			}

			// 방해를 받음
			if (Player) Player->GetHUDWidget()->GetInformWidget()->AddPlayerWarningLog("ITEM USE INTERRUPTED");
			CancelActivating();
			return;
		}

		// Activating end

		// ItemUser가 Stat care test Enemy가 아닌 경우, ItemCurStack을 정상적으로 하나 줄임
		AC_Enemy* Enemy = Cast<AC_Enemy>(ItemUser);
		if (!Enemy || Enemy->GetBehaviorType() != EEnemyBehaviorType::StatCareTest)
			ItemCurStack--;
		
		UC_Util::Print("Item Used: ItemStack -> " + FString::FromInt(ItemCurStack), FColor::MakeRandomColor(), 20.f);

		if (Player) Player->GetHUDWidget()->OnConsumableItemActivatingEnd();

		ConsumableItemState = EConsumableItemState::ACTIVATE_COMPLETED;

		UsingTimer = 0.f;

		// 착용 중인 무기가 있었을 때 해당 무기 다시 장착 시도 (없다면 그냥 넘어가는 처리로 됨)
		ItemUser->GetEquippedComponent()->TryReAttachCurWeaponToHand();

		OnActivatingFinish(); // Pure-virtual Primitive Operation

		ItemUser->SetIsActivatingConsumableItem(false, nullptr);

	}
		return;
	case EConsumableItemState::ACTIVATE_COMPLETED:
		HandleActivateCompletedState(); // Pure-virtual Primitive Operation
		return;
	case EConsumableItemState::USED:
	{
		ConsumableItemState = EConsumableItemState::IDLE;
		
		if (AC_Player* Player = Cast<AC_Player>(ItemUser))
		{
			if (LinkedItemBarWidget)
				LinkedItemBarWidget->SetPercent(0.f, UsageTime);
			
			Player->GetHUDWidget()->OnConsumableUsed();
			Player->GetHUDWidget()->GetInformWidget()->DeActivateConsumableInstruction();
			
			Player->GetInvenSystem()->GetInvenUI()->SetUsingItem(nullptr);
			if (Player->GetInvenSystem()->GetInvenUI()->GetIsPanelOpened() && Player->GetInvenSystem()->GetInvenUI()->GetUsingItem() == nullptr)
				Player->GetInvenSystem()->GetInvenUI()->SetVisibility(ESlateVisibility::Visible);

			Player->GetInvenSystem()->GetInvenUI()->UpdateWidget();
		}
		
		if (ItemCurStack == 0)
		{

			ItemUser->GetInvenComponent()->RemoveItemFromMyList(this);
			HandleDestroy(); // 각 아이템 별 Destroy 하는 시점이 다름 (Pure-virtual Primitive operation)
			return;
		}
	}
		return;
	default:
		return;
	}
}

void AC_ConsumableItem::SetLinkedItemBarWidget(UC_BasicItemBarWidget* InItemBarWidget)
{
	LinkedItemBarWidget = InItemBarWidget;
}

void AC_ConsumableItem::InitializeItem(FName NewItemCode)
{
	Super::InitializeItem(NewItemCode);
	static const FString ContextString(TEXT("HealItem Lookup"));

	//UDataTable* HealSoundDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Project_PUBG/Common/Item/ItemDataTables/DT_HealingItemUsingSound.DT_HealingItemUsingSound"));

	UC_GameInstance* GI = Cast<UC_GameInstance>(GetGameInstance());
	UDataTable* HealSoundDataTable = GI->GetDataTables()[EDataTableType::HealItemUsingSound];

	if (HealSoundDataTable)
	{
		const FHealItemSoundData* ItemData = HealSoundDataTable->FindRow<FHealItemSoundData>(ItemCode, ContextString);
		if (ItemData)
		{
			UsingSoundData = ItemData;  // 원본 참조 저장
			AudioComponent->SetSound(UsingSoundData->UsingSound);
		}
	}
}

FName AC_ConsumableItem::GetConsumableItemName(EConsumableItemType InConsumableItemType)
{
	if (InConsumableItemType == EConsumableItemType::MAX) return "";
	return ConsumableItemNameMap[InConsumableItemType];
}


bool AC_ConsumableItem::StartUsingConsumableItem(AC_BasicCharacter* InItemUser)
{
	if (!IsAvailableToStartUsing(InItemUser))				return false; // Pure-virtual Primitive Operation (자식단계에서 세부 구현이 이루어짐)
	if (InItemUser->GetSwimmingComponent()->IsSwimming())	return false;
	if (ConsumableItemState != EConsumableItemState::IDLE)	return false;

	ItemUser = InItemUser;

	if (ItemUser->GetIsActivatingConsumableItem())				return false;
	if (!UsingMontageMap.Contains(ItemUser->GetPoseState()))    return false;
	if (!UsingMontageMap[ItemUser->GetPoseState()].AnimMontage) return false;


	float PlayTime = ItemUser->PlayAnimMontage(UsingMontageMap[ItemUser->GetPoseState()]);

	// 다른 Montage에 의해 방해 받았을 때
	if (PlayTime == 0.f) return false;

	//SetActorTickEnabled(true);

	// 사용 시작하기
	ConsumableItemState = EConsumableItemState::ACTIVATING;

	//사용 효과음 재생.
	PlayUsingSound();
	
	if (AC_Player* UserPlayer = Cast<AC_Player>(ItemUser))
	{
		//InvenUI에 현재 사용중인 아이템 설정. InvenUI UsingItem이 있을 때 Tick에서 visibility 조정하는 중
		UserPlayer->GetInvenSystem()->GetInvenUI()->SetUsingItem(this); 
	}
	
	OnStartUsing(); // Pure-virtual Primitive Operation

	// 현재 들고 있는 무기가 존재한다면 무기 잠깐 몸 쪽에 붙이기 시도
	ItemUser->GetEquippedComponent()->TryAttachCurWeaponToHolsterWithoutSheathMotion();

	// 사용자의 bIsActivatingConsumableItem 세팅
	ItemUser->SetIsActivatingConsumableItem(true, this);

	//ItemDatas.ItemStack--;

	return true;
}

bool AC_ConsumableItem::CancelActivating()
{
	if (ConsumableItemState != EConsumableItemState::ACTIVATING) return false;

	if (AC_Player* Player = Cast<AC_Player>(ItemUser))
		Player->GetHUDWidget()->OnCancelActivatingConsumableItem();

	//사운드 중지.
	StopUsingSound();

	if (LinkedItemBarWidget)
		LinkedItemBarWidget->SetPercent(0.f, UsageTime);

	UAnimInstance* UserAnimInstance = ItemUser->GetMesh()->GetAnimInstance();

	for (auto& Pair : UsingMontageMap)
	{
		if (UserAnimInstance->Montage_IsPlaying(Pair.Value.AnimMontage))
		{
			UserAnimInstance->Montage_Stop(0.2f, Pair.Value.AnimMontage);
			break;
		}
	}



	// 착용 중인 무기가 있었을 때 재착용 시도
	ItemUser->GetEquippedComponent()->TryReAttachCurWeaponToHand();

	ItemUser->SetIsActivatingConsumableItem(false, nullptr);
	
	if (AC_Player* Player = Cast<AC_Player>(OwnerCharacter))
	{
		Player->GetInvenSystem()->GetInvenUI()->SetUsingItem(nullptr);
		if (Player->GetInvenSystem()->GetInvenUI()->GetIsPanelOpened() && Player->GetInvenSystem()->GetInvenUI()->GetUsingItem() == nullptr)
			Player->GetInvenSystem()->GetInvenUI()->SetVisibility(ESlateVisibility::Visible);
		//Player->GetInvenSystem()->GetInvenUI()->InitWidget();
		Player->GetInvenSystem()->GetInvenUI()->UpdateWidget();

		Player->GetHUDWidget()->GetInformWidget()->DeActivateConsumableInstruction();
		Player->SetCanMove(true);
		if (LinkedItemBarWidget)
			LinkedItemBarWidget->SetPercent(0.f, UsageTime);
	}

	OnCancelActivating(); // Pure-virtual Primitive operation

	ConsumableItemState = EConsumableItemState::IDLE;

	UsingTimer			= 0.f;
	//ItemUser			= nullptr;

	//SetActorTickEnabled(false);

	return true;

}

bool AC_ConsumableItem::Interaction(AC_BasicCharacter* Character)
{

	switch (ItemPlace)
	{
	case EItemPlace::AROUND:
		if (!OwnerCharacter) return MoveToInven(Character, this->GetItemCurStack());
	case EItemPlace::INVEN:
		return StartUsingConsumableItem(Character);
		//break;
	default:
		return false;
	}
}

bool AC_ConsumableItem::MoveInvenToAround(AC_BasicCharacter* Character, int32 InStack)
{
	UC_InvenComponent* InvenComp = Character->GetInvenComponent();

	if (!Character) return false;

	Character->GetInvenComponent()->RemoveItemFromMyList(this);
	//TODO: 분할해서 버리는 경우 새로 스폰해주어야함.
	ItemPlace = EItemPlace::AROUND;
	SetOwnerCharacter(nullptr);
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	//Collider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	SetLinkedItemBarWidget(nullptr);

	//바닥 레이 캐스팅 받아와서 바닥에 아이템 생성하기.
	SetActorLocation(GetGroundLocation(Character) + RootComponent->Bounds.BoxExtent.Z);

	//SetActorRotation(FQuat(0,0,0));

	return true;
}

bool AC_ConsumableItem::MoveAroundToInven(AC_BasicCharacter* Character, int32 InStack)
{
	UC_InvenComponent* InvenComp = Character->GetInvenComponent();

	uint8 ItemStackCount = InvenComp->LoopCheckVolume(this);

	if (ItemStackCount == 0)
	{
		UC_Util::Print("Not Enough Volume");
		if (AC_Player* Player = Cast<AC_Player>(Character))
			Player->GetHUDWidget()->GetInformWidget()->AddPlayerWarningLog("Cannot pick up item : Inventory is full");
		
		return false; //인벤에 넣을 수 있는 아이템의 갯수가 0 이면 넣을 수 없으므로 return false;
	}

	// 이 이후로 파밍 성공 처리

	// HealingTutorial Looting Goal 처리 (HealingItem, BoostItem에 따른 LootingGoalIndex를 다르게 처리)
	if (HealingTutorialDelegate.IsBound() && Cast<AC_Player>(Character))
		HealingTutorialDelegate.Execute(HealingTutorialLootingGoalIndex, -1);

	AC_Item* FoundItem = InvenComp->FindMyItem(this); //인벤에 같은 아이템을 찾아옴, 없다면 nullptr;

	if (ItemCurStack == ItemStackCount) //아이템 전부를 인벤에 넣을 수 있을 때.
	{
		if (IsValid(FoundItem))
		{
			//인벤에 해당 아이템이 존재 할 때.
			FoundItem->SetItemStack(FoundItem->GetItemCurStack() + ItemStackCount);
			//invenComp->GetCurVolume() += FoundItem->GetItemDatas().ItemVolume * ItemStackCount;
			//TODO : destroy를 해도 잔상이 남는것을 대비해서 해놓음 만약 없이도 잔상이 안남는다면 지울 것.
			InvenComp->AddInvenCurVolume(this->ItemDataRef->ItemVolume * ItemStackCount);
			InvenComp->RemoveItemToAroundList(this);
			this->SetActorEnableCollision(false);
			this->SetActorHiddenInGame(true);
			this->DestroyItem();
		}
		else
		{
			//인벤에 해당 아이템이 존재하지 않을 때.
			InvenComp->AddItemToMyList(this);
			//인게임에서 보이는 것과 collision문제 때문에 임시로 꺼둠.
			//this->SetActorEnableCollision(false);
			this->SetActorHiddenInGame(true);
			this->SetActorEnableCollision(false);
			//던질 때 켜짐. 이걸로 만약 아이템의 오버랩이 안끝난다면 다른 방법 고민->ToInven에서 SetActorEnableCollision를 꺼주고 던질때 혹은 ToAround에서 켜주기.
			//Collider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		
		if (InvenComp->FindMyItem(this)) InvenComp->RemoveItemToAroundList(this);
		
		return true;
	}
	
	//아이템의 일부만 인벤에 넣을 수 있을 때.

	if (IsValid(FoundItem)) // 인벤에 아이템이 이미 있을 때
	{
		this->SetItemStack(ItemCurStack - ItemStackCount);
		FoundItem->SetItemStack(FoundItem->GetItemCurStack() + ItemStackCount);

		InvenComp->AddInvenCurVolume(this->ItemDataRef->ItemVolume * ItemStackCount);

		return true;
	}
	
	// 인벤에 아이템이 없을 때
	AC_ConsumableItem* NewItem = Cast<AC_ConsumableItem>(SpawnItem(Character));//아이템 복제 생성

	NewItem->SetItemStack(ItemStackCount);
	this->SetItemStack(ItemCurStack - ItemStackCount);

	InvenComp->AddItemToMyList(NewItem);

	NewItem->SetActorHiddenInGame(true);
	//collider 관련 설정 추가해야 할 수 있음.
	//만약 추가해야 된다면 MoveToInven에서 SetActorEnableCollision을 꺼주고 던질 때 켜주는 방식으로.
	return true;
}

bool AC_ConsumableItem::MoveAroundToSlot(AC_BasicCharacter* Character, int32 InStack)
{
	return MoveToInven(Character, InStack);
}

void AC_ConsumableItem::PlayUsingSound()
{
	if (UsingSoundData->UsingSound && AudioComponent)
	{
		AudioComponent->bAllowSpatialization = (Cast<AC_Player>(ItemUser) == nullptr); // Player or Enemy에 따른 2D Sound or 3D sound 설정
		AudioComponent->Play();
	}
}

void AC_ConsumableItem::StopUsingSound()
{
	if (UsingSoundData->UsingSound && AudioComponent)
	{
		AudioComponent->Stop();
	}
}

bool AC_ConsumableItem::GetActivationProgressTimeRatio(float& OutRatio) const
{
	if (ConsumableItemState != EConsumableItemState::ACTIVATING) return false;
	OutRatio = UsingTimer / UsageTime;
	return true;
}
