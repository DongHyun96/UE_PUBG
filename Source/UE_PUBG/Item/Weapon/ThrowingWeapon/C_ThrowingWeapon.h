// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Weapon/C_Weapon.h"
#include "C_ThrowingWeapon.generated.h"

USTRUCT(BlueprintType)
struct FThrowProcessMontages
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FPriorityAnimMontage RemovePinMontage{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FPriorityAnimMontage ThrowReadyMontage{};		// overdraw

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FPriorityAnimMontage ThrowMontage{};
};

/**
 * 
 */
UCLASS()
class UE_PUBG_API AC_ThrowingWeapon : public AC_Weapon
{
	GENERATED_BODY()

public:

	AC_ThrowingWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	/// <summary>
	/// Throwable�� Holster�� ���� ��, visible�� ��
	/// </summary>
	bool AttachToHolster(class USceneComponent* InParent) override;

	bool AttachToHand(class USceneComponent* InParent) override;

public:

	/// <summary>
	/// TODO : �� �Լ� �����
	/// </summary>
	static void InitTestPool(class AC_BasicCharacter* InOwnerCharacter, UClass* Class, class UC_EquippedComponent* EquippedComponent);

public:

	/// <summary>
	/// On Remove Pin Anim Montage end call back
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void OnRemovePinFin();

	/// <summary>
	/// On Throw loop call back
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void OnThrowReadyLoop();

	/// <summary>
	/// ���������� ������ ó��
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void OnThrowThrowable();

	/// <summary>
	/// On Throw Process End call
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void OnThrowProcessEnd();


public: // Getters & Setters

	void SetIsCharging(bool InIsCharging) { bIsCharging = InIsCharging; }
	bool GetIsCharging() const { return bIsCharging; }

	void SetIsOnThrowProcess(bool IsOnThrowProcess) { bIsOnThrowProcess = IsOnThrowProcess; }
	bool GetIsOnThrowProcess() const { return bIsOnThrowProcess; }

	FThrowProcessMontages GetCurThrowProcessMontages() const { return CurThrowProcessMontages; }
	FPriorityAnimMontage GetCurRemovePinMontage() const { return CurThrowProcessMontages.RemovePinMontage; }
	FPriorityAnimMontage GetCurThrowReadyMontage() const { return CurThrowProcessMontages.ThrowReadyMontage; }
	FPriorityAnimMontage GetCurThrowMontage() const { return CurThrowProcessMontages.ThrowMontage; }

private:

	/// <summary>
	/// ����ź ��ô ���� ��� �׸��� (����� line)
	/// </summary>
	void DrawDebugPredictedPath();

	/// <summary>
	/// ����ź ��ô ���� ��� �׸��� (Niagara line) - USE THIS IN REAL PLAY
	/// </summary>
	void DrawNiagaraPredictedPath();

	void HandlePredictedPath();
	
	void UpdateProjectileLaunchValues();

protected:

	const FName EQUIPPED_SOCKET_NAME = "Throwable_Equip";
	const FName HOLSTER_SOCKET_NAME = "Throwable_Holster";

	// TODO : crawl�� �� �ٸ� socket ��ġ�� ����ؾ� ��
	const FName THROW_START_SOCKET_NAME = "Throwable_ThrowStart";

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EPoseState, FPriorityAnimMontage> DrawMontages{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EPoseState, FPriorityAnimMontage> SheathMontages{};

protected:

	// ���� �ڼ��� �´� Throw process ��Ÿ�ֵ�
	UPROPERTY(BlueprintReadOnly)
	FThrowProcessMontages CurThrowProcessMontages{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EPoseState, FThrowProcessMontages> ThrowProcessMontages{};

protected:

	// ���콺�� ������ �ִ� ����(OnGoing)
	bool bIsCharging{};

	UPROPERTY(BluePrintReadOnly)
	bool bIsOnThrowProcess{};

protected:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	class UShapeComponent* Collider{};

protected: // Projectile ����

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	class UProjectileMovementComponent* ProjectileMovement{};

	UPROPERTY(BlueprintReadOnly)
	FVector ProjStartLocation{};

	UPROPERTY(BlueprintReadOnly)
	FVector ProjLaunchVelocity{};

	float Speed = 1500.f;
	const float UP_DIR_BOOST_OFFSET  = 500.f;

protected:

	UPROPERTY(BlueprintReadOnly)
	bool bDrawPredictedPath{}; // Predicted Path�� �׷����ϴ��� üũ

	// ���� ��� �׸� �� ���
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UNiagaraSystem* NiagaraSystem{};

protected:

	// Testing ��, �賶�� ����ִ� ��ó�� �� ���� / TODO : �� ������� �����
	static TArray<AC_ThrowingWeapon*> ThrowablePool;
	
	static const UINT TESTPOOLCNT = 20;

};
