// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "C_BasicCharacter.generated.h"

DECLARE_MULTICAST_DELEGATE(FDele_PoseTransitionFin);

UENUM(BlueprintType)
enum class EHandState : uint8
{
	UNARMED,
	WEAWPON_GUN,
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
	ATTACK,					// Default Upper body
	DRAW_SHEATH_WEAPON,		// Default Upper body
	POSE_TRANSITION,		// Sub Full body
	THROW_THROWABLE,		// 실질적으로 던지는 자세일 때, Default Upper body
	PRIORITY_MAX
};

/// <summary>
/// 피격 판정 부위
/// </summary>
UENUM(BlueprintType)
enum class EDamagingPartType : uint8
{
	HEAD,			// Physics Asset Neck

	HIPS,

	LEFT_ARM,
	LEFT_HAND,
	RIGHT_ARM,
	RIGHT_HAND,

	LEFT_LEG,
	LEFT_FOOT,
	RIGHT_LEG,
	RIGHT_FOOT,

	SHOULDER,		// Physics Asset spine2
	UPPER_STOMACH,	// Physics Asset spine1
	LOWER_STOMACH	// Phyiscs Asset spine
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
class UE_PUBG_API AC_BasicCharacter : public ACharacter
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

	// TODO : 얘를 Deprecated 시킬지, 아니면 Priority를 사용 안하고 무조건적으로 재생시키는 AnimMontage도 존재하게끔 놔두는게 좋을지
	// 후자의 경우 그냥 해당 오버라이딩 함수는 지워버리면 됨
	/// <summary>
	/// Deprecated in current UE_PUBG Project : Priority를 적용한 AC_PriorityAnimMontage로 AnimMontage 재생해야 함
	/// </summary>
	float PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None) override;

public:

	/// <summary>
	/// 몽타주 재생 우선순위에 따른 PlayAnimMontage 함수
	/// </summary>
	/// <param name="PAnimMontage"> : Priority 적용된 AnimMontage </param>
	/// <returns> Animation Montage Duration </returns>
	float PlayAnimMontage(const FPriorityAnimMontage& PAnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None);

protected:

	/// <summary>
	/// UGameplayStatics::ApplyDamage를 통해 Damage를 받는 함수
	/// </summary>
	/// <param name="DamageAmount"> : Damage 양 </param>
	/// <param name="DamageEvent"></param>
	/// <param name="EventInstigator"></param>
	/// <param name="DamageCauser"></param>
	/// <returns> : The amount of damage actually applied. </returns>
	virtual float TakeDamage
	(
		float				DamageAmount,
		FDamageEvent const& DamageEvent,
		AController*		EventInstigator,
		AActor*				DamageCauser
	) override;

public:

	/// <summary>
	/// <para> 자체로 만든 TakeDamage 함수, 부위별 Damage를 줄 때 사용 </para>
	/// <para> 주의 : 이 함수는 Armor가 적용된 부위의 데미지 감소만 구현, 실질적인 부위별 Damage량은 외부호출에서 처리 </para>
	/// </summary>
	/// <param name="DamageAmount">		: Damage 양 </param>
	/// <param name="DamagingPartType"> : Damage를 줄 부위 </param>
	/// <param name="DamageCauser">		: Damage를 주는 Actor </param>
	/// <returns> : The amount of damage actually applied. </returns>
	virtual float TakeDamage(float DamageAmount, EDamagingPartType DamagingPartType, AActor* DamageCauser);

	virtual float TakeDamage(float DamageAmount, FName DamagingPhyiscsAssetBoneName, AActor* DamageCauser);

protected:

	/// <summary>
	/// Pose와 캐릭터 이동방향에 따른 MaxWalkSpeed 조정
	/// </summary>
	/// <param name="MovementVector"> : Input action movement vector </param>
	void UpdateMaxWalkSpeed(const FVector2D& MovementVector);


public: // Getters and setters

	EHandState GetHandState() const { return HandState; }
	EPoseState GetPoseState() const { return PoseState; }
	void SetHandState(EHandState InHandState) { HandState = InHandState; }

	float GetNextSpeed() const { return NextSpeed; }
	void SetNextSpeed(float InNextSpeed) { NextSpeed = InNextSpeed; }
	bool GetIsJumping() const { return bIsJumping; }
	UFUNCTION(BlueprintCallable)
	class UC_EquippedComponent* GetEquippedComponent() const { return EquippedComponent; }
	//class UC_InvenComponent* GetInvenComponent() const { return BPC_InvenSystemInstance; }
	void SetCanMove(bool InCanMove) { bCanMove = InCanMove; }

	bool GetCanMove() const {return bCanMove;}
	bool GetIsAimDown() { return bIsAimDownSight; }

	void SetIsJumping(bool InIsJumping) { bIsJumping = InIsJumping; }

	//class UC_InvenComponent* GetInvenComponent() { return BPC_InvenSystemInstance; }
	
	class UC_InvenComponent* GetInvenComponent() { return BPC_InvenSystemInstance; }

	bool GetIsHoldDirection() const { return bIsHoldDirection; }

	bool GetIsPoseTransitioning() const { return bIsPoseTransitioning; }

public:

	/// <summary>
	/// Pose Transition montage가 진행 중인 중간에 Callback되는 함수
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void OnPoseTransitionGoing();

	/// <summary>
	/// Pose Transition Montage가 끝나고 Callback되는 함수
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void OnPoseTransitionFinish();

protected:

	/// <summary>
	///  Pose Transition 모션 실행하기
	/// </summary>
	/// <param name="CurrentPoseState"> : 현재 자세 </param>
	/// <param name="InNextPoseState"> : 다음 자세 </param>
	/// <returns> Pose transition action이 제대로 실행되었다면 return true </returns>
	
	
	/// <summary>
	/// Pose Transition 모션 실행하기
	/// </summary>
	/// <param name="TransitionMontage"> : Pose Transition motion </param>
	/// <param name="InNextPoseState"> : 다음 자세 </param>
	/// <returns> Pose transition motion이 제대로 실행되었다면 return true </returns>
	bool ExecutePoseTransitionAction(const FPriorityAnimMontage& TransitionMontage, EPoseState InNextPoseState);

protected:

	// Current hand state
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EHandState HandState{};

	// Current pose state 
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EPoseState PoseState{};

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

protected: // Camera

	UPROPERTY(BluePrintReadWrite, EditAnywhere)
	class UCameraComponent* MainCamera{};

protected:

	// AnimCharacter에서 참조할 Speed의 다음 Lerp destination 값
	float NextSpeed{};
	// Alt키 누르고 있는지 체크
	bool bIsHoldDirection = false;
	// Alt키 눌렸었는지 체크
	bool bIsAltPressed = false;

	bool bIsAimDownSight = false;
	FRotator CharacterMovingDirection;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bCanMove = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsJumping = false;
protected:
	// 장착된 무기 및 장구류 component
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UC_EquippedComponent* EquippedComponent{};

public:
	UPROPERTY(BlueprintReadonly, Category = "InvenSystem", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UC_InvenComponent> BPC_InvenSystem;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UC_InvenComponent* BPC_InvenSystemInstance;


protected: // PriorityAnimMontage 관련

	// 현재 재생 중인, 또는 직전에 재생한 PriorityAnimMontage |<GroupName, AnimMontage>
	UPROPERTY(BlueprintReadOnly)
	TMap<FName, FPriorityAnimMontage> CurPriorityAnimMontageMap{};
	
private:

	/// <summary>
	/// <para> 현재 재생 중인 PriorityAnimMontage의 Priority에 상관 없이 가장 최상위로 재생시킬 때 사용 </para>
	/// <para> 재생 시작 시 무조건 재생 & 재생 중일 때 다음 Montage 들어오지 못하도록 block </para>
	/// </summary>
	//bool bForcePlayCurrentMontage{};


private:
	// 피격 판정 부위 Mapping TPair<PhysicsAssetBoneName, EDamagingPartType>
	const TMap<FName, EDamagingPartType> DAMAGINGPARTS_MAP =	
	{
		{"Neck",		EDamagingPartType::HEAD},

		{"Hips",		EDamagingPartType::HIPS},

		{"LeftUpLeg",	EDamagingPartType::LEFT_LEG},
		{"LeftFoot",	EDamagingPartType::LEFT_FOOT},
		{"RightUpLeg",	EDamagingPartType::RIGHT_LEG},
		{"RightFoot",	EDamagingPartType::RIGHT_FOOT},

		{"Spine",		EDamagingPartType::LOWER_STOMACH},
		{"Spine1",		EDamagingPartType::UPPER_STOMACH},
		{"Spine2",		EDamagingPartType::SHOULDER},

		{"LeftArm",		EDamagingPartType::LEFT_ARM},
		{"LeftHand",	EDamagingPartType::LEFT_HAND},

		{"RightArm",	EDamagingPartType::RIGHT_ARM},
		{"RightHand",	EDamagingPartType::RIGHT_HAND}
	};

};
