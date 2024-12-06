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
	EParkourActionType CurParkourActionType{};

	bool	bIsLowAction{}; // 첫 Obstacle 검사 시 넘을 장애물의 높이 계산
	FVector FirstObstacleHitLocation{};

	FVector WarpStartPos{};
	FVector WarpMiddlePos{};
	FVector WarpLandPos{};

	// Vault일 경우		- MiddlePos와 LandPos 모두 잘 잡혔을 때 CanWarp 가능
	// Mantling일 경우	- MiddlePos까지 잘 잡혔을 때 CanWarp 가능
	bool CanWarp{};
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE_PUBG_API UC_ParkourComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UC_ParkourComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetOwnerCharacter(class AC_BasicCharacter* InOwnerCharacter) { OwnerCharacter = InOwnerCharacter; }

public:

	/// <summary>
	/// 상황에 맞는 파쿠르 액션 시도
	/// </summary>
	/// <returns> : 파쿠르 할 수 없다면 return false </returns>
	bool TryExecuteParkourAction();

	void Vault();

	void SetHasTryVaulting(const FVector2D& MovementVector) { bHasTryVaulting = MovementVector.X == 1; }
	void SetHasTryVaulting(bool HasTryVaulting) { bHasTryVaulting = HasTryVaulting; }

	UFUNCTION(BlueprintCallable)
	void OnParkourAnimMontageEnd();

private:

	/// <summary>
	/// OwnerCharacter Skeletal Mesh Sawp
	/// </summary>
	/// <param name="ToRootedMesh"> : If false, Swap back to Main Skeletal Mesh </param>
	void SwapMesh(bool ToRootedMesh);

	void VaultMotionWarp();



private:

	/// <summary>
	/// <para> 캐릭터 전방에 파쿠르할 장애물이 있는지 검사 </para>
	/// <para> 동시에 bIsLowAction 체크 </para>
	/// </summary>
	/// <param name="CurParkourDesc"> : 현 Parkour Desc </param>
	/// <returns> : 전방 Obstacle target이 valid하지 않다면 return false </returns>
	bool CheckParkourTarget(FParkourDescriptor& CurParkourDesc);

	/// <summary>
	/// CurParkourActionType 및 ParkourStart ~ ParkourLandPos, CanWarp 조사
	/// </summary>
	/// <param name="CurParkourDesc"> : 현 Parkour Desc </param>
	/// <returns> : Warp를 실행할 없는 조건일 때 return false </returns>
	bool CheckParkourActionAndDistance(FParkourDescriptor& CurParkourDesc);

private:

	class AC_BasicCharacter* OwnerCharacter{};

protected:

	// Root Bone 처리된 Skeletal Mesh
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class USkeletalMesh* RootedSkeletalMesh{};

	// Root Bone 처리된 Skeletal Mesh의 Anim class
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TSubclassOf<class UAnimInstance> RootedAnimInstanceClass{};

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FPriorityAnimMontage VaultingMontage{};

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

	FVector VaultStartPos{};
	FVector VaultMiddlePos{};
	FVector VaultLandPos{};

	bool CanWarp{}; // Middle pos 제대로 잡혔다면 true

private:

	// Tick 함수에서 SkeletalMesh와 AnimInstanceClass를 바꾸는지 -> Deferred Update를 사용할 예정
	bool bPendingMeshUpdateToMainMesh{};

private:

	// Input forward와 파쿠르를 동시에 했을 시 true
	bool bHasTryVaulting{};

};
