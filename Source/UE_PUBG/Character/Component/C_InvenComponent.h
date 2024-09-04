// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_InvenComponent.generated.h"

UENUM(BlueprintType)
enum class EBackPackLevel : uint8
{
	LV0,
	LV1,
	LV2,
	LV3
};

/// <summary>
/// C_InvenComponent는 0~3레벨까지 존재하며 이는 가방의 레벨에 따라서 변화한다.
///가방은 자동장착되며 레벨을 올리는데는 제한이 없지만
///다운그레이드의 경우 다운되는 레벨의 무게를 초과하는 인벤상태라면
///불가능하다.
/// EquipmentSystem을 Blueprint에서 만들어서 거기서 InvenComponent와 EquippedComponent를 사용해서 인벤시스템와 UI를 제작.
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

	//가방교체, 아이템획득시 해당함수로 용량이 충분한지 확인하여 T or F를 반환. 블루프린트에서 사용 가능하도록 열어줘야 할 수 있음.
	
	//UFUNCTION(BlueprintCallable)
	//uint16은 해당 매크로가 지원하지 않는다. uint8, uint32는 지원한다.
	bool CheckVolume(uint16 volume);

	UFUNCTION(BlueprintCallable)
	bool CheckMyBackPack(class AC_BackPack* backpack);

	//UFUNCTION(BlueprintCallable)
	void Interaction(class AC_Item* wilditem);

	uint16 CheckBackPackVolume(uint32 backpacklevel);
	uint16 CheckBackPackVolume(EBackPackLevel backpacklevel);

	UFUNCTION(BlueprintCallable)
	void DroppingItem(AC_Item* myitem);

	UFUNCTION(BlueprintCallable)
	void RemoveBackPack();

	UFUNCTION(BlueprintCallable)
	void EquippedBackPack(AC_BackPack* backpack);

public://Getter and Seter
	EBackPackLevel GetCurBackPackLevel() { return CurBackPackLevel; } 
	//EBackPackLevel SetCurBackPackLevel(uint8 level) { CurBackPackLevel = (EBackPackLevel)level; }

	uint32 GetMaxVolume() { return MaxVolume; }
	uint32 GetCurVolume() { return CurVolume; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	class AC_BackPack* GetMyBackPack() { return MyBackPack; }

	

	TArray<class AC_Item*>& GetNearItems() { return NearItems; }

protected:
	AC_BasicCharacter* OwnerCharacter{};

	uint32 MaxVolume = 70;
	uint32 CurVolume =  0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EBackPackLevel CurBackPackLevel = EBackPackLevel::LV0;

	EBackPackLevel PreBackPackLevel = EBackPackLevel::LV0;

	/// <summary>
	/// 기본 : Ui에서 현재 내 아이템목록을 보여주기 위함.
	/// 추가 : 죽었을때 떨굴 내 아이템 목록.(총이나 가방같이 장착아이템들도 떨구어야하는데 이걸 죽었을때 한번에 떨구기 위한 방법은 2가지로 생각. 
	/// 첫번째는 일일이 떨꾸어주는것. 
	/// 두번째는 MyItem에 다 넣고 한번에 떨구는 것.
	/// </summary>
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<AC_Item*> MyItems;

	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "NearItmes Array")
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<AC_Item*> NearItems;
protected:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	AC_BackPack* MyBackPack {};

private:

	
};
