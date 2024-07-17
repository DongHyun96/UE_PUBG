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
	/// TODO : 이 함수 지우기
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

private:

	/// <summary>
	/// 수류탄 투척 예상 경로 그리기 (디버깅 line)
	/// </summary>
	void DrawDebugPredictedPath();

	/// <summary>
	/// 수류탄 투척 예상 경로 그리기 (Niagara line) - USE THIS IN REAL PLAY
	/// </summary>
	void DrawNiagaraPredictedPath();

	void HandlePredictedPath();
	
	void UpdateProjectileLaunchValues();

protected:

	const FName EQUIPPED_SOCKET_NAME = "Throwable_Equip";
	const FName HOLSTER_SOCKET_NAME = "Throwable_Holster";

	// TODO : crawl은 또 다른 socket 위치를 사용해야 함
	const FName THROW_START_SOCKET_NAME = "Throwable_ThrowStart";

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

	UPROPERTY(BlueprintReadOnly)
	FVector ProjStartLocation{};

	UPROPERTY(BlueprintReadOnly)
	FVector ProjLaunchVelocity{};

	float Speed = 1500.f;
	const float UP_DIR_BOOST_OFFSET  = 500.f;

protected:

	UPROPERTY(BlueprintReadOnly)
	bool bDrawPredictedPath{}; // Predicted Path를 그려야하는지 체크

	// 예상 경로 그릴 때 사용
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UNiagaraSystem* NiagaraSystem{};

protected:

	// Testing 용, 배낭에 들어있는 것처럼 쓸 것임 / TODO : 이 멤버변수 지우기
	static TArray<AC_ThrowingWeapon*> ThrowablePool;
	
	static const UINT TESTPOOLCNT = 20;

};
