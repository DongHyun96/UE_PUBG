// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/C_Item.h"
#include "C_EquipableItem.generated.h"

UENUM(BlueprintType)
enum class EEquipableItemLevel : uint8
{
	LV1,
	LV2,
	LV3
};

/**
 * 
 */
UCLASS()
class UE_PUBG_API AC_EquipableItem : public AC_Item
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AC_EquipableItem();



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	//virtual void Interaction(class AC_BasicCharacter* character) override;

	virtual bool Interaction(AC_BasicCharacter* Character);

	//bool MoveToAround(AC_BasicCharacter* Character) override;

	//void SetRelativeTranformToInitial() { SetActorRelativeTransform(InitialRelativeTransform); }

	virtual void AttachToSocket(class AC_BasicCharacter* InParent);

	void SetItemLevel(EEquipableItemLevel inItemLevel) { ItemLevel = inItemLevel; }

	EEquipableItemLevel GetItemLevel() const { return ItemLevel; }

	FName GetSocketName() const { return SocketName; }
	
public:
	/// <summary>
	/// Equipable Item에 Damage 적용
	/// </summary>
	/// <param name="DamageAmount"> : Damage 총량 (각 Lv1에 따른 Damage재계산 처리는 해당 함수안에서 처리될 예정) </param>
	/// <returns> : Damage를 줄 수 없는 EquipableItem의 경우 return false </returns>
	virtual bool TakeDamage(float DamageAmount);

	/// <summary>
	/// 각 Level에 따른 Damage 감소 factor 구하기
	/// </summary>
	/// <returns> : Damage 감소 Factor </returns>
	virtual float GetDamageReduceFactor() const;

	UFUNCTION(BlueprintCallable)
	float GetCurDurabilityRate() const {return CurDurability / DURABILITY_MAX; }
	
protected:
	bool MoveSlotToAround(AC_BasicCharacter* Character, int32 InStack) override;

	bool MoveAroundToSlot(AC_BasicCharacter* Character, int32 InStack) override;


protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	uint8 Level = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EEquipableItemLevel ItemLevel = EEquipableItemLevel::LV1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName SocketName{};

protected: // Equipable 내구도 관련

	static const float DURABILITY_MAX;
	
	float CurDurability = DURABILITY_MAX; // 현재 내구도

	//UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	//class UProgressBar* DurabilityBar = nullptr;
};


