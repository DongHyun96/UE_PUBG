
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
	ARMOR,
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
struct FItemData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
	EItemTypes ItemType{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
	FString ItemName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
	UTexture2D* ItemIcon;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
	uint8 ItemStack = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
	uint8 ItemVolume = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
	EItemPlace ItemPlace = EItemPlace::AROUND;
};	



/// <summary>
/// �Ҹ�����ۺз��� ���� ���� Ŭ����.
/// ���濡 ���� �������� ���� Ŭ����.
/// ��)ź, ������, ����, ����ź��.
/// ���濡 ���� ����ǰ ��� �ۼ��ؾ���.
/// </summary>
UCLASS(abstract)
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
	virtual void DetachmentItem();

	virtual void SetOwnerCharacter(class AC_BasicCharacter* InOwnerCharacter) { OwnerCharacter = InOwnerCharacter; }

	/// <summary>
	/// ItemBar���� ��Ŭ������ ����ϴ� �⺻ ��ȣ�ۿ�.
	/// �ڽĴܰ迡�� �������̵��ؼ� ���.
	/// �������� OwnerCharacter�� �ִ��� �������� �⺻���� �Ǵ��ϰ� ��쿡 ���� �ٸ� ����� ������ ����.
	/// �̸��� ��ȣ�ۿ�(Interaction���� �ٲܱ �����)
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
	virtual void SetItemStack(uint8 ItemStack) PURE_VIRTUAL(AC_Item:SetItemStack, );
	
	/// <summary>
	/// �÷��̾ �������� ���� �� ���Ǵ� �Լ�.
	/// �� Ŭ�������� override�ؼ� �˸°� ���
	/// ���Ŀ� �̸� ���氡��
	/// RemoveItemFromInventory
	/// </summary>
	UFUNCTION(BlueprintCallable)
	virtual void DropItem(AC_BasicCharacter* Character )PURE_VIRTUAL(AC_Item:DropItem, );

	//�������� ������ 1 �÷���.
	UFUNCTION(BlueprintCallable)
	void AddItemStack() { ItemDatas.ItemStack += 1; }
	
	//�������� ������ 1 ������.
	UFUNCTION(BlueprintCallable)
	void DeductItemStack() { ItemDatas.ItemStack -= 1; }

	UFUNCTION(BlueprintCallable)
	virtual bool MoveToInven(AC_BasicCharacter* Character) PURE_VIRTUAL(AC_Item:MoveToInven, return false;);

	UFUNCTION(BlueprintCallable)
	virtual bool MoveToAround(AC_BasicCharacter* Character) PURE_VIRTUAL(AC_Item:MoveToAround, return false;);

	UFUNCTION(BlueprintCallable)
	virtual bool MoveToSlot(AC_BasicCharacter* Character) PURE_VIRTUAL(AC_Item:MoveToSlot, return false;);

	UFUNCTION(BlueprintCallable)
	virtual AC_Item* SpawnItem(AC_BasicCharacter* Character) PURE_VIRTUAL(AC_Item:SpawnItem, return nullptr;);

	void SetItemPlace(EItemPlace InPlace) { ItemDatas.ItemPlace = InPlace; }

	FVector GetGroundLocation(AC_BasicCharacter* Character);

public:
	FItemData GetItemDatas() { return ItemDatas; }

	UFUNCTION(BlueprintCallable)
	uint8 GetVolume() { return Volume; }

	virtual AC_BasicCharacter* GetOwnerCharacter() { return OwnerCharacter; }

private:
	uint8 Volume;
	

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
	EItemTypes MyItemType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
	FString ItemName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
	UTexture2D* ItemIcon;		

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
	uint8 ItemStatck;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
	FItemData ItemDatas;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
	AC_BasicCharacter* OwnerCharacter{};

	
};
// Fill out your copyright notice in the Description page of Project Settings.
