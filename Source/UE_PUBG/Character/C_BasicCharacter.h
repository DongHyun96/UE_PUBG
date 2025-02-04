// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
//#include "Component/C_InvenComponent.h"

#include "Component/C_StatComponent.h"
#include "GenericTeamAgentInterface.h"
#include "C_BasicCharacter.generated.h"

DECLARE_MULTICAST_DELEGATE(FDele_PoseTransitionFin);

UENUM(BlueprintType)
enum class EMainState : uint8
{
	IDLE,
	SKYDIVING,
	DEAD,
	MAX
};

UENUM(BlueprintType)
enum class EHandState : uint8
{
	UNARMED,
	WEAPON_GUN,
	WEAPON_MELEE,
	WEAPON_THROWABLE,
	HANDSTATE_MAX
};

UENUM(BlueprintType)
enum class EPoseState : uint8
{
	STAND,
	CROUCH,
	CRAWL,
	POSE_MAX
};

UENUM(BlueprintType)
enum class EMontagePriority : uint8
{
	TURN_IN_PLACE,			// Default Full body + Sub Lower
	USE_CONSUMABLE,			// Default Upper body
	ATTACK,					// Default Upper body
	DRAW_SHEATH_WEAPON,		// Default Upper body
	POSE_TRANSITION,		// Sub Full body
	THROW_THROWABLE,		// ���������� ������ �ڼ��� ��, Default Upper body
	PRIORITY_MAX,
	MAX
};

USTRUCT(BlueprintType)
struct FPriorityAnimMontage : public FTableRowBase
{
	GENERATED_BODY()

	FPriorityAnimMontage() {}

	FPriorityAnimMontage(UAnimMontage* InAnimMontage, EMontagePriority InPriority)
		:AnimMontage(InAnimMontage), Priority(InPriority) {}

public:
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UAnimMontage* AnimMontage{};

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	EMontagePriority Priority{};

};

/// <summary>
/// �ڼ� �� ��ȯ ���̸� �Ų� Anim Montage��
/// </summary>
USTRUCT(BlueprintType)
struct FPoseTransitionMontages
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FPriorityAnimMontage StandToCrawl{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FPriorityAnimMontage CrawlToStand{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FPriorityAnimMontage CrouchToCrawl{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FPriorityAnimMontage CrawlToCrouch{};
};


UCLASS()
class UE_PUBG_API AC_BasicCharacter : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AC_BasicCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	/// <summary>
	/// Deprecated in current UE_PUBG Project : Priority�� ������ AC_PriorityAnimMontage�� AnimMontage ����ؾ� ��
	/// </summary>
	float PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None) override;

protected:
	/// <summary>
	/// ������ ��ȣ�ۿ��� ���� ������ �Լ�.
	/// ���� ���� �����۰��� �浿�� �����ϴ� �Լ�.
	/// </summary>
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* DetectionSphere;

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
public:

	/// <summary>
	/// ��Ÿ�� ��� �켱������ ���� PlayAnimMontage �Լ�
	/// </summary>
	/// <param name="PAnimMontage"> : Priority ����� AnimMontage </param>
	/// <returns> Animation Montage Duration </returns>
	float PlayAnimMontage(const FPriorityAnimMontage& PAnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None);

protected:

	/// <summary>
	/// <para> UGameplayStatics::ApplyDamage�� ���� Damage�� �޴� �Լ� </para>
	/// <para> ������ �ǰ� ó�� �õ��� �Ϸ��� StatComponent�� TakeDamage�Լ� ���� </para>
	/// </summary>
	/// <param name="DamageAmount"> : Damage �� </param>
	/// <param name="DamageEvent"></param>
	/// <param name="EventInstigator"></param>
	/// <param name="DamageCauser"></param>
	/// <returns> : The amount of damage actually applied. </returns>
	float TakeDamage
	(
		float				DamageAmount,
		FDamageEvent const& DamageEvent,
		AController*		EventInstigator,
		AActor*				DamageCauser
	) override;
	
	//�ڽ� �ܰ迡�� OverlapBegin���� ����� �Լ� Template method
	virtual void HandleOverlapBegin(AActor* OtherActor);

	//�ڽ� �ܰ迡�� OverlapBegin���� ����� �Լ� Template method
	virtual void HandleOverlapEnd(AActor* OtherActor);

public:

	/// <summary>
	/// Pose�� ĳ���� �̵����⿡ ���� MaxWalkSpeed ����
	/// </summary>
	/// <param name="MovementVector"> : Input action movement vector </param>
	void UpdateMaxWalkSpeed(const FVector2D& MovementVector);


public: // Getters and setters

	void SetMainState(EMainState InMainState) { MainState = InMainState; }
	EMainState GetMainState() const { return MainState; }

	EHandState GetHandState() const { return HandState; }
	EPoseState GetPoseState() const { return PoseState; }
	void SetHandState(EHandState InHandState) { HandState = InHandState; }

protected:
	void SetPoseState(EPoseState InPoseState);

public:
	/// <summary>
	/// �ڼ� �ٲٱ� ���� ó��
	/// </summary>
	/// <param name="InChangeFrom"> : �ٲٱ� �� �ڼ� </param>
	/// <param name="InChangeTo"> : �ٲ� �ڼ� </param>
	/// <returns> : ����� �ٲپ��ٸ� return true </returns>
	virtual bool SetPoseState(EPoseState InChangeFrom, EPoseState InChangeTo);

	float GetNextSpeed() const { return NextSpeed; }
	void SetNextSpeed(float InNextSpeed) { NextSpeed = InNextSpeed; }
	bool GetIsJumping() const { return bIsJumping; }
	UFUNCTION(BlueprintCallable)
	class UC_EquippedComponent* GetEquippedComponent() const { return EquippedComponent; }

	void SetCanMove(bool InCanMove) { bCanMove = InCanMove; }

	bool GetCanMove() const { return bCanMove; }
	bool GetIsAimDown() { return bIsAimDownSight; }
	bool GetIsWatchingSight() { return bIsWatchingSight; }
	bool GetIsFiringBullet() { return bIsFiringBullet; }
	bool GetCanFireBullet() { return bCanFireBullet; }
	virtual bool GetIsHighEnoughToFall();
	void SetIsAimDown(bool InIsAimDownSight) { bIsAimDownSight = InIsAimDownSight; }
	void SetIsWatchingSight(bool InIsWatchingSight) { bIsWatchingSight = InIsWatchingSight; }
	void SetIsJumping(bool InIsJumping) { bIsJumping = InIsJumping; }

	
	UFUNCTION(BlueprintCallable)
	class UC_InvenComponent* GetInvenComponent() { return Inventory; }

	void SetIsHoldDirection(bool InIsHoldDirection) { bIsHoldDirection = InIsHoldDirection; }
	bool GetIsHoldDirection() const { return bIsHoldDirection; }

	bool GetIsPoseTransitioning() const { return bIsPoseTransitioning; }

	UC_StatComponent* GetStatComponent() const { return StatComponent; }

	void SetBoostingSpeedFactor(const float& InBoostingSpeedFactor) { BoostingSpeedFactor = InBoostingSpeedFactor; }

	void SetIsActivatingConsumableItem(bool InIsActivatingConsumableItem, class AC_ConsumableItem* ActivatingConsumableItem);
	bool GetIsActivatingConsumableItem() const { return bIsActivatingConsumableItem; }

	AC_ConsumableItem* GetCurActivatingConsumableItem() const { return CurActivatingConsumableItem; }

	void SetIsAltPressed(bool InIsAltPressed) { bIsAltPressed = InIsAltPressed; }
	bool GetIsAltPressed() const { return bIsAltPressed; }

	FPoseTransitionMontages GetPoseTransitionMontagesByHandState(EHandState InHandState) { return PoseTransitionMontages[InHandState]; }

	FRotator GetCharacterMovingDirection() const { return CharacterMovingDirection; }
	void SetCharacterMovingDirection(FRotator InCharacterMovingDirection) { CharacterMovingDirection = InCharacterMovingDirection; }

	bool GetIsWalking() const { return bIsWalking; }
	void SetIsWalking(bool InIsWalking) { bIsWalking = InIsWalking; }
	bool GetIsSprinting() const { return bIsSprinting; }
	void SetIsSprinting(bool InIsSprinting) { bIsSprinting = InIsSprinting; }
	bool GetIsReloadingBullet() { return bIsReloadingBullet; }
	void SetIsReloadingBullet(bool bInIsReloading) { bIsReloadingBullet = bInIsReloading; }
	UFUNCTION(BlueprintCallable)
	UC_InvenComponent* GetInventory() const { return Inventory; }

	class UC_ConsumableUsageMeshComponent* GetConsumableUsageMeshComponent() const { return ConsumableUsageMeshComponent; }

	class UC_PoseColliderHandlerComponent* GetPoseColliderHandlerComponent() const { return PoseColliderHandlerComponent; }

	class UC_SwimmingComponent* GetSwimmingComponent() const { return SwimmingComponent; }

	UFUNCTION(BlueprintGetter)
	class UC_SkyDivingComponent* GetSkyDivingComponent() const { return SkyDiveComponent; }

	//UFUNCTION(BlueprintCallable)
	//class UC_InvenSystem* GetInvenSystem() { return InvenSystem; }

	class UC_AttachableItemMeshComponent* GetAttachmentMeshComponent() { return AttachmentMeshComponent; };

	class UC_ParkourComponent* GetParkourComponent() const { return ParkourComponent; }

	class UMotionWarpingComponent* GetMotionWarpingComponent() const { return MotionWarpingComponent; }

public:

	/// <summary>
	/// Pose Transition montage�� ���� ���� �߰��� Call	�Ǵ� �Լ�
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void OnPoseTransitionGoing();

	/// <summary>
	/// Pose Transition Montage�� ������ Callback�Ǵ� �Լ�
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void OnPoseTransitionFinish();

public:

	/// <summary>
	/// Pose Transition ��� �����ϱ�
	/// </summary>
	/// <param name="TransitionMontage"> : Pose Transition motion </param>
	/// <param name="InNextPoseState"> : ���� �ڼ� </param>
	/// <returns> Pose transition motion�� ����� ����Ǿ��ٸ� return true </returns>
	bool ExecutePoseTransitionAction(const FPriorityAnimMontage& TransitionMontage, EPoseState InNextPoseState);

public:

	FGenericTeamId GetGenericTeamId() const override { return FGenericTeamId(TeamID); }

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	EMainState MainState{};

	// Current hand state
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EHandState HandState{};

	// Current pose state 
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EPoseState PoseState{};

protected:

	// �ڼ��� Collider ��ġ, ũ�� �� Mesh ��ġ ����ִ� Component
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UC_PoseColliderHandlerComponent* PoseColliderHandlerComponent{};

protected: // �ڼ� ��ȯ Transition ����

	// �ڼ� ��ȯ�� �� �ÿ�, �������� ��ȯ�� Pose State
	UPROPERTY(BlueprintReadOnly)
	EPoseState NextPoseState{};

	// Crawl �ڼ��� ������ ��ȯ �ڼ��� ���� ���� -> Crouch to stand, stand to crouch�� �׳� ��ȯ�ص� ������� ����
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EHandState, FPoseTransitionMontages> PoseTransitionMontages{};

	// ���� Pose Transition ����� ����������
	bool bIsPoseTransitioning{};

protected: // Sprint walk state

	bool bIsWalking{};
	bool bIsSprinting{};

public:
	// OnTransitionFinish���� ȣ��� Multicast Delegate
	FDele_PoseTransitionFin Delegate_OnPoseTransitionFin;


	float GetDistanceToGround() { return DistanceToGround; }

protected:

	// AnimCharacter���� ������ Speed�� ���� Lerp destination ��
	float NextSpeed{};
	// AltŰ ������ �ִ��� üũ
	bool bIsHoldDirection = false;
	// AltŰ ���Ⱦ����� üũ
	bool bIsAltPressed = false;
	bool bCanFireBullet = true;
	bool bIsAimDownSight = false;
	bool bIsAimingRifle = false;
	bool bIsWatchingSight = false;
	bool bIsReloadingBullet = false;
	bool bIsFiringBullet = false;
	FRotator CharacterMovingDirection;
	float DistanceToGround{};
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bCanMove = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsJumping = false;

protected:
	// ������ ���� �� �屸�� component
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UC_EquippedComponent* EquippedComponent{};

	// Stat���� Component (ex HP)
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UC_StatComponent* StatComponent{};

	//UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	//class UC_InvenSystem* InvenSystem{};

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UC_InvenComponent* Inventory{};

protected: // PriorityAnimMontage ����

	// ���� ��� ����, �Ǵ� ������ ����� PriorityAnimMontage |<GroupName, AnimMontage>
	UPROPERTY(BlueprintReadOnly)
	TMap<FName, FPriorityAnimMontage> CurPriorityAnimMontageMap{};
	

protected: // Consumable ����
	
	// Boosting�� ���� ĳ���� �ӷ� ���� factor
	float BoostingSpeedFactor = 1.f;
	
	// ���� Consumable Item Activating ���� ��������
	bool bIsActivatingConsumableItem{};

	// ���� Ȱ��ȭ ���� Consumable Item
	class AC_ConsumableItem* CurActivatingConsumableItem{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UC_ConsumableUsageMeshComponent* ConsumableUsageMeshComponent{};


protected: // �Ѿ� Object Pooling(AC_Item���� ������� Bullet�� ���X)
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<class AC_Item*> Bullets;

	int SevenmmBulletCount = 0;
	int FivemmBulletCount  = 0;

protected:
	//�Ѿ� Object Pooling (World���� �۾��� ����)
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TArray<class AC_Bullet*> PooledBullets;

	void PoolingBullets();

public:
	TArray<AC_Bullet*>& GetBullets() { return PooledBullets; }
	int GetCurrentSevenmmBulletCount() { return SevenmmBulletCount; }
	int GetCurrentFivemmBulletCount()  { return FivemmBulletCount;}
	void AddSevenmmBulletStack(int inBulletCount);
	void AddFivemmBulletStack(int inBulletCount);
protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UC_SwimmingComponent* SwimmingComponent{};

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UC_SkyDivingComponent* SkyDiveComponent{};

protected:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UC_AttachableItemMeshComponent* AttachmentMeshComponent{};

protected: // ���� ���� Components

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UC_ParkourComponent* ParkourComponent{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UMotionWarpingComponent* MotionWarpingComponent{};

protected: // AI �Ǿ� �ĺ� ����
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TEnumAsByte<ETeamAttitude::Type> TeamID = ETeamAttitude::Neutral;
};
