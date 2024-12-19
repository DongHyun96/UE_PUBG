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

// �� Parkour ���� �ʿ��� ���� struct
struct FParkourDescriptor
{
	bool			bIsLowAction{};				// ù Obstacle �˻� �� ���� ��ֹ��� ���� ���
	FVector			FirstObstacleHitLocation{};

	TArray<FVector> VerticleHitPositions{};		// Obstacle���� �Ÿ� ���
	FVector			LandPos{};					// Obstacle�� ���� �������� ��ġ (Vaulting Low)
	bool			bLandPosInited{};

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

public:

	/// <summary>
	/// ��Ȳ�� �´� ���� �׼� �õ�
	/// </summary>
	/// <returns> : ���� �� �� ���ٸ� return false </returns>
	bool TryExecuteParkourAction();

	void SetHasTryVaulting(const FVector2D& MovementVector) { bHasTryVaulting = MovementVector.X == 1; }
	void SetHasTryVaulting(bool HasTryVaulting) { bHasTryVaulting = HasTryVaulting; }

	//bool GetIsCurrentlyWarping() const { return bIsCurrentlyWarping; }

	UFUNCTION(BlueprintCallable)
	void OnParkourAnimMontageEnd();

private:

	/// <summary>
	/// OwnerCharacter Skeletal Mesh Sawp
	/// </summary>
	/// <param name="ToRootedMesh"> : If false, Swap back to Main Skeletal Mesh </param>
	void SwapMesh(bool ToRootedMesh);

private:

	/// <summary>
	/// <para> ĳ���� ���濡 ������ ��ֹ��� �ִ��� �˻� </para>
	/// <para> ���ÿ� ���� �� �� �ִ� ���� �� bIsLowAction üũ </para>
	/// </summary>
	/// <param name="CurParkourDesc"> : �� Parkour Desc </param>
	/// <returns> : ���� Obstacle target�� valid���� �ʴٸ� return false </returns>
	bool CheckParkourTarget(FParkourDescriptor& CurParkourDesc);

	/// <summary>
	/// <para> VerticleHitPoints, LandPos, bPossibleToVault init ��Ű�� </para>
	/// </summary>
	/// <param name="CurParkourDesc"> : �� parkour Desc </param>
	/// <returns> : Warp�� ������ ���� ������ �� return false </returns>
	bool InitVerticleHitPositionsAndLandPos(FParkourDescriptor& CurParkourDesc);

	/// <summary>
	/// CurParkourAction Type ���� �� ActionStrategy ����
	/// </summary>
	/// <param name="CurParkourDesc"> : �� Parkour Desc </param>
	/// <returns> : Warp�� ������ ���� ������ �� return false </returns>
	bool InitCurParkourActionStrategy(const FParkourDescriptor& CurParkourDesc);

private:

	class AC_BasicCharacter*	OwnerCharacter{};
	class AC_Player*			OwnerPlayer{};

private:

	static TMap<EParkourActionType, class II_ParkourActionStrategy*> ParkourActionStrategies;

	class II_ParkourActionStrategy* CurParkourActionStrategy{};

protected:

	// Root Bone ó���� Skeletal Mesh
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class USkeletalMesh* RootedSkeletalMesh{};

	// Root Bone ó���� Skeletal Mesh�� Anim class
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
	// TODO : �� �������� Anim Montage ������ ��
	TMap<EParkourActionType, TArray<FPriorityAnimMontage>> ParkourMontageMap{};

private:

	class USkeletalMesh* MainSkeletalMesh{};

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TSubclassOf<class UAnimInstance> MainAnimInstanceClass{};

private:

	// Tick �Լ����� SkeletalMesh�� AnimInstanceClass�� �ٲٴ��� -> Deferred Update�� ����� ����
	bool bPendingMeshUpdateToMainMesh{};

private:

	// Input forward�� ������ ���ÿ� ���� �� true
	bool bHasTryVaulting{};
};
