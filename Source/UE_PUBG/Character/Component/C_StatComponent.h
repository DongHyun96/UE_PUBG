// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_StatComponent.generated.h"


/// <summary>
/// TODO : ���⿡���� UI �ǵ��� �ʰ� ������ Stat ������ ��ġ�� �ǵ帮��
/// </summary>
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE_PUBG_API UC_StatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UC_StatComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public: // Getters and setters

	UFUNCTION(BlueprintCallable)
	float GetCurHP() const { return CurHP; }

	/// <summary>
	/// ���� : Character�ܿ��� ȣ���� ��, HUD ���� �Ұ� -> Character Ŭ���� �ܿ��� ȣ���� ������ Character�� SetCurHP�� ȣ���� �� 
	/// </summary>
	void SetCurHP(const float& InCurHP) { CurHP = InCurHP; }

	UFUNCTION(BlueprintCallable)
	float GetCurBoosting() const { return CurBoosting; }

	/// <summary>
	/// ���� : Character�ܿ��� ȣ���� ��, HUD ���� �Ұ� -> Character Ŭ���� �ܿ��� ȣ���� ������ Character�� SetCurBoosting ȣ���� ��
	/// </summary>
	void SetCurBoosting(const float& InCurBoosting) { CurBoosting = InCurBoosting; }

	const float GetHealUpLimit() const { return HEAL_UP_LIMIT; }

public:

	bool TakeDamage(const float& Damage);

	/// <summary>
	/// ���� : Character�ܿ��� ȣ���� ��, HUD ���� �Ұ� -> Character Ŭ���� �ܿ��� ȣ���� ������ Character�� ApplyHeal ȣ���� �� 
	/// </summary>
	bool ApplyHeal(const float& HealAmount);
	
	/// <summary>
	/// ���� : Character�ܿ��� ȣ���� ��, HUD ���� �Ұ� -> Character Ŭ���� �ܿ��� ȣ���� ������ Character�� AddBoost ȣ���� �� 
	/// </summary>
	bool AddBoost(const float& BoostAmount);

private:

	const float MAX_HP			= 100.f;
	const float MAX_BOOSTING	= 100.f;

	const float HEAL_UP_LIMIT = 75.f; // ���޻���, �ش�� ä�� �� �ִ� �� ���� limit

protected:

	UPROPERTY(BlueprintReadOnly)
	float CurHP = 100.f;

	UPROPERTY(BlueprintReadOnly)
	float CurBoosting{};

};
