// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item/C_Item.h"
#include "InvenUserInterface/C_InvenUiWidget.h"
#include "C_InvenComponent.generated.h"
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
/// C_InvenComponent는 0~3레벨까지 존재하며 이는 가방의 레벨에 따라서 변화한다.
///가방은 자동장착되며 레벨을 올리는데는 제한이 없지만
///다운그레이드의 경우 다운되는 레벨의 무게를 초과하는 인벤상태라면
///불가능하다.
/// EquipmentSystem을 Blueprint에서 만들어서 거기서 InvenComponent와 EquippedComponent를 사용해서 인벤시스템와 UI를 제작.
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
	
	//UFUNCTION(BlueprintCallable)
	//uint16은 해당 매크로가 지원하지 않는다. uint8, uint32는 지원한다.
	bool CheckVolume(class AC_Item* item);

	uint8 LoopCheckVolume(AC_Item* item);

	//가방과 용량을 검사해서 가방 교체 및 장착.
	UFUNCTION(BlueprintCallable)
	bool CheckMyBackPack(class AC_BackPack* backpack);

	//UFUNCTION(BlueprintCallable)
	void Interaction(AC_Item* wilditem);

	//가방의 용량을 반환
	uint16 CheckBackPackVolume(uint32 backpacklevel);
	uint16 CheckBackPackVolume(EBackPackLevel backpacklevel);

	UFUNCTION(BlueprintCallable)
	void DroppingItem(AC_Item* myitem);

	UFUNCTION(BlueprintCallable)
	void RemoveBackPack();

	UFUNCTION(BlueprintCallable)
	void EquippedBackPack(AC_BackPack* backpack);

	/// <summary>
	/// 아이템을 내 인벤에 추가하는 함수.
	/// item->OwnerCharacter = this->OwnerCharacter
	/// item->HiddeninGame = ture;
	/// item->SetActorEnableCollision = false; 상태로 전환
	/// 인벤토리 공간체크후 아이템을 습득하는 과정을 거침.
	/// </summary>
	/// <param name="item"></param>
	/// <returns></returns>
	UFUNCTION(BlueprintCallable)
	bool AddItem(AC_Item* item);

	UFUNCTION(BlueprintCallable)
	class AC_EquipableItem* SetSlotEquipment(EEquipSlot InSlot, AC_EquipableItem* EquipItem);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="item">찾고자 하는 아이템</param>
	/// <returns>찾고자 하는 아이템. (없다면 nullptr 반환)</returns>
	UFUNCTION(BlueprintCallable)
	AC_Item* FindMyItem(AC_Item* item);

	void AddItemToMyList(AC_Item* item); //{ testMyItems.Add(item->GetItemDatas().ItemName, item); }

	void RemoveItemToMyList(AC_Item* item);// { testMyItems.Remove(item->GetItemDatas().ItemName); }

	//
	void AddItemToNearList(AC_Item* item) { NearItems.Add(item); }
	void RemoveItemNearList(AC_Item* item) { NearItems.Remove(item); }
	//

	void AddItemToAroundList(AC_Item* item) { testAroundItems.Add(item->GetItemDatas().ItemName, item); }

	void RemoveItemToAroundList(AC_Item* item) { testAroundItems.Remove(item->GetItemDatas().ItemName); }

	UFUNCTION(BlueprintCallable)
	void GetMapValues(const TMap<FString, AC_Item*>& Map, TArray<AC_Item*>& Values);

	void OpenInvenUI();

	void InitInvenUI();

	//Getter and Seter
public:
	EBackPackLevel GetCurBackPackLevel() { return CurBackPackLevel; } 
	//EBackPackLevel SetCurBackPackLevel(uint8 level) { CurBackPackLevel = (EBackPackLevel)level; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetMaxVolume() { return MaxVolume; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetCurVolume() { return CurVolume; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	class AC_BackPack* GetMyBackPack() { return MyBackPack; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	TMap<EEquipSlot, AC_EquipableItem*> GetEquipmentItems() { return EquipmentItems; }


	const TMap<FString, class AC_Item*>& GetTestMyItems() { return testMyItems; }

	TMap<FString, class AC_Item*>& GetTestAroundItems() { return testAroundItems; }


	TArray<class AC_Item*>& GetNearItems() { return NearItems; }

protected:
	void InitMyitems();
protected:
	AC_BasicCharacter* OwnerCharacter{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 MaxVolume = 70;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 CurVolume =  0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EBackPackLevel CurBackPackLevel = EBackPackLevel::LV0;

	EBackPackLevel PreBackPackLevel = EBackPackLevel::LV0;

	/// <summary>
	/// 기본 : Ui에서 현재 내 아이템목록을 보여주기 위함.
	/// 추가 : 죽었을때 떨굴 내 아이템 목록.(총이나 가방같이 장착아이템들도 떨구어야하는데 이걸 죽었을때 한번에 떨구기 위한 방법은 2가지로 생각. 
	/// 첫번째는 일일이 떨꾸어주는것. 
	/// 두번째는 MyItem에 다 넣고 한번에 떨구는 것.
	/// </summary>
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<AC_Item*> MyItems;

	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "NearItmes Array")
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<AC_Item*> NearItems;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<FString, AC_Item*> testMyItems;

	//TMap<FString, AC_Item*> testMyItems;


	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<FString, AC_Item*> testAroundItems;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<EEquipSlot, AC_EquipableItem*> EquipmentItems;

	UC_InvenUiWidget* InvenUI;

	APlayerController* PlayerController;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> InvenUiClass;
protected:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	AC_BackPack* MyBackPack {};

private:

	
};
