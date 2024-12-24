// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item/C_Item.h"
#include "InvenUserInterface/C_InvenUiWidget.h"
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
/// �⺻ MaxVolume�� 70
/// C_InvenComponent�� 0~3�������� �����ϸ� �̴� ������ ������ ���� ��ȭ�Ѵ�.
///������ �ڵ������Ǹ� ������ �ø��µ��� ������ ������
///�ٿ�׷��̵��� ��� �ٿ�Ǵ� ������ ���Ը� �ʰ��ϴ� �κ����¶��
///�Ұ����ϴ�.���� ���� �ű⼭ InvenC
/// EquipmentSystem�� Blueprintomponent�� EquippedComponent�� ����ؼ� �κ��ý��ۿ� UI�� ����.
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

	//���汳ü, ������ȹ��� �ش��Լ��� �뷮�� ������� Ȯ���Ͽ� T or F�� ��ȯ. �������Ʈ���� ��� �����ϵ��� ������� �� �� ����.
	
	//UFUNCTION(BlueprintCallable)
	//uint16�� �ش� ��ũ�ΰ� �������� �ʴ´�. uint8, uint32�� �����Ѵ�.
	bool CheckVolume(class AC_Item* item);

	/// <summary>
	/// �������� ����� �κ��� ���� �� �ִ��� loop Check
	/// </summary>
	/// <param name="item">�κ��� ���� ������</param>
	/// <returns>���� �� �ִ� �������� Stack(����)</returns>
	float LoopCheckVolume(AC_Item* item);

	//����� �뷮�� �˻��ؼ� ���� ��ü �� ����.
	UFUNCTION(BlueprintCallable)
	bool CheckMyBackPack(class AC_BackPack* backpack);

	//UFUNCTION(BlueprintCallable)
	void Interaction(AC_Item* wilditem);

	//������ �뷮�� ��ȯ
	float CheckBackPackVolume(uint32 backpacklevel);
	float CheckBackPackVolume(EBackPackLevel backpacklevel);

	UFUNCTION(BlueprintCallable)
	void DroppingItem(AC_Item* myitem);

	UFUNCTION(BlueprintCallable)
	void RemoveBackPack();

	UFUNCTION(BlueprintCallable)
	void EquippedBackPack(AC_BackPack* backpack);

	/// <summary>
	/// �������� �� �κ��� �߰��ϴ� �Լ�.
	/// item->OwnerCharacter = this->OwnerCharacter
	/// item->HiddeninGame = ture;
	/// item->SetActorEnableCollision = false; ���·� ��ȯ
	/// �κ��丮 ����üũ�� �������� �����ϴ� ������ ��ħ.
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
	/// <param name="item">ã���� �ϴ� ������</param>
	/// <returns>ã���� �ϴ� ������. (���ٸ� nullptr ��ȯ)</returns>
	UFUNCTION(BlueprintCallable)
	AC_Item* FindMyItem(AC_Item* item);

	AC_Item* FindMyItem(FString itemName);

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

	void SetMyBackPack(AC_BackPack* inBackPack) { MyBackPack = inBackPack; }

	void CheckBackPackOnCharacter();

	void AddInvenCurVolume(float ItemVolume);

	//void SetEquipmentItem(EEquipSlot inSlot, AC_EquipableItem EquipableItem);
	//Getter and Seter
public:
	EBackPackLevel GetCurBackPackLevel() { return CurBackPackLevel; } 
	//EBackPackLevel SetCurBackPackLevel(uint8 level) { CurBackPackLevel = (EBackPackLevel)level; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	float GetMaxVolume() { return MaxVolume; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	float GetCurVolume() { return CurVolume; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	class AC_BackPack* GetMyBackPack() { return MyBackPack; }

public:
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	TMap<EEquipSlot, AC_EquipableItem*> GetEquipmentItems() { return EquipmentItems; }

	const TMap<FString, TArray<class AC_Item*>>& GetTestMyItems() { return testMyItems; }

	//const TMap<FString, class AC_Item*>& GetTestMyItems() { return testMyItems; }

	TMap<FString, class AC_Item*>& GetTestAroundItems() { return testAroundItems; }


	TArray<class AC_Item*>& GetNearItems() { return NearItems; }

protected:
	void InitMyitems();
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
	/// �⺻ : Ui���� ���� �� �����۸���� �����ֱ� ����.
	/// �߰� : �׾����� ���� �� ������ ���.(���̳� ���氰�� ���������۵鵵 ��������ϴµ� �̰� �׾����� �ѹ��� ������ ���� ����� 2������ ����. 
	/// ù��°�� ������ ���پ��ִ°�. 
	/// �ι�°�� MyItem�� �� �ְ� �ѹ��� ������ ��.
	/// </summary>
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<AC_Item*> MyItems;

	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "NearItmes Array")
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<AC_Item*> NearItems;

	//UPROPERTY(BlueprintReadWrite, EditAnywhere)
	//TMap<FString, AC_Item*> testMyItems;

	//UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<FString, TArray<AC_Item*>> testMyItems;


	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<FString, AC_Item*> testAroundItems;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<EEquipSlot, AC_EquipableItem*> EquipmentItems = 
	{
	{EEquipSlot::HELMET, nullptr},
	{EEquipSlot::BACKPACK, nullptr},//�̰� ���� �Ⱦ��� ����.
	{EEquipSlot::VEST, nullptr}
	};


	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UC_InvenUiWidget* InvenUI;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	APlayerController* PlayerController;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> InvenUiClass;
protected:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	AC_BackPack* MyBackPack {};

private:

	
};
