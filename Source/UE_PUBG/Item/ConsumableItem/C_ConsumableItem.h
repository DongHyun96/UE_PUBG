// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Item/C_Item.h"
#include "Character/C_BasicCharacter.h"
#include "C_ConsumableItem.generated.h"


// TODO : Cancel ��� �־�� ��

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
	/// �Ҹ� ������ ����ϱ� ����
	/// </summary>
	/// <param name="ItemUser"> : ������ ��� Character </param>
	/// <returns> : ��� ������ ������ �Ǿ��ٸ� return true </returns>
	bool StartUsingConsumableItem(class AC_BasicCharacter* InItemUser);

	/// <summary>
	/// ��� Ȱ��ȭ �õ� �� Item ��� ��� �õ�
	/// </summary>
	/// <returns> : ���������� ��ҵǾ��ٸ� return true </returns>
	bool CancelActivating();

	bool Interaction(AC_BasicCharacter* Character) override;

	bool MoveToInven(AC_BasicCharacter* Character) override;

	bool MoveToSlot(AC_BasicCharacter* Character) override;

	bool MoveToAround(AC_BasicCharacter* Character) override;


protected:

	/// <summary>
	/// �� ������ �� ��� ���� ������ �������� check
	/// </summary>
	virtual bool IsAvailableToStartUsing(class AC_BasicCharacter* InItemUser) PURE_VIRTUAL(AC_ConsumableItem::IsAvailableToStartUsing, return false;);

	/// <summary>
	/// �� ������ �� ��� ���� �� ȣ��� Template method
	/// </summary>
	virtual void OnStartUsing() PURE_VIRTUAL(AC_ConsumableItem::OnStartUsing, );

	/// <summary>
	/// �� ������ �� Activating�� ������ �� ȣ��� Template method
	/// </summary>
	virtual void OnActivatingFinish() PURE_VIRTUAL(AC_ConsumableItem::OnActivatingFinish, );
	
protected:

	/// <summary>
	/// ������ �ߵ� ���۰� ���� �ߵ� ������ �����ð��� ȣ��� Tick Activating State Template method
	/// </summary>
	virtual void HandleActivatingState() PURE_VIRTUAL(AC_ConsumableItem::HandleActivatingState, );

	/// <summary>
	/// ������ ��� �ð����� ���� ���� �ʰ� ��� �ð��� �Ҹ��ߴٸ�, ����ϱ� ����(Tick ActivatedCompleted Template method)
	/// </summary>
	virtual void HandleActivateCompletedState() PURE_VIRTUAL(AC_ConsumableItem::HandleActivateCompletedState, );

	/// <summary>
	/// ������ �ߵ� ������ Cancel ������ �� ȣ��� Template method
	/// </summary>
	virtual void OnCancelActivating() PURE_VIRTUAL(AC_ConsumableItem::OnCancelActivating, );

public: // getters and setters

	EConsumableItemState GetConsumableItemState() const { return ConsumableItemState; }

	float GetUsingTimer() const { return UsingTimer; }

protected:
	
	EConsumableItemState ConsumableItemState{};
	
	// ��� �ð� (������ ȿ�� �����ϴµ����� �ɸ��� �ð�)
	UPROPERTY(BlueprintReadOnly)
	float UsageTime{};

	// ��� �ð� �� ������ ȿ�� ���� �ð� üũ Timer
	float UsingTimer{};

protected:

	// ������ ��� ĳ���� ��ü
	AC_BasicCharacter* ItemUser{};

	// �ڼ��� ������ ��� ����
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EPoseState, FPriorityAnimMontage> UsingMontageMap{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UC_ItemBarWidget* MyItemBar{};
};
