#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UE_PUBG/Utility/CommonTypes.h"

#include "C_Item.generated.h"

// Training Ground에서 사용할 Delegate 선언
DECLARE_DELEGATE_OneParam(FRespawnableItemPickedUp, class AC_Item*);

/// <summary>
/// AttachableItem PartsName Enum class
/// C_Gun에서 부착 가능 부위
/// </summary>
//UENUM(BlueprintType)
//enum class EPartsName : uint8
//{
//	NONE,
//	SCOPE,
//	MUZZLE,
//	GRIP,
//	MAGAZINE,
//	GUNSTOCK,
//	MAX
//};

/// <summary>
/// Attachment 아이템 이름들
/// </summary>
//UENUM(BlueprintType)
//enum class EAttachmentNames : uint8
//{
//	REDDOT,
//	SCOPE4,
//	SCOPE8,
//	VERTGRIP,
//	EXTENDMAG,
//	QUICKMAG,
//	COMPENSATOR,
//	SUPPRESSOR,
//	MAX
//};
/// <summary>
/// 현재 제대로 사용하지는 않고 있음.
/// </summary>

///// <summary>
///// 아이템의 타입을 정해줘서 인벤토리에서 장착할때 알맞는 UI에 찾아가도록 하고자함.
///// NONE은 돌발상황을 대비해서 만들어둠.
///// 하위클래스에서는 생성자에서 정의하고 있음.
///// </summary>
//UENUM(BlueprintType)
//enum class EItemTypes : uint8
//{
//	NONE,
//	HELMET,
//	VEST,
//	BACKPACK,
//	MAINGUN,
//	CONSUMPTIONITEM,
//	BULLET,
//	THROWABLE,
//	ATTACHMENT,
//	MELEEWEAPON
//};

//UENUM(BlueprintType)
//enum class EItemPlace : uint8
//{
//	NONE,
//	AROUND,
//	INVEN,
//	SLOT
//};

//UENUM(BlueprintType)
//enum class EItemNames : uint8
//{
//	NONE,
//	HELMET,
//	ARMORE,
//	BACKPACK,
//	MAINGUN,
//	MELEEWEAPON,
//	THROWABLE,
//	ATTACHMENT,
//	CONSUMPTIONITEM,
//	BULLET
//};

//USTRUCT(BlueprintType)
//struct FItemData : public FTableRowBase
//{
//	GENERATED_BODY()
//
//	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
//	FString ItemName{};
//
//	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
//	EItemTypes ItemType{};
//
//	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
//	UTexture2D* ItemBarIcon = nullptr;
//
//	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
//	UTexture2D* ItemSlotImage = nullptr;
//
//	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
//	int32 ItemMaxStack = 0;
//
//	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
//	float ItemVolume = 0;
//
//	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
//	TSubclassOf<class AC_Item> ItemClass{};
//
//	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
//	float SpawnProbability = .0f;
//};

/// <summary>
/// 모든 아이템은 이 클래스를 상속받아야함.
/// 틱을	사용하기 위해서는 SetActorEnableTick(true)로 설정해줘야함.
/// 틱 사용이 끝났다면 SetActorEnableTick(false)로 꺼줘야함.
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

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void InitializeItem(FName NewItemCode);

	//아이템의 장착을 해제하고 땅에 떨구게 해줌.
	UFUNCTION(BlueprintCallable)
	virtual void DetachItem();

	virtual void SetOwnerCharacter(class AC_BasicCharacter* InOwnerCharacter) { OwnerCharacter = InOwnerCharacter; }

	/// <summary>
	/// ItemBar에서 우클릭으로 사용하는 기본 상호작용.
	/// 자식단계에서 오버라이드해서 사용.
	/// 아이템의 OwnerCharacter가 있는지 없는지를 기본으로 판단하고 경우에 따라 다른 기능을 실행할 예정.)
	/// 상호작용으로 바꾼다면. OwnerCharcter의 여부에 따라, 아이템에 따라 UseItem의 이름을 다른 범용적인 함수에 사용가능.
	/// </summary>
	UFUNCTION(BlueprintCallable)
	virtual bool Interaction(AC_BasicCharacter* Character) PURE_VIRTUAL(AC_Item::Interaction, return false;);

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
	void SetItemStack(int32 inItemStack);// { ItemDatas.ItemCurStack = inItemStack; }
	
	/// <summary>
	/// 플레이어가 아이템을 버릴 때 사용되는 함수.
	/// 각 클래스에서 override해서 알맞게 사용
	/// 추후에 이름 변경가능
	/// RemoveItemFromInventory
	/// TODO : InStack초기값 설정에 관해 다시 보기
	/// </summary>
	UFUNCTION(BlueprintCallable)
	virtual void DropItem(AC_BasicCharacter* Character); //PURE_VIRTUAL(AC_Item:DropItem, );

	//아이템의 스택을 1 올려줌.
	UFUNCTION(BlueprintCallable)
	void AddItemStack() { ItemCurStack += 1; }

	//아이템의 스택을 1 내려줌.
	UFUNCTION(BlueprintCallable)
	void DeductItemStack() { ItemCurStack -= 1; }

	/// <summary>
	/// 아이템이 Inven으로 보낼 때 사용하면 현재 Place에 맞게  작동
	/// </summary>
	/// <param name="Character"></param>
	/// <returns></returns>
	UFUNCTION(BlueprintCallable)
	bool MoveToInven(AC_BasicCharacter* Character, int32 InStack);
	
	/// <summary>
	/// 아이템을 Around로 보낼 때 사용하면 현재 Place에 맞게 작동.
	/// </summary>
	/// <param name="Character"></param>
	/// <returns></returns>
	UFUNCTION(BlueprintCallable)
	bool MoveToAround(AC_BasicCharacter* Character, int32 InStack);
	
	/// <summary>
	/// 아이템을 Slot으로 보낼 때 Place에 따라 작동.
	/// </summary>
	/// <param name="Character"></param>
	/// <returns></returns>
	UFUNCTION(BlueprintCallable)
	bool MoveToSlot(AC_BasicCharacter* Character, int32 InStack);

	/// <summary>
	/// 현재 아이템과 동일한 아이템을 매개변수 Character의 발 아래에 spawn하는 함수.
	/// </summary>
	/// <param name="Character"></param>
	/// <returns></returns>
	UFUNCTION(BlueprintCallable)
	virtual AC_Item* SpawnItem(AC_BasicCharacter* Character);
	
	/// <summary>
	/// 아이템을 
	/// </summary>
	/// <param name="DivideNum">나눌 아이템의 갯수</param>
	/// <param name="Character">스폰 위치 데이터를 주는 캐릭터(보통 플레이어)</param>
	/// <param name="bIsActorEnableCollision">나누어준 아이템의 ActorEnableCollision을 킬지 말지. 버릴 경우에는 키고(true) 줍는 경우에는 끈다(false)</param>
	/// <returns>새로 생성한 나뉘어진 아이템, nullptr이면 CurStack이 1이여서 못나누는 상황</returns>
	UFUNCTION(BlueprintCallable)
	AC_Item* DividItemSpawn(int32 DivideNum, AC_BasicCharacter* Character, bool bIsActorEnableCollision);

	/// <summary>
	/// 아이템을 파괴하는 함수.
	/// Destroy함수 대신 사용 할 것.
	/// 내부적으로 Destroy를 사용하고 있음.
	/// 아이템 매니저의 컨테이너에서 삭제해주고,
	/// 아이템을 Destroy하는 함수.
	/// </summary>
	/// <param name="bNetForce"></param>
	/// <param name="bShouldModifyLevel"></param>
	/// <returns></returns>
	virtual bool DestroyItem(bool bNetForce = false, bool bShouldModifyLevel = true);

protected:
	//MoveTo~에 사용되는 9개 함수.
	virtual bool   MoveSlotToAround(AC_BasicCharacter* Character, int32 InStack);
	virtual bool    MoveSlotToInven(AC_BasicCharacter* Character, int32 InStack);
	virtual bool     MoveSlotToSlot(AC_BasicCharacter* Character, int32 InStack);

	virtual bool  MoveInvenToAround(AC_BasicCharacter* Character, int32 InStack);
	virtual bool   MoveInvenToInven(AC_BasicCharacter* Character, int32 InStack);
	virtual bool    MoveInvenToSlot(AC_BasicCharacter* Character, int32 InStack);

	virtual bool MoveAroundToAround(AC_BasicCharacter* Character, int32 InStack);
	virtual bool  MoveAroundToInven(AC_BasicCharacter* Character, int32 InStack);
	virtual bool   MoveAroundToSlot(AC_BasicCharacter* Character, int32 InStack);

public:
	FName GetItemCode() const { return ItemCode; }

	const FItemData* GetItemDatas() const { return ItemDataRef; }

	/// <summary>
	/// 아이템의 현재 갯수 반환
	/// </summary>
	/// <returns>아이템 갯수</returns>
	int32 GetItemCurStack() { return ItemCurStack; }

	UFUNCTION(BlueprintCallable)
	EItemPlace GetItemPlace() { return ItemPlace; }

	UFUNCTION(BlueprintCallable)
	EItemTypes GetItemType() const { return ItemDataRef->ItemType; }

	UFUNCTION(BlueprintCallable)
	FString GetItemName() const { return ItemDataRef->ItemName; }

	/// <summary>
	/// 아이템 하나의 Volume
	/// </summary>
	/// <returns></returns>
	UFUNCTION(BlueprintCallable)
	float GetOnceVolume() { return ItemDataRef->ItemVolume; }

	UFUNCTION(BlueprintCallable)
	float GetItemAllVolume() { return ItemDataRef->ItemVolume * ItemCurStack; }

	virtual AC_BasicCharacter* GetOwnerCharacter() { return OwnerCharacter; }

	void SetItemPlace(EItemPlace InPlace) { ItemPlace = InPlace; }

	//캐릭터의 밑바닥을 라인 트레이스로 location을 반환해줌.
	FVector GetGroundLocation(AC_BasicCharacter* Character);

	/// <summary>
	/// 아웃라인을 그리고 꺼주는 함수.
	/// </summary>
	/// <param name="bEnable"></param>
	void SetOutlineEffect(bool bEnable);

	USoundBase* GetPickUpSound() const { return PickUpSound; }

	// 현재 구조로는 아이템을 만들 때 마다 해당 블루프린트에 정의된 GetItemMeshComp()를 오버라이드 해야함.
	UFUNCTION(BlueprintImplementableEvent)
	USceneComponent* GetItemMeshComp();

	float GetUseProgress() const { return UseProgress; }

	void SetUseProgress(float NewProgress)
	{
		UseProgress = NewProgress;
	}
protected:

	/// <summary>
	/// 아이템의 고유 코드, 현재는 블프에서 정의 할 예정
	/// ClickUp에 공유한 PUBG파일의 Item의 Dictionaries에 나온 Code를 사용.
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FName ItemCode = "NONE";  

	/// <summary>
	/// 아이템의 현재 위치 상태
	/// </summary>
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
	EItemPlace ItemPlace = EItemPlace::AROUND;

	/// <summary>
	/// 현재 아이템의 갯수
	/// </summary>
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
	int32 ItemCurStack = 0;

	/// <summary>
	/// Time Progress Bar percent
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float UseProgress = 0.f;

	/// <summary>
	/// 아이템 데이터 구조체
	/// </summary>
	const FItemData* ItemDataRef = nullptr;

	/// <summary>
	/// 이 아이템을 들고 있는 Character
	/// </summary>
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
	AC_BasicCharacter* OwnerCharacter{};
protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Sound")
	USoundBase* PickUpSound{};

public:

	// Respawn 처리되어야 할 아이템일 경우 해당 Delegate를 통해 파밍되었을 시, 파밍 이 후 Respawn 처리
	FRespawnableItemPickedUp OnRespawnableItemPickedUp{};

private:

	// Outline effect처리에 쓰일 PrimitiveComponent들 모음
	TArray<UPrimitiveComponent*> PrimitiveComponents{};
	
};

