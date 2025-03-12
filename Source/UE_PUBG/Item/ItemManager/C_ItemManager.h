// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Item/C_Item.h"
#include "C_ItemManager.generated.h"


/// <summary>
/// 아이템의 타입을 정해줘서 인벤토리에서 장착할때 알맞는 UI에 찾아가도록 하고자함.
/// NONE은 돌발상황을 대비해서 만들어둠.
/// 하위클래스에서는 생성자에서 정의하고 있음.
/// </summary>
//UENUM(BlueprintType)
//enum class EItemTypes : uint8
//{
//    NONE,
//    HELMET,
//    VEST,
//    BACKPACK,
//    MAINGUN,
//    MELEEWEAPON,
//    THROWABLE,
//    ATTACHMENT,
//    CONSUMPTIONITEM,
//    BULLET
//};


//USTRUCT(BlueprintType)
//struct FWeaponData : public FBasicItemData
//{
//    GENERATED_BODY()
//
//public:
//    UPROPERTY(EditAnywhere, BlueprintReadWrite)
//    USoundBase* FireSound;  // 총기 발사 사운드
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite)
//    int32 Damage;  // 총기 공격력
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite)
//    int32 AmmoCapacity;  // 탄창 크기
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite)
//    UTexture2D* WeaponSlotImage;  // 아이템 UI 아이콘
//};
/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_ItemManager : public UObject
{
	GENERATED_BODY()

public:
    UC_ItemManager();

    // 아이템 매니저 초기화
    void InitializeItemManager();

    // 아이템을 데이터 테이블에서 불러오고 캐싱
    void LoadItemDataTable();

public:
    // 아이템을 이름으로 조회
    FItemData* GetItemData(FName ItemCode);

    // 총기 데이터 정보 가져오기
    //FWeaponData* GetWeaponData(FName WeaponName);
protected:
    UPROPERTY(EditAnywhere, Category = "Item Data")
    UDataTable* GeneralItemTable;

    // 총기 데이터 테이블
    //UPROPERTY(EditAnywhere, Category = "Weapon Data")
    //UDataTable* WeaponItemTable;

private:
    // 아이템을 이름을 키로 하여 캐싱
    TMap<FName, FItemData> ItemDataCache{};
};
