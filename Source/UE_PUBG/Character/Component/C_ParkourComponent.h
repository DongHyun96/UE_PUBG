// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Character/C_BasicCharacter.h"

#include "C_ParkourComponent.generated.h"

UENUM(BlueprintType)
enum class EParkourActionType : uint8
{
	VAULTING_LOW,
	VAULTING_HIGH,
	MANTLING_LOW,
	MANTLING_HIGH,
	MAX
};

// 현 Parkour 관련 필요한 정보 struct
struct FParkourDescriptor
{
	FCollisionQueryParams CollisionParams{};

	bool			bIsLowAction{};				// 첫 Obstacle 검사 시 넘을 장애물의 높이 계산
	FVector			FirstObstacleHitLocation{};

	TArray<FVector> VerticalHitPositions{};		// Obstacle위의 거리 재기
	FVector			LandPos{};					// Obstacle을 지나 내려가는 위치 (Vaulting Low)
	bool			bLandPosInited{};

	bool bMustVault{}; // 무조건 Vaulting으로 처리를 해야하는지
	bool bMustMantle{}; // 무조건 Mantling으로 처리를 해야하는지

};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE_PUBG_API UC_ParkourComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UC_ParkourComponent();

protected:
	virtual void BeginPlay() override;

private:

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetOwnerCharacter(class AC_BasicCharacter* InOwnerCharacter) { OwnerCharacter = InOwnerCharacter; }
	void SetOwnerPlayer(class AC_Player* InOwnerPlayer) { OwnerPlayer = InOwnerPlayer; }

	TArray<FPriorityAnimMontage>& GetParkourMontages(EParkourActionType ParkourActionType) { return ParkourMontageMap[ParkourActionType]; }

	void SetCanMoveTimerAfterWarpActionFin(float Time) { CanMoveTimerAfterWarpActionFin = Time; }

public:

	/// <summary>
	/// 상황에 맞는 파쿠르 액션 시도
	/// </summary>
	/// <returns> : 파쿠르 할 수 없다면 return false </returns>
	bool TryExecuteParkourAction();

	void SetHasTryVaulting(const FVector2D& MovementVector) { bHasTryVaulting = MovementVector.X == 1; }
	void SetHasTryVaulting(bool HasTryVaulting) { bHasTryVaulting = HasTryVaulting; }

	//bool GetIsCurrentlyWarping() const { return bIsCurrentlyWarping; }

	UFUNCTION(BlueprintCallable)
	void OnParkourAnimMontageEnd();

private:

	/// <summary>
	/// 파쿠르 검사 프레임워크 통과 후 Motion warping을 실행하는 단계
	/// </summary>
	void ExecuteMotionWarpingAction(const FParkourDescriptor& CurParkourDesc);

private:

	/// <summary>
	/// OwnerCharacter Skeletal Mesh Sawp
	/// </summary>
	/// <param name="ToRootedMesh"> : If false, Swap back to Main Skeletal Mesh </param>
	void SwapMesh(bool ToRootedMesh);

	// TODO : 무기를 들고 있었다면 holster에 붙인 뒤, parkour가 끝나면 다시 무기를 드는 동작으로 가져가야 함

	/// <summary>
	/// 현 skeletalMesh에 부착된 Actor 조사하기
	/// </summary>
	/// <param name="AttachedActors"> : Init 시킬 AttachedActors </param>
	void GetSocketAttachedActors(TMap<FName, AActor*>& AttachedActors);

	/// <summary>
	/// 전 SkeletalMesh에 부착된 Actor 다시 부착하기
	/// </summary>
	/// <param name="PrevAttachedActors"> : 전에 부착되었었던 Actor들 </param>
	void ReAttachActorsToSocket(const TMap<FName, AActor*>& PrevAttachedActors);

	/// <summary>
	/// SwapMesh -> MainMesh로 돌아왔을 때 호출
	/// </summary>
	void SetOwnerCharacterCanMoveToTrue() { OwnerCharacter->SetCanMove(true); }

private:

	/// <summary>
	/// <para> VerticleHitPoints, LandPos, bPossibleToVault init 시키기 </para>
	/// </summary>
	/// <param name="CurParkourDesc"> : 현 parkour Desc </param>
	/// <returns> : Warp를 실행할 없는 조건일 때 return false </returns>
	bool InitVerticalHitPositionsAndLandPos(FParkourDescriptor& CurParkourDesc);

	/// <summary>
	/// CurParkourAction Type 지정 및 ActionStrategy 지정
	/// </summary>
	/// <param name="CurParkourDesc"> : 현 Parkour Desc </param>
	void InitCurParkourActionStrategy(const FParkourDescriptor& CurParkourDesc);

private:

	/// <summary>
	/// CheckFirstHitLocation -> InitVerticalHitPositionsAndLandPos -> CheckSpaceForParkourAction 일련의 과정
	/// </summary>
	/// <param name="CurParkourDesc"> : 현 ParkourDesc </param>
	/// <param name="bCheckLowAction"> : true이면 LowAction에 대한 Parkour 프레임워크 조사 </param>
	/// <returns></returns>
	bool CheckParkourFramework(FParkourDescriptor& CurParkourDesc, bool bCheckLowAction);

private:

	/// <summary>
	/// 캐릭터 전방에 LowAction처리할 장애물이 있는지 검사
	/// </summary>
	/// <param name="CurParkourDesc"> : 현 Parkour Desc </param>
	/// <returns> : 전방 Obstacle target이 valid하지 않다면 return false </returns>
	bool CheckLowParkourTarget(FParkourDescriptor& CurParkourDesc);

	/// <summary>
	/// 캐릭터 전방에 HighAction처리할 장애물이 있는지 검사
	/// </summary>
	/// <param name="CurParkourDesc"> : 현 Parkour Desc </param>
	/// <returns> : 전방 Obstacle target이 valid하지 않다면 return false </returns>
	bool CheckHighParkourTarget(FParkourDescriptor& CurParkourDesc);

private: // Check Parkour space 관련

	/// <summary>
	/// VerticalHitPosition 방면 파쿠르 가능한 공간이 충분히 있는지 조사 | bMustMantle, bMustVault 조정
	/// </summary>
	/// <param name="CurParkourDesc"> : 현 Parkour Desc </param>
	/// <returns> : 파쿠르를 하기에 충분한 공간이 있으면 return true </returns>
	bool CheckSpaceForParkourAction(FParkourDescriptor& CurParkourDesc);

	/// <summary>
	/// Vaulting 가능한 Space가 니오는지 검사
	/// </summary>
	/// <param name="CurParkourDesc"></param>
	/// <returns></returns>
	bool CheckSpaceForVaulting(const FParkourDescriptor& CurParkourDesc);

	/// <summary>
	/// Mantling 가능한 Space가 나오는지 검사
	/// </summary>
	/// <param name="CurParkourDesc"></param>
	/// <returns></returns>
	bool CheckSpaceForMantling(const FParkourDescriptor& CurParkourDesc);

private:

	/// <summary>
	/// bMustVault, bMustMantle 초기화
	/// </summary>
	/// <param name="CurParkourDesc"> 현 Parkour Desc </param>
	/// <returns> : 초기화 불가능 (파쿠르 자체를 할 수 없는) 상황이면 return false </returns>
	bool InitMustVaultOrMustMantle(FParkourDescriptor& CurParkourDesc);


private:

	class AC_BasicCharacter*	OwnerCharacter{};
	class AC_Player*			OwnerPlayer{};

private:

	static TMap<EParkourActionType, class II_ParkourActionStrategy*> ParkourActionStrategies;

	class II_ParkourActionStrategy* CurParkourActionStrategy{};

protected:

	// Root Bone 처리된 Skeletal Mesh
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class USkeletalMesh* RootedSkeletalMesh{};

	// Root Bone 처리된 Skeletal Mesh의 Anim class
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TSubclassOf<class UAnimInstance> RootedAnimInstanceClass{};

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TArray<FPriorityAnimMontage> LowVaultingMontages{};
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TArray<FPriorityAnimMontage> HighVaultingMontages{};
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TArray<FPriorityAnimMontage> LowMantlingMontages{};
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TArray<FPriorityAnimMontage> HighMantlingMontages{};


private:
	// TODO : 이 묶음으로 Anim Montage 관리할 것
	TMap<EParkourActionType, TArray<FPriorityAnimMontage>> ParkourMontageMap{};

private:

	class USkeletalMesh* MainSkeletalMesh{};

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TSubclassOf<class UAnimInstance> MainAnimInstanceClass{};

private:

	// Tick 함수에서 SkeletalMesh와 AnimInstanceClass를 바꾸는지 -> Deferred Update를 사용할 예정
	bool bPendingMeshUpdateToMainMesh{};

private:

	// Warp Action이 끝나고 MainSkeletalMesh로 돌아온 이 후, CanMove를 true로 주기까지의 Timer 시간
	float CanMoveTimerAfterWarpActionFin{};
	struct FTimerHandle TimerHandle{};

private:

	// Input forward와 파쿠르를 동시에 했을 시 true
	bool bHasTryVaulting{};

private:

	FColor DebugMsgColor{};
};
