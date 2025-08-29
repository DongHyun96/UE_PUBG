// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item/C_Item.h"
#include "C_PlayerCombatFieldManager.generated.h"


enum class EAttachmentNames : uint8;

enum class EPlayerCombatFieldState : uint8
{
	Idle,				// Play 중이지 않은 상황
	WaitingRoundStart,	// Round 시작 전까지 기다리고 있는 상황
	PlayingRound,		// Round 중
	RoundEnd,			// Round 끝
	MatchingEnd			// Matching 끝 (3판 2선 중 2선을 했거나 3판 모두 끝났을 때)
};

enum class EPlayerCombatRoundResult : uint8
{
	NotPlayed,
	PlayerWin,
	EnemyWin,
	Draw
};

struct FPlayerCombatRoundResult
{
	EPlayerCombatRoundResult RoundResult{};

	float RoundPlayTime{};
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UE_PUBG_API UC_PlayerCombatFieldManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UC_PlayerCombatFieldManager();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:

	class AC_Enemy* GetCombatFieldEnemy() const { return CombatFieldEnemy; }

	void SetOwnerCombatFieldManager(class AC_CombatFieldManager* InCombatFieldManager) { OwnerCombatFieldManager = InCombatFieldManager; }
	void SetPlayerCombatFieldWidget(class UC_PlayerCombatFieldWidget* InPlayerCombatFieldWidget) { PlayerCombatFieldWidget = InPlayerCombatFieldWidget; }

public:

	uint8 GetCurrentRound() const { return CurrentRound; }

	const TArray<FPlayerCombatRoundResult>& GetRoundResults() const { return RoundResults; }

	/// <summary>
	/// Combat enemy나 Combat Player 사망 시, Round Result 세팅 처리
	/// </summary>
	/// <returns> : Setting될 수 없는 상황이라면 return false </returns>
	bool SetCurrentRoundResultOnCharacterDead(bool bIsDraw);

	void SetPlayerCombatFieldState(EPlayerCombatFieldState FieldState);
	EPlayerCombatFieldState GetPlayerCombatFieldState() const { return CombatFieldState; }

public:
	
	/// <summary>
	/// Round UI Animation Sequence 종료 이후, Match가 끝났는지, 아니면 다음 Round로 넘어가야 하는지 체크해서 PlayerCombatField 상태 변환 처리 
	/// </summary>
	void OnRoundUIRoutineFinished();

	
private: // Combat Start 처리 관련 (OpeningGate Interaction)
	
	UFUNCTION()
	void OnGateOpeningBoxBeginOverlap
	(
		UPrimitiveComponent*	OverlappedComp,
		AActor*					OtherActor,
		UPrimitiveComponent*	OtherComp,
		int32					OtherBodyIndex,
		bool					bFromSweep,
		const FHitResult&		SweepResult
	);

	UFUNCTION()
	void OnGateOpeningBoxEndOverlap
	(
		UPrimitiveComponent* OverlappedComponent,
		AActor*				 OtherActor,
		UPrimitiveComponent* OtherComp,
		int32				 OtherBodyIndex
	);

	/// <summary>
	/// Start Gate OpeningBox Overlap되었을 때, F Key Start 처리
	/// </summary>
	/// <returns> : 제대로 Start 처리가 되었다면 return true </returns>
	bool OnStartGateFKeyInteraction();

private:

	/// <summary>
	/// 마우스 Input callback 함수
	/// </summary>
	void OnLookInput(const struct FInputActionValue& Value);

private:

	AC_CombatFieldManager* OwnerCombatFieldManager{};

protected:

	UPROPERTY(BlueprintReadWrite, EditInstanceOnly)
	AC_Enemy* CombatFieldEnemy{};

private: // Spawn(Respawn) Transform 관련
	
	TArray<FTransform> SpawnTransforms{};
	
private:
	
	UC_PlayerCombatFieldWidget* PlayerCombatFieldWidget{};

protected:

	UPROPERTY(BlueprintReadWrite, EditInstanceOnly)
	class AC_TutorialGate* CombatFieldStartGate{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UInputMappingContext* IMC_WaitRound{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UInputAction* LookAction{};	
	
protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EAttachmentNames, TSubclassOf<class AC_AttachableItem>> GunAttachmentClasses =
	{
		{EAttachmentNames::REDDOT, 		nullptr},
		{EAttachmentNames::SCOPE4, 		nullptr},
		{EAttachmentNames::SCOPE8, 		nullptr},
		{EAttachmentNames::VERTGRIP,	nullptr},
		{EAttachmentNames::EXTENDMAG,	nullptr},
		{EAttachmentNames::QUICKMAG,	nullptr},
		{EAttachmentNames::COMPENSATOR, nullptr},
		{EAttachmentNames::SUPPRESSOR,	nullptr}
	};


private:

	EPlayerCombatFieldState CombatFieldState{};

	// Round Start 까지 남은 시간 및, Round 남은 시간 표시할 범용적 Timer
	float CombatFieldTimer{};

	// 현재 Round Number
	uint8 CurrentRound = 1;

	// Index 0은 사용하지 않음 (Dummy, Round1, Round2, Round3)
	TArray<FPlayerCombatRoundResult> RoundResults{};

	uint8 PlayerWinCount{}, EnemyWinCount{};
};
