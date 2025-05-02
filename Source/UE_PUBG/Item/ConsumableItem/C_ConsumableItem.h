// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/C_Item.h"

#include "Character/C_BasicCharacter.h"
#include "C_ConsumableItem.generated.h"


UENUM(BlueprintType)
enum class EConsumableItemState : uint8
{
	IDLE,
	ACTIVATING,
	ACTIVATE_COMPLETED,
	USED
};

UENUM(BlueprintType)
enum class EConsumableItemType : uint8
{
	MEDKIT,
	FIRST_AID_KIT,
	BANDAGE,
	PAIN_KILLER,
	ENERGY_DRINK,
	MAX
};

USTRUCT(BlueprintType)
struct FHealItemSoundData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USoundBase* UsingSound = nullptr;

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
	
	//void SetLinkedItemBarWidget(class UC_ItemBarWidget* InItemBarWidget);
	
	void SetLinkedItemBarWidget(class UC_BasicItemBarWidget* InItemBarWidget);

	void InitializeItem(FName NewItemCode) override;

public:
	
	// static const TMap<EConsumableItemType, FName> ConsumableItemNameMap	
	static FName GetConsumableItemName(EConsumableItemType InConsumableItemType);
	
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

	//bool LegacyMoveToInven(AC_BasicCharacter* Character) override;
	//
	//bool LegacyMoveToSlot(AC_BasicCharacter* Character) override;
	//
	//bool LegacyMoveToAround(AC_BasicCharacter* Character) override;


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

protected:

	virtual void HandleDestroy() PURE_VIRTUAL(AC_ConsumableItem::HandleDestroy, );

protected:
	bool MoveInvenToAround(AC_BasicCharacter* Character, int32 InStack) override;
	bool MoveInvenToInven(AC_BasicCharacter* Character, int32 InStack) override;
	bool MoveInvenToSlot(AC_BasicCharacter* Character, int32 InStack) override;

	bool MoveAroundToAround(AC_BasicCharacter* Character, int32 InStack) override;
	bool MoveAroundToInven(AC_BasicCharacter* Character, int32 InStack) override;
	bool MoveAroundToSlot(AC_BasicCharacter* Character, int32 InStack) override;

protected:
	void PlayUsingSound();

	void StopUsingSound();

public: // getters and setters

	EConsumableItemState GetConsumableItemState() const { return ConsumableItemState; }

	float GetUsingTimer() const { return UsingTimer; }
	
	//이전 코드
	//class UC_ItemBarWidget* GetLinkedItemBarWidget() { return LinkedItemBarWidget; }

	//리팩토링중인 ItemBar
	class UC_BasicItemBarWidget* GetLinkedItemBarWidget() { return LinkedItemBarWidget; }

	const FHealItemSoundData* GetUsingSoundData() const { return UsingSoundData; }

	/// <summary>
	/// Activating 단계에서 (활성화한 시간 / 총 활성화 시간) 비율 구하기
	/// </summary>
	/// <param name="OutRatio"> : 구한 비율 </param>
	/// <returns> : 만약 ACTIVATING State가 아니라면 return false </returns>
	bool GetActivationProgressTimeRatio(float& OutRatio) const;

protected:
	
	EConsumableItemState ConsumableItemState{};
	
	// 사용 시간 (아이템 효과 적용하는데까지 걸리는 시간)
	UPROPERTY(BlueprintReadOnly)
	float UsageTime{};

	// 사용 시간 및 아이템 효과 적용 시간 체크 Timer
	float UsingTimer{};

	//효과음 구조체
	const FHealItemSoundData* UsingSoundData = nullptr;

	//오디오 컨트롤러
	UAudioComponent* AudioComponent = nullptr;


protected:

	// 아이템 사용 캐릭터 객체
	AC_BasicCharacter* ItemUser{};

	// 자세별 아이템 사용 동작
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EPoseState, FPriorityAnimMontage> UsingMontageMap{};


private:

	//// 이 Consumable Item과 연결된 ItemBarWidget 객체
	//UI리팩토링중에 사용중인 ItemBar
	UC_BasicItemBarWidget* LinkedItemBarWidget{};

private:

	static const TMap<EConsumableItemType, FName> ConsumableItemNameMap;

};


