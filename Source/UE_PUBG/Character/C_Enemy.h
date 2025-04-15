// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/C_BasicCharacter.h"
#include "C_Enemy.generated.h"

namespace EPathFollowingResult
{
	enum Type : int;
}

enum class ESightRangeLevel : uint8;
/**
 * 
 */
UCLASS()
class UE_PUBG_API AC_Enemy : public AC_BasicCharacter
{
	GENERATED_BODY()

public:
	AC_Enemy();

	void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaSeconds) override;


	class UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }

protected:
	
	/// <summary>
	/// 캐릭터가 땅에 착지했을 때 호출될 함수 
	/// </summary>
	void Landed(const FHitResult& Hit) override;

public:

	bool SetPoseState(EPoseState InChangeFrom, EPoseState InChangeTo) override;

	class UProgressBar* GetHPBar() const { return HPBar; }
	class UProgressBar* GetBoostBar() const { return BoostBar; }

	class AC_EnemyAIController* GetEnemyAIController() const;

	class UC_DefaultItemSpawnerComponent* GetItemSpawnerHelper() const { return ItemSpawnerHelper; }

	class UC_TargetLocationSettingHelper* GetTargetLocationSettingHelper() const { return TargetLocationSettingHelper; }

	/// <summary>
	/// <para> Character Capsule 밑 부분을 들어온 Parameter값으로 맞춰서 ActorLocation set하기 </para>
	/// </summary>
	/// <param name="BottomLocation"> : Character Capsule 밑 부분이 될 값 </param>
	/// <param name="TeleportType"></param>
	void SetActorBottomLocation(const FVector& BottomLocation, ETeleportType TeleportType = ETeleportType::None);

	FVector GetActorBottomLocation() const { return GetActorLocation() - FVector::UnitZ() * ActorZLocationOffsetFromBottom[GetPoseState()]; }

	static float GetJumpVelocityZOrigin() { return JUMP_VELOCITYZ_ORIGIN; }

public:
	
	/// <summary>
	/// <para> StatComponent에서 Damage를 정상적으로 입었을 때 호출될 함수 </para>
	/// <para> Behavior Tree FSM 처리 </para>
	/// </summary>
	/// <param name="DamageCauser"> : Damage를 준 캐릭터 </param>
	void OnTakeDamage(class AC_BasicCharacter* DamageCauser);

private:
	/// <summary>
	/// DamageCauser로 TargetCharacter 업데이트 시도 
	/// </summary>
	/// <returns> : 해당 Character로 Update 되지 않았다면 return false </returns>
	bool TryUpdateTargetCharacterToDamageCauser(AC_BasicCharacter* DamageCauser);

protected:

	void OnPoseTransitionFinish() override;

public: // For Testing
	
	UFUNCTION(BlueprintImplementableEvent)
	void SetTargetCharacterWidgetName(const FString& TargetCharacterName);

	UFUNCTION(BlueprintImplementableEvent)
	void SetSightRangeCharactersName(ESightRangeLevel SightRangeLevel, const FString& SightRangeCharactersName);	

	UFUNCTION(BlueprintImplementableEvent)
	void SetStunnedTime(float Time);

private:

	void CharacterDead() override;
	
protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UBehaviorTree* BehaviorTree{};
	
protected:

	// 디버깅용 HPBar
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UProgressBar* HPBar{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UProgressBar* BoostBar{};
	
protected:

	// Default 아이템 스폰 처리 Component
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	class UC_DefaultItemSpawnerComponent* ItemSpawnerHelper{};

private:
	
	class UC_TargetLocationSettingHelper* TargetLocationSettingHelper{};

private:

	class UC_EnemySkyDivingComponent* EnemySkyDivingComponent{};

private:

	static const float JUMP_VELOCITYZ_ORIGIN;

	// 바닥의 Z값이 0일 때, 각 자세별 ActorLocation Z 위치(또는 바닥으로부터 ActorLocation Z의 offset 값이라고 보면 됨)
	static const TMap<EPoseState, float> ActorZLocationOffsetFromBottom;

};


