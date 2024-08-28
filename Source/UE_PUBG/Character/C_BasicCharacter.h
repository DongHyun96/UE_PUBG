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
	THROW_THROWABLE,		// ���������� ������ �ڼ��� ��, Default Upper body
	PRIORITY_MAX
};

/// <summary>
/// �ǰ� ���� ����
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

	// TODO : �긦 Deprecated ��ų��, �ƴϸ� Priority�� ��� ���ϰ� ������������ �����Ű�� AnimMontage�� �����ϰԲ� ���δ°� ������
	// ������ ��� �׳� �ش� �������̵� �Լ��� ���������� ��
	/// <summary>
	/// Deprecated in current UE_PUBG Project : Priority�� ������ AC_PriorityAnimMontage�� AnimMontage ����ؾ� ��
	/// </summary>
	float PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None) override;

public:

	/// <summary>
	/// ��Ÿ�� ��� �켱������ ���� PlayAnimMontage �Լ�
	/// </summary>
	/// <param name="PAnimMontage"> : Priority ����� AnimMontage </param>
	/// <returns> Animation Montage Duration </returns>
	float PlayAnimMontage(const FPriorityAnimMontage& PAnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None);

protected:

	/// <summary>
	/// UGameplayStatics::ApplyDamage�� ���� Damage�� �޴� �Լ�
	/// </summary>
	/// <param name="DamageAmount"> : Damage �� </param>
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
	/// <para> ��ü�� ���� TakeDamage �Լ�, ������ Damage�� �� �� ��� </para>
	/// <para> ���� : �� �Լ��� Armor�� ����� ������ ������ ���Ҹ� ����, �������� ������ Damage���� �ܺ�ȣ�⿡�� ó�� </para>
	/// </summary>
	/// <param name="DamageAmount">		: Damage �� </param>
	/// <param name="DamagingPartType"> : Damage�� �� ���� </param>
	/// <param name="DamageCauser">		: Damage�� �ִ� Actor </param>
	/// <returns> : The amount of damage actually applied. </returns>
	virtual float TakeDamage(float DamageAmount, EDamagingPartType DamagingPartType, AActor* DamageCauser);

	virtual float TakeDamage(float DamageAmount, FName DamagingPhyiscsAssetBoneName, AActor* DamageCauser);

protected:

	/// <summary>
	/// Pose�� ĳ���� �̵����⿡ ���� MaxWalkSpeed ����
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
	/// Pose Transition montage�� ���� ���� �߰��� Callback�Ǵ� �Լ�
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void OnPoseTransitionGoing();

	/// <summary>
	/// Pose Transition Montage�� ������ Callback�Ǵ� �Լ�
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void OnPoseTransitionFinish();

protected:

	/// <summary>
	///  Pose Transition ��� �����ϱ�
	/// </summary>
	/// <param name="CurrentPoseState"> : ���� �ڼ� </param>
	/// <param name="InNextPoseState"> : ���� �ڼ� </param>
	/// <returns> Pose transition action�� ����� ����Ǿ��ٸ� return true </returns>
	
	
	/// <summary>
	/// Pose Transition ��� �����ϱ�
	/// </summary>
	/// <param name="TransitionMontage"> : Pose Transition motion </param>
	/// <param name="InNextPoseState"> : ���� �ڼ� </param>
	/// <returns> Pose transition motion�� ����� ����Ǿ��ٸ� return true </returns>
	bool ExecutePoseTransitionAction(const FPriorityAnimMontage& TransitionMontage, EPoseState InNextPoseState);

protected:

	// Current hand state
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EHandState HandState{};

	// Current pose state 
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EPoseState PoseState{};

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

protected: // Camera

	UPROPERTY(BluePrintReadWrite, EditAnywhere)
	class UCameraComponent* MainCamera{};

protected:

	// AnimCharacter���� ������ Speed�� ���� Lerp destination ��
	float NextSpeed{};
	// AltŰ ������ �ִ��� üũ
	bool bIsHoldDirection = false;
	// AltŰ ���Ⱦ����� üũ
	bool bIsAltPressed = false;

	bool bIsAimDownSight = false;
	FRotator CharacterMovingDirection;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bCanMove = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsJumping = false;
protected:
	// ������ ���� �� �屸�� component
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UC_EquippedComponent* EquippedComponent{};

public:
	UPROPERTY(BlueprintReadonly, Category = "InvenSystem", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UC_InvenComponent> BPC_InvenSystem;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UC_InvenComponent* BPC_InvenSystemInstance;


protected: // PriorityAnimMontage ����

	// ���� ��� ����, �Ǵ� ������ ����� PriorityAnimMontage |<GroupName, AnimMontage>
	UPROPERTY(BlueprintReadOnly)
	TMap<FName, FPriorityAnimMontage> CurPriorityAnimMontageMap{};
	
private:

	/// <summary>
	/// <para> ���� ��� ���� PriorityAnimMontage�� Priority�� ��� ���� ���� �ֻ����� �����ų �� ��� </para>
	/// <para> ��� ���� �� ������ ��� & ��� ���� �� ���� Montage ������ ���ϵ��� block </para>
	/// </summary>
	//bool bForcePlayCurrentMontage{};


private:
	// �ǰ� ���� ���� Mapping TPair<PhysicsAssetBoneName, EDamagingPartType>
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
