// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/C_BasicCharacter.h"
#include "C_Enemy.generated.h"

namespace EPathFollowingResult
{
	enum Type : int;
}

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

	void Tick(float DeltaSeconds) override;


	class UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }

protected:
	
	/// <summary>
	/// 캐릭터가 땅에 착지했을 때 호출될 함수 
	/// </summary>
	void Landed(const FHitResult& Hit) override;

public:

	bool SetPoseState(EPoseState InChangeFrom, EPoseState InChangeTo) override;

	class UProgressBar* GetHPBar() const { return HPBar; }

	class AC_EnemyAIController* GetEnemyAIController() const;

	class UC_DefaultItemSpawnerComponent* GetItemSpawnerHelper() const { return ItemSpawnerHelper; }

	class UC_TargetLocationSettingHelper* GetTargetLocationSettingHelper() const { return TargetLocationSettingHelper; }

	/// <summary>
	/// <para> Character Capsule 밑 부분을 들어온 Parameter값으로 맞춰서 ActorLocation set하기 </para>
	/// </summary>
	/// <param name="BottomLocation"> : Character Capsule 밑 부분이 될 값 </param>
	/// <param name="TeleportType"></param>
	void SetActorBottomLocation(const FVector& BottomLocation, ETeleportType TeleportType = ETeleportType::None);

	static float GetJumpVelocityZOrigin() { return JUMP_VELOCITYZ_ORIGIN; }

public:

	UFUNCTION(BlueprintImplementableEvent)
	void SetTargetCharacterWidgetName(const FString& TargetCharacterName);
	
protected:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	class UBehaviorTree* BehaviorTree{};

protected:

	// 디버깅용 HPBar
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UProgressBar* HPBar{};

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

};


