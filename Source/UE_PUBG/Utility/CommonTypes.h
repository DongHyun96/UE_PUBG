#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
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