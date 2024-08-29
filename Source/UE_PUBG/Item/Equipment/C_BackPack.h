// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Equipment/C_EquipableItem.h"
#include "GameFramework/Actor.h"
#include "C_BackPack.generated.h"

/**
 * �賶 (Level 1): ���� �뷮 +150
 * �賶 (Level 2): ���� �뷮 +200
 * �賶 (Level 3)(���� ���� 3���� �賶�� ����): ���� �뷮 +250
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

	virtual void Interaction(class AC_BasicCharacter* character) override;

	void AttachToSocket(class USceneComponent* InParent);

	void DetachToSocket(class AC_BasicCharacter* character);
	uint8 GetLevel() { return Level; }

	// Skeletal mesh for the backpack
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* BackpackMesh;

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	uint8 Level;

private:
};
