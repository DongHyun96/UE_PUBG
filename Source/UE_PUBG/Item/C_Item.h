
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
	GUNSTOCK
};
/// <summary>
/// 현재 제대로 사용하지는 않고 있음.
/// </summary>
USTRUCT(BlueprintType)
struct FItemData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
	FString ItemName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
	UTexture2D* ItemIcon;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
	uint8 ItemNums;
};
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
	ARMORE,
	BACKPACK,
	MAINGUN,
	MELEEWEAPON,
	THROWABLE,
	ATTACHMENT,
	CONSUMPTIONITEM
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

	virtual void Interaction(class AC_BasicCharacter* character) PURE_VIRTUAL(AC_Item::Interaction, );

	//아이템의 장착을 해제하고 땅에 떨구게 해줌.
	UFUNCTION(BlueprintCallable)
	virtual void DetachmentItem();

	virtual void SetOwnerCharacter(AC_BasicCharacter* InOwnerCharacter) { OwnerCharacter = InOwnerCharacter; }

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
	uint8 ItemNums;  // uint8 -> 0 ~ 255까지밖에 안됨 (탄 때문에 범위 더 늘려줘야 함)

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
	AC_BasicCharacter* OwnerCharacter{};
};
// Fill out your copyright notice in the Description page of Project Settings.
