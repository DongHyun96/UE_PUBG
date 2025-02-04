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
	/// Throwable�� Holster�� ���� ��, visible�� ��
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
		/// �ش� ������(��ü)�� �󸶳� �κ��� ���� �� �ִ��� ����ؼ� �ִ´�. -> �Ϻθ� �ִ� ��쿡�� �κ��� ���� �� Spawn�� ���� ���ο� ��ü�� ����� 
		/// ������ �������ְ� ������ ������ ����. (����stack = ������ ������ stack + ������ü�� stack)
		/// </summary>
		/// <param name="Character"></param>
		/// <returns></returns>
	bool LegacyMoveToInven(AC_BasicCharacter* Character) override;

	/// <summary>
	/// �ش� ������(��ü)�� �󸶳� ���� ������ ����ؼ� ������.(�̰��� �Ŀ� ���������� ����� ���� uint8������ �޾� ����ϸ� ������.)
	/// ->�Ϻθ� ������ ��� ������(��ü)�� ���� �����ؼ� ������ ������ �����ؾ� �Ѵ�. (����stack = ������ ������ stack + ������ü�� stack)
	/// </summary>
	/// <param name="Character"></param>
	/// <returns></returns>
	bool LegacyMoveToAround(AC_BasicCharacter* Character) override;

	/// <summary>
	/// �������� �������� �̵�.
	/// </summary>
	/// <param name="Character"></param>
	/// <returns>true�� �̵� ����, false�� ����</returns>
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

	void SetIsCooked(bool IsCooked) { bIsCooked = IsCooked; }
	bool GetIsCooked() const { return bIsCooked; }

	class UShapeComponent* GetExplosionSphere() const { return ExplosionSphere; }

	class UParticleSystem* GetParticleExplodeEffect() const { return ParticleExplodeEffect; }
	class UNiagaraSystem* GetNiagaraExplodeEffect() const { return NiagaraExplodeEffect; }

public:

	/// <summary>
	/// ���������̱��� ������
	/// </summary>
	void StartCooking();

	/// <summary>
	/// <para> Cooking�� ä�� ���� �������� / �̹� cooking�� ���۵Ǿ��� �� ���⸦ �ٲٰų�, �տ� ��� ���� �� ������ ���� ��� </para>
	/// <para> ���� �������� OnThrowProcessEnd ȣ�������ν� ���� ���۵� ��� ó�� </para>
	/// </summary>
	/// <returns> ���� �׳� ���� �� ���� ��� return false </returns>
	bool ReleaseOnGround();

private:

	/// <summary>
	/// ��ô�� ��ġ��
	/// </summary>
	void Explode();

protected:

	// Get Predicted Projectile path start location
	UFUNCTION(BlueprintCallable)
	FVector GetPredictedThrowStartLocation();

private:

	/// <summary>
	/// ��ô ���� ��� �׸��� (����� line)
	/// </summary>
	void DrawDebugPredictedPath();

	/// <summary>
	/// ��ô ���� ��� �׸��� (���� line)
	/// </summary>
	void DrawPredictedPath();

	void HandlePredictedPath();
	void UpdateProjectileLaunchValues();

protected:
	/// <summary>
	/// OwnerCharacter�� Pose Transition ����� ������ �� Delegate�� ���� call back�� �޴� �Լ� (���� ĳ������ slot�� ������ ���⸸ call back �� ����) 
	/// </summary>
	void OnOwnerCharacterPoseTransitionFin() override;

public:

	UFUNCTION(BlueprintCallable)
	void ClearSpline();

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

private:

	FVector ProjStartLocation{};

	FVector ProjLaunchVelocity{};

	float Speed = 1500.f;
	static const float UP_DIR_BOOST_OFFSET;

protected: // Predicted Path ����

	class USplineComponent* PathSpline{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UStaticMeshComponent* PredictedEndPoint{};

	TArray<class USplineMeshComponent*> SplineMeshes{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UStaticMesh* SplineMesh{};

private:

	// Predicted Path�� �׸� ��, ������ �ڼ������� socket��ġ�� �ľ��ϱ� ����, �÷��̾ ���
	static class USkeletalMeshComponent* OwnerMeshTemp;

	//  GameScene�� ��ġ�� �� ThrowingWeapon ���� -> OwnerMeshTemp Destroy ó�� �� ��� ����
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

	// ���� �ݰ� ���� collider
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	class UShapeComponent* ExplosionSphere{};

private:

	static const TMap<EThrowableType, FString> THROWABLETYPE_ITEMNAME_MAP;

public:
	virtual bool ExecuteAIAttack(class AC_BasicCharacter* InTargetCharacter) override;
};
