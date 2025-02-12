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
	SMOKE,
	MAX
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

private:

	//UFUNCTION(BlueprintCallable)
	void PickUpItem(AC_BasicCharacter* Character) override;

	void DropItem(AC_BasicCharacter* Character) override;

	//void SetItemStack(uint8 ItemStack) override;

	void EquipToCharacter(AC_BasicCharacter* Character);
	//protected:
		/// <summary>
		/// 해당 아이템(객체)를 얼마나 인벤에 넣을 수 있는지 계산해서 넣는다. -> 일부만 넣는 경우에는 인벤에 넣을 때 Spawn을 통해 새로운 객체를 만들어 
		/// 갯수를 설정해주고 원본의 갯수도 수정. (원본stack = 수정된 원본의 stack + 생성객체의 stack)
		/// </summary>
		/// <param name="Character"></param>
		/// <returns></returns>
	bool LegacyMoveToInven(AC_BasicCharacter* Character) override;

	/// <summary>
	/// 해당 아이템(객체)를 얼마나 버릴 것인지 계산해서 버린다.(이것은 후에 나눠버리기 기능을 위해 uint8정도를 받아 사용하면 좋을듯.)
	/// ->일부만 버리는 경우 아이템(객체)를 새로 생성해서 원본과 수량을 조정해야 한다. (원본stack = 수정된 원본의 stack + 생성객체의 stack)
	/// </summary>
	/// <param name="Character"></param>
	/// <returns></returns>
	bool LegacyMoveToAround(AC_BasicCharacter* Character) override;

	/// <summary>
	/// 아이템을 슬롯으로 이동.
	/// </summary>
	/// <param name="Character"></param>
	/// <returns>true면 이동 성공, false면 실패</returns>
	bool LegacyMoveToSlot(AC_BasicCharacter* Character) override;


	bool Interaction(AC_BasicCharacter* Character) override;

	AC_Item* SpawnItem(AC_BasicCharacter* Character) override;

protected:
	bool MoveSlotToAround(AC_BasicCharacter* Character) override;
	bool MoveSlotToInven(AC_BasicCharacter* Character) override;
	bool MoveSlotToSlot(AC_BasicCharacter* Character) override;

	bool MoveInvenToAround(AC_BasicCharacter* Character) override;
	bool MoveInvenToInven(AC_BasicCharacter* Character) override;
	bool MoveInvenToSlot(AC_BasicCharacter* Character) override;

	bool MoveAroundToAround(AC_BasicCharacter* Character) override;
	bool MoveAroundToInven(AC_BasicCharacter* Character) override;
	bool MoveAroundToSlot(AC_BasicCharacter* Character) override;

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

	//EThrowableType GetThrowableType() const { return ThrowableType; }

public:

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

public:
	bool ExecuteAIAttack(class AC_BasicCharacter* InTargetCharacter) override;
	
	bool ExecuteAIAttackTickTask(class AC_BasicCharacter* InTargetCharacter, const float& DeltaTime) override;
	
protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	EThrowableType ThrowableType{};

protected:

	static const TMap<EThrowableType, FName> EQUIPPED_SOCKET_NAMES;

	static const FName HOLSTER_SOCKET_NAME;
	static const FName THROW_START_SOCKET_NAME;

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
	static const float UP_DIR_BOOST_OFFSET;

protected: // Predicted Path 관련

	class USplineComponent* PathSpline{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UStaticMeshComponent* PredictedEndPoint{};

	TArray<class USplineMeshComponent*> SplineMeshes{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UStaticMesh* SplineMesh{};

private:

	// Predicted Path를 그릴 때, 던지기 자세에서의 socket위치를 파악하기 위함, 플레이어만 사용
	static class USkeletalMeshComponent* OwnerMeshTemp;

	//  GameScene에 배치된 총 ThrowingWeapon 개수 -> OwnerMeshTemp Destroy 처리 시 사용 예정
	static int ThrowingWeaponCount;

private:

	bool bIsCooked{};

	struct FTimerHandle TimerHandle {};

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float CookingTime = 5.f;

	//protected:
	//
	//	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	//	TScriptInterface<class II_ExplodeStrategy> ExplodeStrategy{};

private:

	static TMap<EThrowableType, class II_ExplodeStrategy*> ExplodeStrategies;
	class II_ExplodeStrategy* ExplodeStrategy{};

protected:

	//UPROPERTY(BlueprintReadWrite, EditAnywhere)

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UParticleSystem* ParticleExplodeEffect{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UNiagaraSystem* NiagaraExplodeEffect{};

protected:

	// 폭발 반경 범위 collider
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	class UShapeComponent* ExplosionSphere{};

private:

	static const TMap<EThrowableType, FString> THROWABLETYPE_ITEMNAME_MAP;

private:

	// 각 Throwable 종류 별 AI Attack 전략
	static TMap<EThrowableType, class II_AIThrowableAttackStrategy*> AIAttackStrategies;

	// 현 Throwable 종류에 따른 AI Attack 전략
	class II_AIThrowableAttackStrategy* AIAttackStrategy{};

};



