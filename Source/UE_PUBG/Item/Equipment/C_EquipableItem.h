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

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	uint8 Level = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EEquipableItemLevel ItemLevel = EEquipableItemLevel::LV1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName SocketName{};
};
