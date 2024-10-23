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

UENUM(BluePrintType)
enum class EThrowableType : uint8
{
	GRENADE,
	FLASH_BANG,
	SMOKE
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

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	/// <summary>
	/// Throwable의 Holster에 부착 시, visible을 끔
	/// </summary>
	bool AttachToHolster(class USceneComponent* InParent) override;

	bool AttachToHand(class USceneComponent* InParent) override;


	//UFUNCTION(BlueprintCallable)
	void PickUpItem(AC_BasicCharacter* Character) override;

	void DropItem(AC_BasicCharacter* Character) override;

	void SetItemStack(uint8 AddItemStack) override;

	void EquipToCharacter(AC_BasicCharacter* Character);

	bool MoveToInven(AC_BasicCharacter* Character) override;

	bool MoveToAround(AC_BasicCharacter* Character) override;

	bool MoveToSlot(AC_BasicCharacter* Character) override;

	bool Interaction(AC_BasicCharacter* Character) override;

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

	void SetIsCooked(bool IsCooked) { bIsCooked = IsCooked; }
	bool GetIsCooked() const { return bIsCooked; }

	class UShapeComponent* GetExplosionSphere() const { return ExplosionSphere; }

	class UParticleSystem* GetParticleExplodeEffect() const { return ParticleExplodeEffect; }
	class UNiagaraSystem* GetNiagaraExplodeEffect() const { return NiagaraExplodeEffect; }

public:

	UFUNCTION(BlueprintCallable)
	void InitExplodeStrategy(EThrowableType ThrowableType);

	/// <summary>
	/// 안전손잡이까지 날리기
	/// </summary>
	void StartCooking();

	/// <summary>
	/// <para> Cooking된 채로 땅에 내려놓기 / 이미 cooking이 시작되었을 때 무기를 바꾸거나, 손에 쥐고 있을 때 터졌을 때도 사용 </para>
	/// <para> 땅에 내려놓고 OnThrowProcessEnd 호출함으로써 다음 동작도 모두 처리 </para>
	/// </summary>
	/// <returns> 땅에 그냥 놓을 수 없는 경우 return false </returns>
	bool ReleaseOnGround();

private:

	/// <summary>
	/// 투척류 터치기
	/// </summary>
	void Explode();

protected:
	
	// Get Predicted Projectile path start location
	UFUNCTION(BlueprintCallable)
	FVector GetPredictedThrowStartLocation();

private:

	/// <summary>
	/// 투척 예상 경로 그리기 (디버깅 line)
	/// </summary>
	void DrawDebugPredictedPath();

	/// <summary>
	/// 투척 예상 경로 그리기 (실제 line)
	/// </summary>
	void DrawPredictedPath();

	void HandlePredictedPath();
	void UpdateProjectileLaunchValues();

protected:
	/// <summary>
	/// OwnerCharacter의 Pose Transition 모션이 끝났을 때 Delegate를 통해 call back을 받는 함수 (현재 캐릭터의 slot에 장착된 무기만 call back 될 예정) 
	/// </summary>
	void OnOwnerCharacterPoseTransitionFin() override;

public:

	UFUNCTION(BlueprintCallable)
	void ClearSpline();

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

private:

	FVector ProjStartLocation{};

	FVector ProjLaunchVelocity{};

	float Speed = 1500.f;
	const float UP_DIR_BOOST_OFFSET  = 500.f;

protected: // Predicted Path 관련

	class USplineComponent* PathSpline{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UStaticMeshComponent* PredictedEndPoint{};
	
	TArray<class USplineMeshComponent*> SplineMeshes{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UStaticMesh* SplineMesh{};

protected:

	// Testing 용, 배낭에 들어있는 것처럼 쓸 것임 / TODO : 이 멤버변수 지우기
	static TArray<AC_ThrowingWeapon*> ThrowablePool;
	
	static const UINT TESTPOOLCNT = 2;

private:

	// Predicted Path를 그릴 때, 던지기 자세에서의 socket위치를 파악하기 위함, 플레이어만 사용
	static class USkeletalMeshComponent* OwnerMeshTemp;

private:
	
	bool bIsCooked{};

	struct FTimerHandle TimerHandle{};

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float CookingTime = 5.f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TScriptInterface<class II_ExplodeStrategy> ExplodeStrategy{};

	//UPROPERTY(BlueprintReadWrite, EditAnywhere)

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UParticleSystem* ParticleExplodeEffect{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UNiagaraSystem* NiagaraExplodeEffect{};

protected:

	// 폭발 반경 범위 collider
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	class UShapeComponent* ExplosionSphere{};

};
