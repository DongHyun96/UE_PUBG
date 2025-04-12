// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
//#include "Component/C_InvenComponent.h"

#include "Component/C_StatComponent.h"
#include "GenericTeamAgentInterface.h"
#include "C_BasicCharacter.generated.h"

DECLARE_MULTICAST_DELEGATE(FDele_PoseTransitionFin);

DECLARE_MULTICAST_DELEGATE_OneParam(FDele_CharacterDead, AC_BasicCharacter*);

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
	THROW_THROWABLE,		// 실질적으로 던지는 자세일 때, Default Upper body
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
/// 자세 별 전환 사이를 매꿀 Anim Montage들
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

	/// <summary>
	/// Enemy AI Controller가 Team인지 아닌지 인식할 때 사용 -> 여기서는 무조건 모두 적으로 간주(Enemy AI 서로도 적으로 간주)
	/// </summary>
	/// <param name="Other"></param>
	/// <returns></returns>
	ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override { return ETeamAttitude::Hostile; }

private:

	/// <summary>
	/// Deprecated in current UE_PUBG Project : Priority를 적용한 AC_PriorityAnimMontage로 AnimMontage 재생해야 함
	/// </summary>
	float PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None) override;

protected:
	/// <summary>
	/// 아이템 상호작용을 위한 변수와 함수.
	/// 구를 통해 아이템과의 충동을 감지하는 함수.
	/// </summary>
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* DetectionSphere;

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
public:

	/// <summary>
	/// 몽타주 재생 우선순위에 따른 PlayAnimMontage 함수
	/// </summary>
	/// <param name="PAnimMontage"> : Priority 적용된 AnimMontage </param>
	/// <returns> Animation Montage Duration </returns>
	float PlayAnimMontage(const FPriorityAnimMontage& PAnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None);

	UFUNCTION(BlueprintCallable)
	void CharacterDead();

	void EnableRagdoll();

protected:

	/// <summary>
	/// <para> UGameplayStatics::ApplyDamage를 통해 Damage를 받는 함수 </para>
	/// <para> 부위별 피격 처리 시도를 하려면 StatComponent의 TakeDamage함수 참고 </para>
	/// </summary>
	/// <param name="DamageAmount"> : Damage 양 </param>
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
	
	//자식 단계에서 OverlapBegin에서 사용할 함수 Template method
	virtual void HandleOverlapBegin(AActor* OtherActor);

	//자식 단계에서 OverlapBegin에서 사용할 함수 Template method
	virtual void HandleOverlapEnd(AActor* OtherActor);

	void DestroyCharacter();
public:

	/// <summary>
	/// Pose와 캐릭터 이동방향에 따른 MaxWalkSpeed 조정
	/// </summary>
	/// <param name="MovementVector"> : Input action movement vector </param>
	virtual void UpdateMaxWalkSpeed(const FVector2D& MovementVector);


public: // Getters and setters

	void SetMainState(EMainState InMainState) { MainState = InMainState; }
	EMainState GetMainState() const { return MainState; }

	EHandState GetHandState() const { return HandState; }
	EPoseState GetPoseState() const { return PoseState; }
	void SetHandState(EHandState InHandState) { HandState = InHandState; }

	void SetIsHitting(bool bHit) { bIsHitting = bHit; }

protected:
	void SetPoseState(EPoseState InPoseState);

public:
	/// <summary>
	/// 자세 바꾸기 통합 처리
	/// </summary>
	/// <param name="InChangeFrom"> : 바꾸기 전 자세 </param>
	/// <param name="InChangeTo"> : 바꿀 자세 </param>
	/// <returns> : 제대로 바꾸었다면 return true </returns>
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
	virtual bool GetIsTooCloseToAimGun();
	void SetIsAimDown(bool InIsAimDownSight) { bIsAimDownSight = InIsAimDownSight; }
	void SetIsWatchingSight(bool InIsWatchingSight) { bIsWatchingSight = InIsWatchingSight; }
	void SetIsJumping(bool InIsJumping) { bIsJumping = InIsJumping; }

	
	UFUNCTION(BlueprintCallable)
	class UC_InvenComponent* GetInvenComponent() { return InvenComponent; }


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

	class UC_ConsumableUsageMeshComponent* GetConsumableUsageMeshComponent() const { return ConsumableUsageMeshComponent; }

	class UC_PoseColliderHandlerComponent* GetPoseColliderHandlerComponent() const { return PoseColliderHandlerComponent; }

	class UC_SwimmingComponent* GetSwimmingComponent() const { return SwimmingComponent; }

	UFUNCTION(BlueprintGetter)
	class UC_SkyDivingComponent* GetSkyDivingComponent() const { return SkyDivingComponent; }

	class UC_AttachableItemMeshComponent* GetAttachmentMeshComponent() { return AttachmentMeshComponent; };

	class UC_ParkourComponent* GetParkourComponent() const { return ParkourComponent; }

	class UMotionWarpingComponent* GetMotionWarpingComponent() const { return MotionWarpingComponent; }

public:

	/// <summary>
	/// Pose Transition montage가 진행 중인 중간에 Call	되는 함수
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void OnPoseTransitionGoing();

	/// <summary>
	/// Pose Transition Montage가 끝나고 Callback되는 함수
	/// </summary>
	UFUNCTION(BlueprintCallable)
	virtual void OnPoseTransitionFinish();

public:

	/// <summary>
	/// Pose Transition 모션 실행하기
	/// </summary>
	/// <param name="TransitionMontage"> : Pose Transition motion </param>
	/// <param name="InNextPoseState"> : 다음 자세 </param>
	/// <returns> Pose transition motion이 제대로 실행되었다면 return true </returns>
	bool ExecutePoseTransitionAction(const FPriorityAnimMontage& TransitionMontage, EPoseState InNextPoseState);

public:

	FGenericTeamId GetGenericTeamId() const override { return FGenericTeamId(TeamID); }

public:

	void Jump() override;

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

	// 자세별 Collider 위치, 크기 및 Mesh 위치 잡아주는 Component
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UC_PoseColliderHandlerComponent* PoseColliderHandlerComponent{};

protected: // 자세 변환 Transition 관련

	// 자세 전환을 할 시에, 다음으로 전환될 Pose State
	UPROPERTY(BlueprintReadOnly)
	EPoseState NextPoseState{};

	// Crawl 자세에 관련한 전환 자세만 취할 것임 -> Crouch to stand, stand to crouch는 그냥 전환해도 어색하지 않음
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EHandState, FPoseTransitionMontages> PoseTransitionMontages{};

	// 현재 Pose Transition 모션이 진행중인지
	bool bIsPoseTransitioning{};

protected: // Sprint walk state

	bool bIsWalking{};
	bool bIsSprinting{};

public:
	// OnTransitionFinish에서 호출될 Multicast Delegate
	FDele_PoseTransitionFin Delegate_OnPoseTransitionFin;

	// CharacterDead에서 호출될 Multicast Delegate
	FDele_CharacterDead Delegate_OnCharacterDead;

protected:

	// AnimCharacter에서 참조할 Speed의 다음 Lerp destination 값
	float NextSpeed{};
	// Alt키 누르고 있는지 체크
	bool bIsHoldDirection = false;
	// Alt키 눌렸었는지 체크
	bool bIsAltPressed = false;
	bool bCanFireBullet = true;
	bool bIsAimDownSight = false;
	bool bIsAimingRifle = false;
	bool bIsWatchingSight = false;
	bool bIsReloadingBullet = false;
	bool bIsFiringBullet = false;
	FRotator CharacterMovingDirection;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bCanMove = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsJumping = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsHitting = false;

protected:
	// 장착된 무기 및 장구류 component
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UC_EquippedComponent* EquippedComponent{};

	// Stat관련 Component (ex HP)
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UC_StatComponent* StatComponent{};

	// LootCrate class
	//UPROPERTY(EditDefaultsOnly)
	//TSubclassOf<class AC_LootCrate> LootCrateClass;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UC_InvenComponent* InvenComponent{};

protected: // PriorityAnimMontage 관련

	// 현재 재생 중인, 또는 직전에 재생한 PriorityAnimMontage |<GroupName, AnimMontage>
	UPROPERTY(BlueprintReadOnly)
	TMap<FName, FPriorityAnimMontage> CurPriorityAnimMontageMap{};
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FPriorityAnimMontage DyingMontage{};

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FPriorityAnimMontage HitMontage{};

protected: // Consumable 관련
	
	// Boosting에 따른 캐릭터 속력 조절 factor
	float BoostingSpeedFactor = 1.f;
	
	// 현재 Consumable Item Activating 중인 상태인지
	bool bIsActivatingConsumableItem{};

	// 현재 활성화 중인 Consumable Item
	class AC_ConsumableItem* CurActivatingConsumableItem{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UC_ConsumableUsageMeshComponent* ConsumableUsageMeshComponent{};


protected: // 총알 Object Pooling(AC_Item으로 만들어진 Bullet은 사용X)
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<class AC_Item*> Bullets;



protected:
	//총알 Object Pooling (World에서 작업할 예정)
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TArray<class AC_Bullet*> PooledBullets;

	void PoolingBullets();

public:
	TArray<AC_Bullet*>& GetBullets() { return PooledBullets; }
	
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UC_FeetComponent* FeetComponent{};
protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UC_SwimmingComponent* SwimmingComponent{};

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UC_SkyDivingComponent* SkyDivingComponent{};

protected:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UC_AttachableItemMeshComponent* AttachmentMeshComponent{};

protected: // 파쿠르 관련 Components

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UC_ParkourComponent* ParkourComponent{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UMotionWarpingComponent* MotionWarpingComponent{};

protected: // AI 피아 식별 관련
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TEnumAsByte<ETeamAttitude::Type> TeamID = ETeamAttitude::Hostile;
public:
	///
	///캐릭터 피 파티클시스템
	///
	TArray<class UParticleSystemComponent*> GetBloodParticles() { return BloodParticleComponents; }
	void ActivateBloodParticle(FVector InLocation);
protected:
	TArray<class UParticleSystemComponent*> BloodParticleComponents;
	void InitializeBloodParticleComponents();
};


