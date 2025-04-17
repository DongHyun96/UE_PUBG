// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item/C_Item.h"
#include "Item/Weapon/Gun/C_Gun.h"
#include "C_ItemManager.generated.h"

UCLASS()
class UE_PUBG_API AC_ItemManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AC_ItemManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    // 아이템 매니저 초기화
    void InitializeItemManager();

    // 아이템을 데이터 테이블에서 불러오고 캐싱
    void LoadItemDataTable();

    /// <summary>
    /// 아이템 매니저에서 아이템을 스폰하는 함수.
    /// </summary>
    /// <param name="ItemCode">스폰하고자 하는 아이템의 code</param>
    /// <param name="Location">아이템의 위치</param>
    /// <param name="Stack">아이템의 CurStack</param>
    /// <returns>스폰한 아이템, nullptr이면 없는 코드</returns>
	UFUNCTION(BlueprintCallable)
    AC_Item* SpawnItem(FName ItemCode, FVector Location, int32 Stack = 0);

public:
    // 아이템을 이름으로 조회
    FItemData* GetItemData(FName ItemCode);

    // 총기 데이터 정보 가져오기
    FGunData* GetGunData(FName ItemCode);
protected:
    UPROPERTY(EditAnywhere, Category = "Item Data")
    UDataTable* GeneralItemTable;

    // 총기 데이터 테이블
    UPROPERTY(EditAnywhere, Category = "Weapon Data")
    UDataTable* GunItemTable;

private:
    // 아이템을 이름을 키로 하여 캐싱
    TMap<FName, FItemData> GeneralItemDataCache{};

    TMap<FName, FGunData> GunItemDataCache{};
};
