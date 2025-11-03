#pragma once
// 여러 곳에서 사용하는 공통 타입들 정의
// 이 헤더파일을 고치면 라이브 코딩으로 처리 힘듬.
// 이유는 100개가 넘는 검사가 이루어 지기 때문.
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
//#include "Character/Component/C_AttachableItemMeshComponent.h"
//#include "Item/C_Item.h"
#include "CommonTypes.generated.h"



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
	VEST,
	BACKPACK,
	MAINGUN,
	CONSUMPTIONITEM,
	BULLET,
	THROWABLE,
	ATTACHMENT,
	MELEEWEAPON
};

UENUM(BlueprintType)
enum class EItemPlace : uint8
{
	NONE,
	AROUND,
	INVEN,
	SLOT
};

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
	CONSUMPTIONITEM,
	BULLET
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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
	int32 ItemMaxStack = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
	float ItemVolume = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
	TSubclassOf<class AC_Item> ItemClass{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
	float SpawnProbability = .0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	bool bIsStackable = false;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Sound")
	USoundBase* PickUpSound{};
};

USTRUCT(BlueprintType)
struct FAttachmentTableRow : public FTableRowBase
{
	GENERATED_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPartsName PartType{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAttachmentNames AttachmentName{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AAttachmentActor> AttachmentClass{};
};

/// <summary>
/// 사용하지 않는 구조체
/// 절대 사용하지 말 것.
/// </summary>
//USTRUCT(BlueprintType)
//struct FCommonTypes: public FTableRowBase
//{
//    GENERATED_BODY()
//
//    UPROPERTY(BlueprintReadWrite, EditAnywhere)
//    FString ItemName;
//
//    //UPROPERTY(BlueprintReadWrite, EditAnywhere)
//    //EItemTypes ItemType;
//
//    UPROPERTY(BlueprintReadWrite, EditAnywhere)
//    UTexture2D* ItemBarIcon;
//
//    // ... 나머지 UPROPERTY
//};