// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UE_PUBG/Item/C_Item.h"
#include "GameFramework/Actor.h"
#include "Character/C_BasicCharacter.h"

#include "C_Weapon.generated.h"

enum class EWeaponSlot : uint8;

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
	/// AI 공격 순수 가상함수 (AttackTask의 ExecuteTask 부분에서 사용 예정)
	/// </summary>
	/// <param name="InTargetCharacter"> : 공격 대상 BasicCharacter </param>
	/// <returns> 공격을 할 수 없는 상황이라면 return false </returns>
	virtual bool ExecuteAIAttack(class AC_BasicCharacter* InTargetCharacter) PURE_VIRTUAL(AC_Weapon::ExecuteAIAttack, return false;);

	/// <summary>
	/// 현재 무기로 AI 공격을 하는 중이고 TickTask처리로 필요하다면 해당 함수 사용
	/// </summary>
	/// <param name="InTargetCharacter"> : 공격 대상 BasicCharacter </param>
	/// <param name="DeltaTime"> : DeltaTime </param>
	/// <returns> : TickTask처리가 끝났다면 return false (or TickTask처리가 필요하지 않다면 return false) </returns>
	virtual bool ExecuteAIAttackTickTask(class AC_BasicCharacter* InTargetCharacter, const float& DeltaTime);

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
	virtual bool AttachToHand(class USceneComponent* InParent) PURE_VIRTUAL(AC_Weapon::AttachToHand, return false;);

	/// <summary>
	/// 아이템의 type에 따라 알맞는 WeaponSlot을 반환
	/// </summary>
	/// <returns>WeaponSlot을 반환</returns>
	EWeaponSlot GetWeaponSlot();

public:

	void SetOwnerCharacter(AC_BasicCharacter* InOwnerCharacter) { OwnerCharacter = InOwnerCharacter; }

	FPriorityAnimMontage GetCurDrawMontage() const { return CurDrawMontage; }
	FPriorityAnimMontage GetCurSheathMontage() const { return CurSheathMontage; }

	UTexture2D* GetKillLogTexture() const { return KillLogTexture; }

public:

	/// <summary>
	/// OwnerCharacter의 Pose Transition 모션이 끝났을 때 Delegate를 통해 call back을 받는 함수 (현재 캐릭터의 slot에 장착된 무기만 call back 될 예정) 
	/// </summary>
	virtual void OnOwnerCharacterPoseTransitionFin() PURE_VIRTUAL(AC_Weapon::OnOwnerCharacterPoseTransitionFin, );

public:

	void SetRelativeTranformToInitial() { SetActorRelativeTransform(InitialRelativeTransform); }

protected:
	virtual bool MoveSlotToAround(AC_BasicCharacter* Character, int32 InStack) override;
	virtual bool MoveSlotToInven(AC_BasicCharacter* Character, int32 InStack) override;
	virtual bool MoveSlotToSlot(AC_BasicCharacter* Character, int32 InStack) override;

	virtual bool MoveInvenToAround(AC_BasicCharacter* Character, int32 InStack) override;
	virtual bool MoveInvenToInven(AC_BasicCharacter* Character, int32 InStack) override;
	virtual bool MoveInvenToSlot(AC_BasicCharacter* Character, int32 InStack) override;

	virtual bool MoveAroundToAround(AC_BasicCharacter* Character, int32 InStack) override;
	virtual bool MoveAroundToInven(AC_BasicCharacter* Character, int32 InStack) override;
	virtual bool MoveAroundToSlot(AC_BasicCharacter* Character, int32 InStack) override;


protected:

	// 무기에 따른 각 버튼에 대한 strategy 알고리즘 객체
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TScriptInterface<class II_WeaponButtonStrategy> WeaponButtonStrategy{};

	//class AC_BasicCharacter* OwnerCharacter{};

protected:
	UPROPERTY(BlueprintReadWrite, EditInstanceOnly)
	FPriorityAnimMontage CurDrawMontage{}; // 현재 무기 위치에 해당하는 무기 뽑기 Anim montage

	UPROPERTY(BlueprintReadWrite, EditInstanceOnly)
	FPriorityAnimMontage CurSheathMontage{}; // 현재 무기 위치에 해당하는 무기 집어넣기 Anim montage

private:
	
	FTransform InitialRelativeTransform{};

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UTexture2D* KillLogTexture{};
};


