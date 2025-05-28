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

USTRUCT(BlueprintType)
struct FThrowingWeaponSoundData : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USoundBase* PinPullSound = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USoundBase* CookingSound = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USoundBase* ExplosionSound = nullptr;

};

/// <summary>
/// ThrowableType에 대한 Iterator 구현을 위한 연산자 오버로딩 (Prefix (++Type))
/// </summary>
/// <param name="Type"></param>
/// <returns></returns>
inline EThrowableType& operator++(EThrowableType& Type)
{
	uint8 TypeToInt = static_cast<uint8>(Type);
	uint8 MaxType   = static_cast<uint8>(EThrowableType::MAX);

	Type = (TypeToInt >= MaxType - 1) ? static_cast<EThrowableType>(0) :
									    static_cast<EThrowableType>(static_cast<uint8>(Type) + 1);   
	return Type;
}

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
	/// OwnerCharacter의 PoseState에 맞추어 CurMontage 맞추기 
	/// </summary>
	/// <return> : OwnerCharcter가 nullptr이면, return false </return>
	bool UpdateCurMontagesToOwnerCharacterPoseState();

private:
	
	/// <summary>
	/// 투척류 Cooking 처리된 이후 Handling 
	/// </summary>
	/// <param name="DeltaTime"></param>
	void HandleAfterCooked(float DeltaTime);

public:

	/// <summary>
	/// Throwable의 Holster에 부착 시, visible을 끔
	/// </summary>
	bool AttachToHolster(class USceneComponent* InParent) override;

	bool AttachToHand(class USceneComponent* InParent) override;

	void InitializeItem(FName NewItemCode) override;


public:
	
	static FName GetThrowableItemName(EThrowableType TargetType);	
	
	EThrowableType GetThrowableType() const { return ThrowableType; }

private:

	void DropItem(AC_BasicCharacter* Character) override;

	bool Interaction(AC_BasicCharacter* Character) override;

	AC_Item* SpawnItem(AC_BasicCharacter* Character) override;

protected:
	//bool MoveSlotToAround(AC_BasicCharacter* Character) override;
	//bool MoveSlotToInven(AC_BasicCharacter* Character) override;
	//
	//bool MoveInvenToAround(AC_BasicCharacter* Character) override;
	//bool MoveInvenToSlot(AC_BasicCharacter* Character) override;
	//
	//bool MoveAroundToInven(AC_BasicCharacter* Character) override;
	//bool MoveAroundToSlot(AC_BasicCharacter* Character) override;

	bool MoveSlotToAround(AC_BasicCharacter* Character, int32 InStack) override;
	bool MoveSlotToInven(AC_BasicCharacter* Character, int32 InStack) override;

	bool MoveInvenToAround(AC_BasicCharacter* Character, int32 InStack) override;
	bool MoveInvenToSlot(AC_BasicCharacter* Character, int32 InStack) override;

	bool MoveAroundToInven(AC_BasicCharacter* Character, int32 InStack) override;
	bool MoveAroundToSlot(AC_BasicCharacter* Character, int32 InStack) override;

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

	void SetProjectileStartLocation(const FVector& InLocation) { ProjStartLocation = InLocation; }
	void SetProjectileLaunchVelocity(const FVector& LaunchVelocity) { ProjLaunchVelocity = LaunchVelocity; }
	float GetSpeed() const { return Speed; }
	static float GetProjectileRadius() { return PROJECTILE_RADIUS; }
	//EThrowableType GetThrowableType() const { return ThrowableType; }

	const FThrowingWeaponSoundData* GetThrowingWeaponSoundData() { return ThrowingWeaponSoundData; }

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

public:

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

	/// <summary>
	/// Player의 투척 예상 경로 실시간으로 그리기(Tick)
	/// </summary>
	void HandlePlayerPredictedPath();


	/// <summary>
	/// <para> Player의 Projectile발사 관련 값들 초기화 </para>
	/// <para> (ProjStartLocation & ProjLaunchVelocity) 초기화 </para>
	/// </summary>
	void UpdatePlayerProjectileLaunchValues();

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

private:
	
	/// <summary>
	/// 현재 손에 쥐고 있는 무기가 Inven에 추가 또는 삭제한 투척류 종류일 때, MagazineText 업데이트 처리 (Boiler-plate code 정리)
	/// </summary>
	void TryUpdateAmmoWidgetMagazineTextIfNecessary(class AC_Player* Player);

	/// <summary>
	/// OwnerPlayer의 Magazine Text(개수) 업데이트하기 (주의 : OwnerPlayer가 Valid할 때에만 사용)
	/// </summary>
	void UpdateAmmoWidgetMagazineText(AC_Player* Player);
	
protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	EThrowableType ThrowableType{};

protected:

	static const TMap<EThrowableType, FName> EQUIPPED_SOCKET_NAMES;

	static const FName HOLSTER_SOCKET_NAME;
	static const FName THROW_START_SOCKET_NAME; // TempMesh의 위치로 Throwable Start 위치 잡는 Socket & 실질적으로 Throw Throwable할 때 Projectile위치를 이 Socket으로 잡음

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
	static const float PROJECTILE_RADIUS;

protected: // Predicted Path 관련

	class USplineComponent* PathSpline{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UStaticMeshComponent* PredictedEndPoint{};

	TArray<class USplineMeshComponent*> SplineMeshes{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UStaticMesh* SplineMesh{};

private:

	//  GameScene에 배치된 총 ThrowingWeapon 개수 -> OwnerMeshTemp Destroy 처리 시 사용 예정
	static int ThrowingWeaponCount;

private:

	bool bIsCooked{};
	bool bExploded{};

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float CookingTime = 5.f;

private:

	float CookingTimer{}; // Cooking 시작 되었을 때 Count될 Timer

	//protected:
	//
	//	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	//	TScriptInterface<class II_ExplodeStrategy> ExplodeStrategy{};

private:

	static TMap<EThrowableType, class II_ExplodeStrategy*> ExplodeStrategies;
	II_ExplodeStrategy* ExplodeStrategy{};

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

	static const TMap<EThrowableType, FName> THROWABLETYPE_ITEMNAME_MAP;

private:

	// 각 Throwable 종류 별 AI Attack 전략
	static TMap<EThrowableType, class II_AIThrowableAttackStrategy*> AIAttackStrategies;

	// 현 Throwable 종류에 따른 AI Attack 전략
	class II_AIThrowableAttackStrategy* AIAttackStrategy{};

protected:
	const FThrowingWeaponSoundData* ThrowingWeaponSoundData = nullptr;
};



