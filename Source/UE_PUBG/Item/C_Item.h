
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "C_Item.generated.h"

/// <summary>
/// AttachableItem PartsName Enum class
/// C_Gun에서 부착 가능 부위
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
/// Attachment 아이템 이름들
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
/// 현재 제대로 사용하지는 않고 있음.
/// </summary>

/// <summary>
/// 아이템의 타입을 정해줘서 인벤토리에서 장착할때 알맞는 UI에 찾아가도록 하고자함.
/// NONE은 돌발상황을 대비해서 만들어둠.
/// 하위클래스에서는 생성자에서 정의하고 있음.
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
/// 소모아이템분류를 위한 상위 클래스.
/// 가방에 들어가는 아이템을 위한 클래스.
/// 예)탄, 진통제, 구상, 수류탄등.
/// 가방에 들어가는 소지품 목록 작성해야함.
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

	//아이템의 장착을 해제하고 땅에 떨구게 해줌.
	UFUNCTION(BlueprintCallable)
	virtual void DetachmentItem();

	virtual void SetOwnerCharacter(class AC_BasicCharacter* InOwnerCharacter) { OwnerCharacter = InOwnerCharacter; }

	/// <summary>
	/// ItemBar에서 우클릭으로 사용하는 기본 상호작용.
	/// 자식단계에서 오버라이드해서 사용.
	/// 아이템의 OwnerCharacter가 있는지 없는지를 기본으로 판단하고 경우에 따라 다른 기능을 실행할 예정.
	/// 이름을 상호작용(Interaction으로 바꿀까도 고민중)
	/// 상호작용으로 바꾼다면. OwnerCharcter의 여부에 따라, 아이템에 따라 UseItem의 이름을 다른 범용적인 함수에 사용가능.
	/// </summary>
	UFUNCTION(BlueprintCallable)
	virtual bool Interaction(AC_BasicCharacter* Character) PURE_VIRTUAL(AC_Item::Interaction, return false;);

	virtual bool UseItem() PURE_VIRTUAL(AC_Item::UseItem, return false;);

	/// <summary>
	/// 플레이어가 아이템을 습득할때 사용되는 함수.
	/// 각 클래스에서 override해서 클래스에 맞게 사용.
	/// 추후에 이름 변경 가능
	/// AddItemToInventory, PickupItem
	/// </summary>
	UFUNCTION(BlueprintCallable)
	virtual void PickUpItem(AC_BasicCharacter* Character) PURE_VIRTUAL(AC_Item:PickUpItem, );

	/// <summary>
	/// Around -> MyItems로 아이템이 이동할 때, 이미 해당 아이템이 존재할때 ItemStack을 C_Item내에서 Set하는 함수.
	/// </summary>
	UFUNCTION(BlueprintCallable)
	virtual void SetItemStack(uint8 ItemStack) PURE_VIRTUAL(AC_Item:SetItemStack, );
	
	/// <summary>
	/// 플레이어가 아이템을 버릴 때 사용되는 함수.
	/// 각 클래스에서 override해서 알맞게 사용
	/// 추후에 이름 변경가능
	/// RemoveItemFromInventory
	/// </summary>
	UFUNCTION(BlueprintCallable)
	virtual void DropItem(AC_BasicCharacter* Character )PURE_VIRTUAL(AC_Item:DropItem, );

	//아이템의 스택을 1 올려줌.
	UFUNCTION(BlueprintCallable)
	void AddItemStack() { ItemDatas.ItemStack += 1; }
	
	//아이템의 스택을 1 내려줌.
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
