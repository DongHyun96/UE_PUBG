// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Item/C_Item.h"
#include "Character/C_BasicCharacter.h"
#include "C_ConsumableItem.generated.h"


// TODO : Cancel 기능 넣어야 함

UENUM(BlueprintType)
enum class EConsumableItemState : uint8
{
	IDLE,
	ACTIVATING,
	ACTIVATE_COMPLETED,
	USED
};

UCLASS(Abstract)
class UE_PUBG_API AC_ConsumableItem : public AC_Item
{
	GENERATED_BODY()

public:

	AC_ConsumableItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	/// <summary>
	/// 소모 아이템 사용하기 시작
	/// </summary>
	/// <param name="ItemUser"> : 아이템 사용 Character </param>
	/// <returns> : 사용 시작이 적절히 되었다면 return true </returns>
	bool StartUsingConsumableItem(class AC_BasicCharacter* InItemUser);

	/// <summary>
	/// 사용 활성화 시도 중 Item 사용 취소 시도
	/// </summary>
	/// <returns> : 정상적으로 취소되었다면 return true </returns>
	bool CancelActivating();

	bool Interaction(AC_BasicCharacter* Character) override;

	bool MoveToInven(AC_BasicCharacter* Character) override;

	bool MoveToSlot(AC_BasicCharacter* Character) override;

	bool MoveToAround(AC_BasicCharacter* Character) override;


protected:

	/// <summary>
	/// 각 아이템 별 사용 시작 가능한 조건인지 check
	/// </summary>
	virtual bool IsAvailableToStartUsing(class AC_BasicCharacter* InItemUser) PURE_VIRTUAL(AC_ConsumableItem::IsAvailableToStartUsing, return false;);

	/// <summary>
	/// 각 아이템 별 사용 시작 시 호출될 Template method
	/// </summary>
	virtual void OnStartUsing() PURE_VIRTUAL(AC_ConsumableItem::OnStartUsing, );

	/// <summary>
	/// 각 아이템 별 Activating이 끝났을 시 호출될 Template method
	/// </summary>
	virtual void OnActivatingFinish() PURE_VIRTUAL(AC_ConsumableItem::OnActivatingFinish, );
	
protected:

	/// <summary>
	/// 아이템 발동 시작과 실제 발동 사이의 사용대기시간에 호출될 Tick Activating State Template method
	/// </summary>
	virtual void HandleActivatingState() PURE_VIRTUAL(AC_ConsumableItem::HandleActivatingState, );

	/// <summary>
	/// 아이템 사용 시간까지 방해 받지 않고 모두 시간을 소모했다면, 사용하기 적용(Tick ActivatedCompleted Template method)
	/// </summary>
	virtual void HandleActivateCompletedState() PURE_VIRTUAL(AC_ConsumableItem::HandleActivateCompletedState, );

	/// <summary>
	/// 아이템 발동 이전에 Cancel 당했을 시 호출될 Template method
	/// </summary>
	virtual void OnCancelActivating() PURE_VIRTUAL(AC_ConsumableItem::OnCancelActivating, );

public: // getters and setters

	EConsumableItemState GetConsumableItemState() const { return ConsumableItemState; }

	float GetUsingTimer() const { return UsingTimer; }

protected:
	
	EConsumableItemState ConsumableItemState{};
	
	// 사용 시간 (아이템 효과 적용하는데까지 걸리는 시간)
	UPROPERTY(BlueprintReadOnly)
	float UsageTime{};

	// 사용 시간 및 아이템 효과 적용 시간 체크 Timer
	float UsingTimer{};

protected:

	// 아이템 사용 캐릭터 객체
	AC_BasicCharacter* ItemUser{};

	// 자세별 아이템 사용 동작
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EPoseState, FPriorityAnimMontage> UsingMontageMap{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UC_ItemBarWidget* MyItemBar{};
};
