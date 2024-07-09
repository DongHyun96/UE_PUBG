// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UE_PUBG/Item/C_Item.h"
#include "GameFramework/Actor.h"
#include "C_Weapon.generated.h"

UCLASS(Abstract)
class UE_PUBG_API AC_Weapon : public AC_Item
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AC_Weapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public: /* 각 Key에 따른 무기 작동 */

	/// <summary>
	/// B Key weapon method
	/// </summary>
	/// <returns> 실행 중 실패하거나, 매칭되는 액션이 없을 때 return false </returns>
	bool ExecuteBKey();

	/// <summary>
	/// R Key weapon method
	/// </summary>
	/// <returns> 실행 중 실패하거나, 매칭되는 액션이 없을 때 return false </returns>
	bool ExecuteRKey();

	/// <summary>
	/// MLB Started weapon method
	/// </summary>
	/// <returns> 실행 중 실패하거나, 매칭되는 액션이 없을 때 return false </returns>
	bool ExecuteMlb_Started();

	/// <summary>
	/// MLB OnGoing weapon method
	/// </summary>
	/// <returns> 실행 중 실패하거나, 매칭되는 액션이 없을 때 return false </returns>
	bool ExecuteMlb_OnGoing();

	/// <summary>
	/// MLB Completed weapon method
	/// </summary>
	/// <returns> 실행 중 실패하거나, 매칭되는 액션이 없을 때 return false </returns>
	bool ExecuteMlb_Completed();

	/// <summary>
	/// MRB started weapon method
	/// </summary>
	/// <returns> 실행 중 실패하거나, 매칭되는 액션이 없을 때 return false </returns>
	bool ExecuteMrb_Started();

	/// <summary>
	/// MRB OnGoing weapon method
	/// </summary>
	/// <returns> 실행 중 실패하거나, 매칭되는 액션이 없을 때 return false </returns>
	bool ExecuteMrb_OnGoing();

	/// <summary>
	/// MRB Completed weapon method
	/// </summary>
	/// <returns> 실행 중 실패하거나, 매칭되는 액션이 없을 때 return false </returns>
	bool ExecuteMrb_Completed();

public:

	/// <summary>
	/// 무기집에 무기 붙이기
	/// </summary>
	/// <returns> 붙이기를 실패하거나 붙일 무기집이 없다면 return false </returns>
	virtual bool AttachToHolster(class USceneComponent* InParent) PURE_VIRTUAL(AC_Weapon::AttachToHolster, return false;);

	/// <summary>
	/// 손에 장착하기
	/// </summary>
	/// <returns> 무기를 손에 붙이지 못하였다면 return false </returns>
	virtual bool AttachToHand(class USceneComponent* InParent) PURE_VIRTUAL(AC_Weapon::AttachToHolster, return false;);


public:

	void SetOwnerCharacter(class AC_BasicCharacter* InOwnerCharacter) { OwnerCharacter = InOwnerCharacter; }

	class UAnimMontage* GetCurDrawMontage() const { return CurDrawMontage; }
	class UAnimMontage* GetCurSheathMontage() const { return CurSheathMontage; }

protected:

	// 무기에 따른 각 버튼에 대한 strategy 알고리즘 객체
	class II_WeaponButtonStrategy* WeaponButtonStrategy{};

	class AC_BasicCharacter* OwnerCharacter{};

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UAnimMontage* CurDrawMontage{}; // 현재 무기 위치에 해당하는 무기 뽑기 Anim montage

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UAnimMontage* CurSheathMontage{}; // 현재 무기 위치에 해당하는 무기 집어넣기 Anim montage

};
