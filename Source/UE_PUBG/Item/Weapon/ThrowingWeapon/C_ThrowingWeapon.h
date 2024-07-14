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
	/// Throwable의 Holster에 부착 시, visible을 끔
	/// </summary>
	bool AttachToHolster(class USceneComponent* InParent) override;


	bool AttachToHand(class USceneComponent* InParent) override;



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
	/// 실질적으로 던지기 처리
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

protected:

	const FName EQUIPPED_SOCKET_NAME = "Throwable_Equip";
	const FName HOLSTER_SOCKET_NAME = "Throwable_Holster";

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EPoseState, FPriorityAnimMontage> DrawMontages{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EPoseState, FPriorityAnimMontage> SheathMontages{};

protected:

	// 현재 자세에 맞는 Throw process 몽타주들
	UPROPERTY(BlueprintReadOnly)
	FThrowProcessMontages CurThrowProcessMontages{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EPoseState, FThrowProcessMontages> ThrowProcessMontages{};

protected:

	// 마우스를 누르고 있는 상태(OnGoing)
	bool bIsCharging{};

	UPROPERTY(BluePrintReadOnly)
	bool bIsOnThrowProcess{};

protected:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	class UShapeComponent* Collider{};

protected: // Projectile 관련

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	class UProjectileMovementComponent* ProjectileMovement{};

	FVector Direction{};

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	float Speed = 2000.f;

};
