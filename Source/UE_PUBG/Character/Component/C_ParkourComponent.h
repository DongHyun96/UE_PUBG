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
	FCollisionQueryParams CollisionParams{};

	bool			bIsLowAction{};				// ù Obstacle �˻� �� ���� ��ֹ��� ���� ���
	FVector			FirstObstacleHitLocation{};

	TArray<FVector> VerticalHitPositions{};		// Obstacle���� �Ÿ� ���
	FVector			LandPos{};					// Obstacle�� ���� �������� ��ġ (Vaulting Low)
	bool			bLandPosInited{};

	bool bMustVault{}; // ������ Vaulting���� ó���� �ؾ��ϴ���
	bool bMustMantle{}; // ������ Mantling���� ó���� �ؾ��ϴ���

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
	/// ���� �˻� �����ӿ�ũ ��� �� Motion warping�� �����ϴ� �ܰ�
	/// </summary>
	void ExecuteMotionWarpingAction(const FParkourDescriptor& CurParkourDesc);

private:

	/// <summary>
	/// OwnerCharacter Skeletal Mesh Sawp
	/// </summary>
	/// <param name="ToRootedMesh"> : If false, Swap back to Main Skeletal Mesh </param>
	void SwapMesh(bool ToRootedMesh);

	// TODO : ���⸦ ��� �־��ٸ� holster�� ���� ��, parkour�� ������ �ٽ� ���⸦ ��� �������� �������� ��

	/// <summary>
	/// �� skeletalMesh�� ������ Actor �����ϱ�
	/// </summary>
	/// <param name="AttachedActors"> : Init ��ų AttachedActors </param>
	void GetSocketAttachedActors(TMap<FName, AActor*>& AttachedActors);

	/// <summary>
	/// �� SkeletalMesh�� ������ Actor �ٽ� �����ϱ�
	/// </summary>
	/// <param name="PrevAttachedActors"> : ���� �����Ǿ����� Actor�� </param>
	void ReAttachActorsToSocket(const TMap<FName, AActor*>& PrevAttachedActors);

	/// <summary>
	/// SwapMesh -> MainMesh�� ���ƿ��� �� ȣ��
	/// </summary>
	void SetOwnerCharacterCanMoveToTrue() { OwnerCharacter->SetCanMove(true); }

private:

	/// <summary>
	/// <para> VerticleHitPoints, LandPos, bPossibleToVault init ��Ű�� </para>
	/// </summary>
	/// <param name="CurParkourDesc"> : �� parkour Desc </param>
	/// <returns> : Warp�� ������ ���� ������ �� return false </returns>
	bool InitVerticalHitPositionsAndLandPos(FParkourDescriptor& CurParkourDesc);

	/// <summary>
	/// CurParkourAction Type ���� �� ActionStrategy ����
	/// </summary>
	/// <param name="CurParkourDesc"> : �� Parkour Desc </param>
	void InitCurParkourActionStrategy(const FParkourDescriptor& CurParkourDesc);

private:

	/// <summary>
	/// CheckFirstHitLocation -> InitVerticalHitPositionsAndLandPos -> CheckSpaceForParkourAction �Ϸ��� ����
	/// </summary>
	/// <param name="CurParkourDesc"> : �� ParkourDesc </param>
	/// <param name="bCheckLowAction"> : true�̸� LowAction�� ���� Parkour �����ӿ�ũ ���� </param>
	/// <returns></returns>
	bool CheckParkourFramework(FParkourDescriptor& CurParkourDesc, bool bCheckLowAction);

private:

	/// <summary>
	/// ĳ���� ���濡 LowActionó���� ��ֹ��� �ִ��� �˻�
	/// </summary>
	/// <param name="CurParkourDesc"> : �� Parkour Desc </param>
	/// <returns> : ���� Obstacle target�� valid���� �ʴٸ� return false </returns>
	bool CheckLowParkourTarget(FParkourDescriptor& CurParkourDesc);

	/// <summary>
	/// ĳ���� ���濡 HighActionó���� ��ֹ��� �ִ��� �˻�
	/// </summary>
	/// <param name="CurParkourDesc"> : �� Parkour Desc </param>
	/// <returns> : ���� Obstacle target�� valid���� �ʴٸ� return false </returns>
	bool CheckHighParkourTarget(FParkourDescriptor& CurParkourDesc);

private: // Check Parkour space ����

	/// <summary>
	/// VerticalHitPosition ��� ���� ������ ������ ����� �ִ��� ���� | bMustMantle, bMustVault ����
	/// </summary>
	/// <param name="CurParkourDesc"> : �� Parkour Desc </param>
	/// <returns> : ������ �ϱ⿡ ����� ������ ������ return true </returns>
	bool CheckSpaceForParkourAction(FParkourDescriptor& CurParkourDesc);

	/// <summary>
	/// Vaulting ������ Space�� �Ͽ����� �˻�
	/// </summary>
	/// <param name="CurParkourDesc"></param>
	/// <returns></returns>
	bool CheckSpaceForVaulting(const FParkourDescriptor& CurParkourDesc);

	/// <summary>
	/// Mantling ������ Space�� �������� �˻�
	/// </summary>
	/// <param name="CurParkourDesc"></param>
	/// <returns></returns>
	bool CheckSpaceForMantling(const FParkourDescriptor& CurParkourDesc);

private:

	/// <summary>
	/// bMustVault, bMustMantle �ʱ�ȭ
	/// </summary>
	/// <param name="CurParkourDesc"> �� Parkour Desc </param>
	/// <returns> : �ʱ�ȭ �Ұ��� (���� ��ü�� �� �� ����) ��Ȳ�̸� return false </returns>
	bool InitMustVaultOrMustMantle(FParkourDescriptor& CurParkourDesc);


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

	// Warp Action�� ������ MainSkeletalMesh�� ���ƿ� �� ��, CanMove�� true�� �ֱ������ Timer �ð�
	float CanMoveTimerAfterWarpActionFin{};
	struct FTimerHandle TimerHandle{};

private:

	// Input forward�� ������ ���ÿ� ���� �� true
	bool bHasTryVaulting{};

private:

	FColor DebugMsgColor{};
};
