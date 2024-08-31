// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_StatComponent.generated.h"


/// <summary>
/// TODO : 여기에서는 UI 건들지 않고 오로지 Stat 관련한 수치만 건드리기
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
	/// 주의 : Character외에서 호출할 때, HUD 적용 불가 -> Character 클래스 외에서 호출할 때에는 Character의 SetCurHP를 호출할 것 
	/// </summary>
	void SetCurHP(const float& InCurHP) { CurHP = InCurHP; }

	UFUNCTION(BlueprintCallable)
	float GetCurBoosting() const { return CurBoosting; }

	/// <summary>
	/// 주의 : Character외에서 호출할 때, HUD 적용 불가 -> Character 클래스 외에서 호출할 때에는 Character의 SetCurBoosting 호출할 것
	/// </summary>
	void SetCurBoosting(const float& InCurBoosting) { CurBoosting = InCurBoosting; }

	const float GetHealUpLimit() const { return HEAL_UP_LIMIT; }

public:

	bool TakeDamage(const float& Damage);

	/// <summary>
	/// 주의 : Character외에서 호출할 때, HUD 적용 불가 -> Character 클래스 외에서 호출할 때에는 Character의 ApplyHeal 호출할 것 
	/// </summary>
	bool ApplyHeal(const float& HealAmount);
	
	/// <summary>
	/// 주의 : Character외에서 호출할 때, HUD 적용 불가 -> Character 클래스 외에서 호출할 때에는 Character의 AddBoost 호출할 것 
	/// </summary>
	bool AddBoost(const float& BoostAmount);

private:

	const float MAX_HP			= 100.f;
	const float MAX_BOOSTING	= 100.f;

	const float HEAL_UP_LIMIT = 75.f; // 구급상자, 붕대로 채울 수 있는 총 힐량 limit

protected:

	UPROPERTY(BlueprintReadOnly)
	float CurHP = 100.f;

	UPROPERTY(BlueprintReadOnly)
	float CurBoosting{};

};
