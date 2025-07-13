// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/C_Item.h"
#include "TrainingLevel/Tutorial/TutorialStageChecker/C_TutorialStageChecker.h"
#include "C_Item_Bullet.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EBulletType : uint8
{
	FIVEMM,
	SEVENMM,
	NONE
};
UCLASS()
class UE_PUBG_API AC_Item_Bullet : public AC_Item
{
	GENERATED_BODY()
public:
	AC_Item_Bullet();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	bool Interaction(AC_BasicCharacter* Character) override;
	bool MoveAroundToInven(AC_BasicCharacter* Character, int32 InStack) override;
	bool MoveInvenToAround(AC_BasicCharacter* Character, int32 InStack) override;

	/// <summary>
	/// 이 아이템의 주인이 player인 경우 실행하여
	/// 현재 총을 들고 있는지 체크
	/// 현재 들고 있는 총과 습득한 총알의 Tpye이 일치하면
	/// AmmoWidget의 Update를 진행.
	/// </summary>
	/// <param name="InOwnerPlayer">이 아이템의 주인</param>
	void UpdateLeftAmmoWidget(class AC_Player* InOwnerPlayer);

public:

	static FName GetBulletTypeName(EBulletType InBulletType);

protected:
	UPROPERTY(BluePrintReadWrite, EditAnywhere)

	EBulletType CurBulletType;

private:

	static const TMap<EBulletType, FName> BulletTypeNameMap;

public:

	static FTutorialStageGoalCheckerDelegate WeaponTutorialDelegate;
	
};


