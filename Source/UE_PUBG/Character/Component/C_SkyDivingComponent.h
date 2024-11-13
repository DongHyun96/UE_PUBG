// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Character/C_BasicCharacter.h"

#include "C_SkyDivingComponent.generated.h"

UENUM(BlueprintType)
enum class ESkyDivingState : uint8
{
	READY,			// ����⿡ ���� �ִ� ����
	SKYDIVING,		
	PARACHUTING,
	LANDING,			// ���� ���� ���
	MAX
};

/// <summary>
/// �ڼ� speed ���� struct
/// </summary>
struct DivingSpeeds
{
	DivingSpeeds() {}

	DivingSpeeds(float MinWalk, float MaxWalk, float BackKeyZSpeed, float ZSpeedMin, float ZSpeedMax) :
		MaxWalkSpeed_Min(MinWalk),
		MaxWalkSpeed_Max(MaxWalk),
		BackKeyZSpeed(BackKeyZSpeed),
		ZSpeedMin(ZSpeedMin),
		ZSpeedMax(ZSpeedMax) 
	{}

	FVector2D GetZSpeedMinMax() const { return FVector2D(ZSpeedMin, ZSpeedMax); }
	FVector2D GetMaxWalkSpeedMaxMin() const { return FVector2D(MaxWalkSpeed_Max, MaxWalkSpeed_Min); }

	float MaxWalkSpeed_Min{};	// ĳ���� �ּ� �̵� �ӷ�
	float MaxWalkSpeed_Max{};	// ĳ���� �ִ� �̵� �ӷ�
	float BackKeyZSpeed{};		// �޹��� input�� ���� Z Speed
	float ZSpeedMin{};			// �Ϲ� ��Ȳ������ Z Speed Min
	float ZSpeedMax{};			// �Ϲ� ��Ȳ������ Z Speed Max
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE_PUBG_API UC_SkyDivingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UC_SkyDivingComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/// <summary>
	/// SkyDiving�ÿ� Player InputComponent Move �Լ� ������ ȣ��� �Լ�
	/// </summary>
	/// <param name="MovementVector"></param>
	void HandlePlayerMovement(const FVector2D& MovementVector);

	void OnSkyMoveEnd();

public:

	void SetOwnerCharcter(class AC_BasicCharacter* InOwnerCharacter) { OwnerCharacter = InOwnerCharacter; }

	ESkyDivingState GetSkyDivingState() const { return SkyDivingState; }
	void SetSkyDivingState(ESkyDivingState InSkyDivingState);

public:

	/// <summary>
	/// Parachuting ������ �� �� ���� �������� �� ȣ��� ����ó�� �Լ�
	/// </summary>
	void OnCharacterLandedOnWater();

public:

	/// <summary>
	/// Deploy Parachute ��Ÿ�� ��� ���� �� AnimNotify callback �Լ�
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void OnDeployParachuteMontageEnd();

	/// <summary>
	/// LandingMontage AnimNotify callback �Լ�
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void OnLandingMontageEnd();

	/// <summary>
	/// Parachute Landing End Animotify callback �Լ�
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void OnParachuteLandingMontageEnd();

private:

	/// <summary>
	/// Player ī�޶� SpringArm ���� Lerp ��Ű��
	/// </summary>
	/// <param name="DeltaTime"></param>
	void LerpPlayerMainCameraArmLength(const float& DeltaTime);

	/// <summary>
	/// �߷� ���ӵ��� ����ϴ� ��� OwnerCharacter�� VelocityZ�� ���� ����
	/// </summary>
	/// <param name="DeltaTime"></param>
	void LerpVelocityZ(const float& DeltaTime);

	/// <summary>
	/// ���̿� ���� State ��ȯ ��� (ex -> ���� ���� ���ϸ� ���ϻ� �ڵ����� ��ġ��)
	/// </summary>
	void HandleStateTransitionByHeight();
	
	/// <summary>
	/// ���� ����(��) Update
	/// </summary>
	void UpdateCurrentHeight();

	void UpdatePlayerSkyDiveHUD();

private:

	class AC_BasicCharacter* OwnerCharacter{};
	class AC_Player* OwnerPlayer{};

	//ESkyDivingState SkyDivingState = ESkyDivingState::LANDING;
	ESkyDivingState SkyDivingState = ESkyDivingState::READY;

private: // �߷°��� ��� x -> ���� Velocity.Z�� ����

	float VelocityZLerpDest{};

private: // SkyDiving & Parachuting �ڼ� speed ����

	const TMap<ESkyDivingState, DivingSpeeds> State_DivingSpeeds =
	{
		{
			ESkyDivingState::SKYDIVING, 
			DivingSpeeds(0.f, 3000.f, -2500.f, -3000.f, -6000.f) 
		},

		{
			ESkyDivingState::PARACHUTING,
			DivingSpeeds(400.f, 1000.f, -300.f, -500.f, -2000.f)
		}
	};

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UStaticMeshComponent* ParachuteBackpackStaticMesh{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class USkeletalMeshComponent* ParachuteSkeletalMesh{};

	const FName PARABACKPACK_SOCKET_NAME = "ParachuteBackPackSocket";
	const FName PARACHUTE_SOCKET_NAME    = "ParachuteSocket";

private: // Player Main camera spring arm �Ÿ� ���� ����

	const float PLAYER_PARACHUTE_MAINCAM_ARMLENGTH = 600.f;
	const float PLAYER_SKYDIVE_MAINCAM_ARMLENGTH = 400.f;
	const float PLAYER_ORIGIN_MAINCAM_ARMLENGTH  = 220.f;

protected: // Character AnimMontages

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FPriorityAnimMontage DeployParachuteMontage{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FPriorityAnimMontage LandingMontage{};

protected: // Parachute Skeletal Mesh AnimMontages

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UAnimMontage* ParachuteDeployMontage{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UAnimMontage* ParachuteEjectMontage{};

private:

	// ���� ��ġ
	//const float PARACHUTE_DEPLOY_LIMIT_HEIGHT = 43000.f;
	//const float MAX_SKYDIVE_JUMP_ALTITUDE = 155000.f; // ���� ���� 8 x 8 �� 1.5km ��� �ƽøؿ��� �پ��

	const float PARACHUTE_DEPLOY_LIMIT_HEIGHT	= 10000.f;
	const float MAX_SKYDIVE_JUMP_ALTITUDE		= 155000.f; // ���� ���� 8 x 8 �� 1.5km ��� �ƽøؿ��� �پ��

private:
	
	// ���� ���� ����(��)
	float CurrentHeight{};


};
