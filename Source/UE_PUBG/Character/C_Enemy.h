// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/C_BasicCharacter.h"
#include "C_Enemy.generated.h"

// CharacterDestroy 함수가 호출될 때 불려질 Delegate Type 선언
// Combat Tester가 사용할 예정
DECLARE_DELEGATE(FOnCombatCharacterDestroy_Delegate);
// DECLARE_DELEGATE_RetVal(bool, FOnCombatCharacterDestroy_Delegate);


namespace EPathFollowingResult
{
	enum Type : int;
}

enum class ESightRangeLevel : uint8;

UENUM(BlueprintType)
enum class EEnemyBehaviorType : uint8
{
	InGamePlayable,
	MovementTest,
	StatCareTest,
	SkyDivingTest,
	CombatTest,
	Max
};

/**
 * 
 */
UCLASS()
class UE_PUBG_API AC_Enemy : public AC_BasicCharacter
{
	GENERATED_BODY()

public:
	AC_Enemy();

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaSeconds) override;

public:
	
	/// <summary>
	/// 자신의 BehaviorType에 따라 BehaviorTree 초기화 시키기 
	/// </summary>
	void InitBehaviorTreeBySelfBehaviorType();

	class UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }

	EEnemyBehaviorType GetBehaviorType() const { return EnemyBehaviorType; }

protected:
	
	/// <summary>
	/// 캐릭터가 땅에 착지했을 때 호출될 함수 
	/// </summary>
	void Landed(const FHitResult& Hit) override;

public:

	bool SetPoseState(EPoseState InChangeFrom, EPoseState InChangeTo) override;

	class UProgressBar* GetHPBar() const { return HPBar; }
	UProgressBar*       GetBoostBar() const { return BoostBar; }

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

	class UCameraComponent* GetSpectatorCameraComponent() const { return SpectatorCameraComponent; }
	class USpringArmComponent* GetSpringArmComponent() const { return SpectatorSpringArmComponent; }

public:
	
	/// <summary>
	/// <para> StatComponent에서 Damage를 정상적으로 입었을 때 호출될 함수 </para>
	/// <para> Behavior Tree FSM 처리 </para>
	/// </summary>
	/// <param name="DamageCauser"> : Damage를 준 캐릭터 </param>
	void OnTakeDamage(AC_BasicCharacter* DamageCauser);

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

private: /* Dead 처리 관련 */

	void CharacterDead(const FKillFeedDescriptor& KillFeedDescriptor) override;

	void DestroyCharacter() override;

public:

	UFUNCTION(BlueprintCallable)
	void SetRotationToInitialRotation() { SetActorRotation(InitialRotation); }
	
protected:

	// 이 EnemyBehaviorType이 결정됨에 따라 BehaviorTree도 서로 다른 BehaviorTree로 초기화
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EEnemyBehaviorType EnemyBehaviorType{};

private:

	static const TMap<EEnemyBehaviorType, FString>		BehaviorTreeReferenceDirectories; 
	static TMap<EEnemyBehaviorType, UBehaviorTree*>		BehaviorTrees;
	
	// 자신의 EnemyBehaviorType에 따라 사용할 BehaviorTree
	UBehaviorTree* BehaviorTree{};
	
protected:

	// 디버깅용 HPBar
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UProgressBar* HPBar{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UProgressBar* BoostBar{};
	
protected:

	// Default 아이템 스폰 처리 Component
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	UC_DefaultItemSpawnerComponent* ItemSpawnerHelper{};

private:
	
	UCameraComponent* SpectatorCameraComponent{}; // CombatTester Spectate 용 CameraActor
	USpringArmComponent* SpectatorSpringArmComponent{}; // CombatTester Spectate 용 SpringArmS

private:
	
	UC_TargetLocationSettingHelper* TargetLocationSettingHelper{};

private:

	class UC_EnemySkyDivingComponent* EnemySkyDivingComponent{};

private:

	static const float JUMP_VELOCITYZ_ORIGIN;

	// 바닥의 Z값이 0일 때, 각 자세별 ActorLocation Z 위치(또는 바닥으로부터 ActorLocation Z의 offset 값이라고 보면 됨)
	static const TMap<EPoseState, float> ActorZLocationOffsetFromBottom;

private:

	// SkyDive Tester 처리 때에만 쓰일 예정
	FRotator InitialRotation{};

public:

	FOnCombatCharacterDestroy_Delegate Delegate_OnCombatCharacterDestroy{};	

};
