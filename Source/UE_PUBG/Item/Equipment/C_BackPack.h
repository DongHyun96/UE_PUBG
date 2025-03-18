// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Equipment/C_EquipableItem.h"
#include "GameFramework/Actor.h"
#include "C_BackPack.generated.h"

/**
 * 배낭 (Level 1): 소지 용량 +150
 * 배낭 (Level 2): 소지 용량 +200
 * 배낭 (Level 3)(보급 전용 3레벨 배낭도 포함): 소지 용량 +250
 */


UCLASS()
class UE_PUBG_API AC_BackPack : public AC_EquipableItem
{
	GENERATED_BODY()

public:

	AC_BackPack();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//virtual void Interaction(class AC_BasicCharacter* character) override;

	//virtual bool Interaction() override;


	void AttachToSocket(class AC_BasicCharacter* InParent) override;

	/// <summary>
	/// 장착중인 가방을 내려놓음.
	/// 내려 놓는 위치는 발정도.
	/// </summary>
	/// <param name="character"></param>
	UFUNCTION(BlueprintCallable)
	void DetachToSocket(AC_BasicCharacter* character);

	uint8 GetLevel() { return Level; }

	// Skeletal mesh for the backpack
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Components")
	//UStaticMeshComponent* BackpackMesh;
	//USkeletalMeshComponent* BackpackMesh;
	bool Interaction(AC_BasicCharacter* Character) override;
	
	//void PickUpItem(AC_BasicCharacter* Character) override;

	bool LegacyMoveToAround(AC_BasicCharacter* Character) override;

	bool LegacyMoveToSlot(AC_BasicCharacter* Character) override;
protected:
	bool MoveSlotToAround(AC_BasicCharacter* Character, int32 InStack) override;

	bool MoveAroundToSlot(AC_BasicCharacter* Character, int32 InStack) override;

protected:
	//UPROPERTY(BlueprintReadWrite, EditAnywhere)
	//uint8 Level;

};


