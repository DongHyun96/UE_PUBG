
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "C_Item.generated.h"



/// <summary>
/// AttachableItem PartsName Enum class
/// C_Gun���� ���� ���� ����
/// </summary>
UENUM(BlueprintType)
enum class EPartsName : uint8
{
	NONE,
	SCOPE,
	MUZZLE,
	GRIP,
	MAGAZINE,
	GUNSTOCK,
	MAX
};

/// <summary>
/// Attachment ������ �̸���
/// </summary>
UENUM(BlueprintType)
enum class EAttachmentNames : uint8
{
	REDDOT,
	SCOPE4,
	SCOPE8,
	VERTGRIP,
	EXTENDMAG,
	QUICKMAG,
	COMPENSATOR,
	SUPPRESSOR,
	MAX
};
/// <summary>
/// ���� ����� ��������� �ʰ� ����.
/// </summary>

/// <summary>
/// �������� Ÿ���� �����༭ �κ��丮���� �����Ҷ� �˸´� UI�� ã�ư����� �ϰ�����.
/// NONE�� ���߻�Ȳ�� ����ؼ� ������.
/// ����Ŭ���������� �����ڿ��� �����ϰ� ����.
/// </summary>
UENUM(BlueprintType)
enum class EItemTypes : uint8
{
	NONE,
	HELMET,
	VEST,
	BACKPACK,
	MAINGUN,
	MELEEWEAPON,
	THROWABLE,
	ATTACHMENT,
	CONSUMPTIONITEM
};

UENUM(BlueprintType)
enum class EItemPlace : uint8
{
	NONE,
	AROUND,
	INVEN,
	SLOT
};

UENUM(BlueprintType)
enum class EItemNames : uint8
{
	NONE,
	HELMET,
	ARMORE,
	BACKPACK,
	MAINGUN,
	MELEEWEAPON,
	THROWABLE,
	ATTACHMENT,
	CONSUMPTIONITEM
};

USTRUCT(BlueprintType)
struct FSpawnItemData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SpawnItemData")
	FSoftObjectPath itemMesh{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SpawnItemData")
	FSoftObjectPath  SpawnedItemData{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SpawnItemData")
	TSubclassOf<AC_Item> ItemClass{};

};

USTRUCT(BlueprintType)
struct FItemData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
	FString ItemName{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
	EItemTypes ItemType{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
	UTexture2D* ItemBarIcon = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
	UTexture2D* ItemSlotImage = nullptr;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Mesh")
	//FSoftObjectPath ItemMeshPath{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
	uint8 ItemCurStack = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
	uint8 ItemMaxStack = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
	float ItemVolume = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
	EItemPlace ItemPlace = EItemPlace::AROUND;
};	



/// <summary>
/// �Ҹ�����ۺз��� ���� ���� Ŭ����.
/// ���濡 ���� �������� ���� Ŭ����.
/// ��)ź, ������, ����, ����ź��.
/// ���濡 ���� ����ǰ ��� �ۼ��ؾ���.
/// </summary>
UCLASS(abstract, BlueprintType)
class UE_PUBG_API AC_Item : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AC_Item();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//UFUNCTION()
	//virtual void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//virtual void Interaction(class AC_BasicCharacter* character) PURE_VIRTUAL(AC_Item::Interaction, );

	//�������� ������ �����ϰ� ���� ������ ����.
	UFUNCTION(BlueprintCallable)
	virtual void DetachItem();

	virtual void SetOwnerCharacter(class AC_BasicCharacter* InOwnerCharacter) { OwnerCharacter = InOwnerCharacter; }

	/// <summary>
	/// ItemBar���� ��Ŭ������ ����ϴ� �⺻ ��ȣ�ۿ�.
	/// �ڽĴܰ迡�� �������̵��ؼ� ���.
	/// �������� OwnerCharacter�� �ִ��� �������� �⺻���� �Ǵ��ϰ� ��쿡 ���� �ٸ� ����� ������ ����.)
	/// ��ȣ�ۿ����� �ٲ۴ٸ�. OwnerCharcter�� ���ο� ����, �����ۿ� ���� UseItem�� �̸��� �ٸ� �������� �Լ��� ��밡��.
	/// </summary>
	UFUNCTION(BlueprintCallable)
	virtual bool Interaction(AC_BasicCharacter* Character) PURE_VIRTUAL(AC_Item::Interaction, return false;);

	virtual bool UseItem() PURE_VIRTUAL(AC_Item::UseItem, return false;);

	/// <summary>
	/// �÷��̾ �������� �����Ҷ� ���Ǵ� �Լ�.
	/// �� Ŭ�������� override�ؼ� Ŭ������ �°� ���.
	/// ���Ŀ� �̸� ���� ����
	/// AddItemToInventory, PickupItem
	/// </summary>
	UFUNCTION(BlueprintCallable)
	virtual void PickUpItem(AC_BasicCharacter* Character) PURE_VIRTUAL(AC_Item:PickUpItem, );

	/// <summary>
	/// Around -> MyItems�� �������� �̵��� ��, �̹� �ش� �������� �����Ҷ� ItemStack�� C_Item������ Set�ϴ� �Լ�.
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void SetItemStack(uint8 inItemStack) { ItemDatas.ItemCurStack = inItemStack; }
	
	/// <summary>
	/// �÷��̾ �������� ���� �� ���Ǵ� �Լ�.
	/// �� Ŭ�������� override�ؼ� �˸°� ���
	/// ���Ŀ� �̸� ���氡��
	/// RemoveItemFromInventory
	/// </summary>
	UFUNCTION(BlueprintCallable)
	virtual void DropItem(AC_BasicCharacter* Character); //PURE_VIRTUAL(AC_Item:DropItem, );

	//�������� ������ 1 �÷���.
	UFUNCTION(BlueprintCallable)
	void AddItemStack() { ItemDatas.ItemCurStack += 1; }
	
	//�������� ������ 1 ������.
	UFUNCTION(BlueprintCallable)
	void DeductItemStack() { ItemDatas.ItemCurStack -= 1; }

	UFUNCTION(BlueprintCallable)
	virtual bool LegacyMoveToInven(AC_BasicCharacter* Character) PURE_VIRTUAL(AC_Item:MoveToInven, return false;);
	
	UFUNCTION(BlueprintCallable)
	virtual bool LegacyMoveToAround(AC_BasicCharacter* Character) PURE_VIRTUAL(AC_Item:MoveToAround, return false;);
	
	UFUNCTION(BlueprintCallable)
	virtual bool LegacyMoveToSlot(AC_BasicCharacter* Character) PURE_VIRTUAL(AC_Item:MoveToSlot, return false;);

	UFUNCTION(BlueprintCallable)
	virtual bool MoveToInven(AC_BasicCharacter* Character);
	
	UFUNCTION(BlueprintCallable)
	virtual bool MoveToAround(AC_BasicCharacter* Character);
	
	UFUNCTION(BlueprintCallable)
	virtual bool MoveToSlot(AC_BasicCharacter* Character);

	UFUNCTION(BlueprintCallable)
	virtual AC_Item* SpawnItem(AC_BasicCharacter* Character);

	void SetItemPlace(EItemPlace InPlace) { ItemDatas.ItemPlace = InPlace; }

	//ĳ������ �عٴ��� ���� Ʈ���̽��� location�� ��ȯ����.
	FVector GetGroundLocation(AC_BasicCharacter* Character);

private:
	virtual bool MoveSlotToAround(AC_BasicCharacter* Character);
	virtual bool MoveSlotToInven(AC_BasicCharacter* Character);
	virtual bool MoveSlotToSlot(AC_BasicCharacter* Character);

	virtual bool MoveInvenToAround(AC_BasicCharacter* Character);
	virtual bool MoveInvenToInven(AC_BasicCharacter* Character);
	virtual bool MoveInvenToSlot(AC_BasicCharacter* Character);

	virtual bool MoveAroundToAround(AC_BasicCharacter* Character);
	virtual bool MoveAroundToInven(AC_BasicCharacter* Character);
	virtual bool MoveAroundToSlot(AC_BasicCharacter* Character);

public:
	FItemData GetItemDatas() { return ItemDatas; }

	/// <summary>
	/// ������ �ϳ��� Volume
	/// </summary>
	/// <returns></returns>
	UFUNCTION(BlueprintCallable)
	float GetOnceVolume() { return ItemDatas.ItemVolume; }

	UFUNCTION(BlueprintCallable)
	float GetAllVolume() { return ItemDatas.ItemVolume * ItemDatas.ItemCurStack; }

	virtual AC_BasicCharacter* GetOwnerCharacter() { return OwnerCharacter; }


protected:
	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
	//EItemTypes MyItemType;
	//
	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
	//FString ItemName;
	//
	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
	//UTexture2D* ItemIcon;		
	//
	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
	//uint8 ItemStatck;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
	FItemData ItemDatas;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
	AC_BasicCharacter* OwnerCharacter{};
};
// Fill out your copyright notice in the Description page of Project Settings.
