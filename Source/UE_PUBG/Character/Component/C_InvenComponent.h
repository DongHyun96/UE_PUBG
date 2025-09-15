// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item/C_Item.h"
//#include "InvenUserInterface/C_InvenUiWidget.h"
#include "C_InvenComponent.generated.h"

#define MAX_STACK 120

UENUM(BlueprintType)
enum class EBackPackLevel : uint8
{
	LV0,
	LV1,
	LV2,
	LV3
};

UENUM(BlueprintType)
enum class EEquipSlot : uint8
{
	HELMET,
	BACKPACK,
	VEST
};

/// <summary>
/// 기본 MaxVolume은 70
/// C_InvenComponent는 0~3레벨까지 존재하며 이는 가방의 레벨에 따라서 변화한다.
///가방은 자동장착되며 레벨을 올리는데는 제한이 없지만
///다운그레이드의 경우 다운되는 레벨의 무게를 초과하는 인벤상태라면
///불가능하다.에서 만들어서 거기서 InvenC
/// EquipmentSystem을 Blueprintomponent와 EquippedComponent를 사용해서 인벤시스템와 UI를 제작.
/// </summary>
//UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class UE_PUBG_API UC_InvenComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UC_InvenComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetOwnerCharacter(class AC_BasicCharacter* InOwnerCharacter) { OwnerCharacter = InOwnerCharacter; }

	//가방교체, 아이템획득시 해당함수로 용량이 충분한지 확인하여 T or F를 반환. 블루프린트에서 사용 가능하도록 열어줘야 할 수 있음.
	/// <summary>
	/// Item을 인벤에 넣을 때, 나의 최대 용량(MaxVolume)을 넘는지를 검사.
	/// 아이템의 stack에 따른 용량의 변화 작업해야함.
	/// </summary>
	/// <param name="volume"></param>
	/// <returns></returns>
	bool CheckVolume(class AC_Item* item);

	/// <summary>
	/// 아이템을 몇개까지 인벤에 넣을 수 있는지 loop Check
	/// </summary>
	/// <param name="item">인벤에 넣을 아이템</param>
	/// <returns>넣을 수 있는 아이템의 Stack(갯수)</returns>
	float LoopCheckVolume(AC_Item* item);

	//UFUNCTION(BlueprintCallable)
	//void Interaction(AC_Item* wilditem);

	//가방의 용량을 반환
	float CheckBackPackVolume(uint32 backpacklevel);
	float CheckBackPackVolume(EBackPackLevel backpacklevel);

	//UFUNCTION(BlueprintCallable)
	//void RemoveBackPack();
	
	UFUNCTION(BlueprintCallable)
	class AC_EquipableItem* SetSlotEquipment(EEquipSlot InSlot, AC_EquipableItem* EquipItem);

	AC_EquipableItem* GetSlotEquipment(EEquipSlot InSlot) const { return EquipmentItems[InSlot]; }

	/// <summary>
	/// 
	/// </summary>
	/// <param name="item">찾고자 하는 아이템</param>
	/// <returns>찾고자 하는 아이템. (없다면 nullptr 반환)</returns>
	UFUNCTION(BlueprintCallable)
	AC_Item* FindMyItem(AC_Item* item);

	/// <summary>
	/// 아이템 객체의 변수 ItemCode를 통해 InvenComponent에 있는 MyItems에 해당 코드의 아이템이 있는지 확인.
	/// </summary>
	/// <param name="itemName">찾고자 하는 아이템</param>
	/// <returns>찾고자 하는 아이템. (없다면 nullptr 반환)</returns>
	UFUNCTION(BlueprintCallable)
	AC_Item* FindMyItemByName(const FName& itemName);

	bool HandleItemStackOverflow(AC_Item* InItem);

	// MyItemList에 아이템을 추가하는 함수
	void AddItemToMyList(AC_Item* item); //{ testMyItems.Add(item->GetItemDatas().ItemName, item); }

	// MyItemList에 아이템을 제거하는 함수
	void RemoveItemFromMyList(AC_Item* item);// { testMyItems.Remove(item->GetItemDatas().ItemName); }

	/// <summary>
	/// 해당 ItemCode의 Item들 모두 삭제 (Item Destroy까지 직접 관여, TrainingGround의 Player Inven탄 초기화 시 사용하는 중)
	/// </summary>
	void ClearAllItemCodeItemsFromMyList(const FName& ItemCode);

	// MyItemList에 아이템을 Destroy하는 함수
	void DestroyMyItem(AC_Item* DestroyedItem);

	// Around Item List에 아이템을 추가하는 함수
	void AddItemToAroundList(AC_Item* item) { AroundItems.Add(item); }

	// Around Item List에 아이템을 제거하는 함수
	void RemoveItemToAroundList(AC_Item* item) { AroundItems.Remove(item); }

	// 캐릭터가 가방을 매고 있는지 확인하고 총의 위치를 조정하는 함수.
	void CheckBackPackOnCharacter();
	
	// 현재 인벤의 용량에 용량을 추가하는 함수.
	void AddInvenCurVolume(float ItemVolume);

public:
	
	/// <summary>
	/// <para> 인벤토리 내의 아이템 모두 일괄 삭제처리 </para>  // TODO : 추후 저장 및 다시 원상복구 처리할 수 있는 방안이 나오면 그렇게 할 것
	/// <para> TrainingGround Player CombatField 시작 시 호출될 예정 </para>
	/// </summary>
	void ClearInventory();

public: //Getter and Seter
	EBackPackLevel GetCurBackPackLevel() { return CurBackPackLevel; } 
	//EBackPackLevel SetCurBackPackLevel(uint8 level) { CurBackPackLevel = (EBackPackLevel)level; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	float GetMaxVolume() { return MaxVolume; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	float GetCurVolume() { return CurVolume; }

	//UFUNCTION(BlueprintCallable, Category = "Inventory")
	//class AC_BackPack* GetMyBackPack() { return MyBackPack; }

	UFUNCTION(BlueprintCallable)
	void GetMapValues(const TMap<FString, AC_Item*>& Map, TArray<AC_Item*>& Values);

	//void SetMyBackPack(AC_BackPack* inBackPack) { MyBackPack = inBackPack; }

	float GetVestVolume();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	TMap<EEquipSlot, AC_EquipableItem*> GetEquipmentItems() { return EquipmentItems; }

	const TMap<FName, TArray<class AC_Item*>>& GetMyItems() { return MyItems; }

	TArray<AC_Item*>& GetAroundItems() { return AroundItems; }

	void SetAroundItems(TArray<AC_Item*> InAroundItemList) { AroundItems = InAroundItemList; }

	UFUNCTION(BlueprintCallable)
	int32 GetTotalStackByItemName(const FName& ItemName);

	void DecreaseItemStack(const FName& ItemName, int32 Amount);

protected:
	AC_BasicCharacter* OwnerCharacter{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MaxVolume = 70.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float CurVolume =  0.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EBackPackLevel CurBackPackLevel = EBackPackLevel::LV0;

	EBackPackLevel PreBackPackLevel = EBackPackLevel::LV0;

	/// <summary>
	/// 기본 : Ui에서 현재 내 아이템목록을 보여주기 위함.
	/// 추가 : 죽었을때 떨굴 내 아이템 목록.(총이나 가방같이 장착아이템들도 떨구어야하는데 이걸 죽었을때 한번에 떨구기 위한 방법은 2가지로 생각. 
	/// 첫번째는 일일이 떨꾸어주는것. 
	/// 두번째는 MyItem에 다 넣고 한번에 떨구는 것.
	/// </summary>

	//UPROPERTY(BlueprintReadWrite, EditAnywhere)
	//TMap<FName, AC_Item*> MyItems;

	TMap<FName, TArray<AC_Item*>> MyItems{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<AC_Item*> AroundItems{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<EEquipSlot, AC_EquipableItem*> EquipmentItems = 
	{
	{EEquipSlot::HELMET, nullptr},
	{EEquipSlot::BACKPACK, nullptr},
	{EEquipSlot::VEST, nullptr}
	};

	//UPROPERTY(BlueprintReadWrite, EditAnywhere)
	//AC_BackPack* MyBackPack {};
	
};


