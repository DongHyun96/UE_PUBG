// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Character/Component/C_EquippedComponent.h"
#include "C_BTTaskSwapWeapon.generated.h"

enum class EThrowableType : uint8;
enum class EWeaponSlot : uint8;

struct FThrowableSwapExecutionDescriptor
{
	EThrowableType InChangeTo{};						// Swap 시도 하려는 목표 ThrowableType
	EThrowableType CurrentThrowableSlotType{};			// 현재 Slot에 장착된 ThowableType (아무것도 장착된게 없다면 MAX)

	class AC_Enemy*				Enemy{};
	class UC_BehaviorComponent* BehaviorComponent{};

	class AC_ThrowingWeapon* InvenGrenade{};			// Inven에 들어있는 Grenade (없다면 nullptr)
	class AC_ThrowingWeapon* InvenFlashBang{};			// ..
};


UCLASS()
class UE_PUBG_API UC_BTTaskSwapWeapon : public UBTTaskNode
{
	GENERATED_BODY()

public:

	UC_BTTaskSwapWeapon();

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:

	/// <summary>
	/// <para> ExecuteTask 내의 Boiler-plate 코드 부분 정리 </para>
	/// <para> Slot에 해당 무기가 존재할 때 사용할 것 </para>
	/// </summary>
	/// <param name="InChangeTo"> : 손에 들려고 하는 무기 slot 종류 </param>
	/// <param name="Enemy"></param>
	/// <param name="BehaviorComponent"></param>
	/// <returns> : ExecuteTask에 대한 결과값 </returns>
	EBTNodeResult::Type ExecuteWeaponSwapRoutine(EWeaponSlot InChangeTo, class AC_Enemy* Enemy, class UC_BehaviorComponent* BehaviorComponent);

private:

	EBTNodeResult::Type ExecuteThrowableSwapRoutine(const FThrowableSwapExecutionDescriptor& Desc);

	/// <summary>
	/// <para> ExecuteTask 내에서 return 처리하는 BoilerPlate 코드 정리 </para>
	/// <para> 주의 : InProgress일 경우, Enemy와 InChangeTo Type parameter도 같이 보낼 것 </para>
	/// </summary>
	/// <param name="ReturnType"> : ExecuteTask 내에서 return하려는 type </param>
	/// <param name="BehaviorComponent"> : Enemy BehaviorComponent </param>
	EBTNodeResult::Type ExecuteTaskReturnRoutine(const EBTNodeResult::Type& ReturnType, UC_BehaviorComponent* BehaviorComponent, AC_Enemy* Enemy = nullptr, EWeaponSlot InChangeTo = EWeaponSlot::NONE);

private:

	// class AC_Enemy* OwnerEnemy{};
	// class UC_BehaviorComponent* OwnerBehaviorComponent{};

	// float TotalTime{};

	TMap<class AC_Enemy*, EWeaponSlot> SwapTargetWeaponSlotMap{};
	TMap<AC_Enemy*, float> TotalTimeMap{};
	
	// EWeaponSlot SwapTargetWeaponSlot{};
};
