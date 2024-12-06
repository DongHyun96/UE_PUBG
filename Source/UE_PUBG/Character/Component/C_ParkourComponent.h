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
	EParkourActionType CurParkourActionType{};

	bool	bIsLowAction{}; // ù Obstacle �˻� �� ���� ��ֹ��� ���� ���
	FVector FirstObstacleHitLocation{};

	FVector WarpStartPos{};
	FVector WarpMiddlePos{};
	FVector WarpLandPos{};

	// Vault�� ���		- MiddlePos�� LandPos ��� �� ������ �� CanWarp ����
	// Mantling�� ���	- MiddlePos���� �� ������ �� CanWarp ����
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
	/// ��Ȳ�� �´� ���� �׼� �õ�
	/// </summary>
	/// <returns> : ���� �� �� ���ٸ� return false </returns>
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
	/// <para> ĳ���� ���濡 ������ ��ֹ��� �ִ��� �˻� </para>
	/// <para> ���ÿ� bIsLowAction üũ </para>
	/// </summary>
	/// <param name="CurParkourDesc"> : �� Parkour Desc </param>
	/// <returns> : ���� Obstacle target�� valid���� �ʴٸ� return false </returns>
	bool CheckParkourTarget(FParkourDescriptor& CurParkourDesc);

	/// <summary>
	/// CurParkourActionType �� ParkourStart ~ ParkourLandPos, CanWarp ����
	/// </summary>
	/// <param name="CurParkourDesc"> : �� Parkour Desc </param>
	/// <returns> : Warp�� ������ ���� ������ �� return false </returns>
	bool CheckParkourActionAndDistance(FParkourDescriptor& CurParkourDesc);

private:

	class AC_BasicCharacter* OwnerCharacter{};

protected:

	// Root Bone ó���� Skeletal Mesh
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class USkeletalMesh* RootedSkeletalMesh{};

	// Root Bone ó���� Skeletal Mesh�� Anim class
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
	// TODO : �� �������� Anim Montage ������ ��
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

	bool CanWarp{}; // Middle pos ����� �����ٸ� true

private:

	// Tick �Լ����� SkeletalMesh�� AnimInstanceClass�� �ٲٴ��� -> Deferred Update�� ����� ����
	bool bPendingMeshUpdateToMainMesh{};

private:

	// Input forward�� ������ ���ÿ� ���� �� true
	bool bHasTryVaulting{};

};
