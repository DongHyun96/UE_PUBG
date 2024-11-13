// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Character/C_BasicCharacter.h"

#include "C_SkyDivingComponent.generated.h"

UENUM(BlueprintType)
enum class ESkyDivingState : uint8
{
	READY,			// 비행기에 아직 있는 상태
	SKYDIVING,		
	PARACHUTING,
	LANDING,			// 착지 동작 재생
	MAX
};

/// <summary>
/// 자세 speed 관련 struct
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

	float MaxWalkSpeed_Min{};	// 캐릭터 최소 이동 속력
	float MaxWalkSpeed_Max{};	// 캐릭터 최대 이동 속력
	float BackKeyZSpeed{};		// 뒷방향 input일 때의 Z Speed
	float ZSpeedMin{};			// 일반 상황에서의 Z Speed Min
	float ZSpeedMax{};			// 일반 상황에서의 Z Speed Max
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
	/// SkyDiving시에 Player InputComponent Move 함수 내에서 호출될 함수
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
	/// Parachuting 상태일 때 물 위로 착륙했을 때 호출될 예외처리 함수
	/// </summary>
	void OnCharacterLandedOnWater();

public:

	/// <summary>
	/// Deploy Parachute 몽타주 재생 끝날 때 AnimNotify callback 함수
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void OnDeployParachuteMontageEnd();

	/// <summary>
	/// LandingMontage AnimNotify callback 함수
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void OnLandingMontageEnd();

	/// <summary>
	/// Parachute Landing End Animotify callback 함수
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void OnParachuteLandingMontageEnd();

private:

	/// <summary>
	/// Player 카메라 SpringArm 길이 Lerp 시키기
	/// </summary>
	/// <param name="DeltaTime"></param>
	void LerpPlayerMainCameraArmLength(const float& DeltaTime);

	/// <summary>
	/// 중력 가속도를 사용하는 대신 OwnerCharacter의 VelocityZ를 직접 조정
	/// </summary>
	/// <param name="DeltaTime"></param>
	void LerpVelocityZ(const float& DeltaTime);

	/// <summary>
	/// 높이에 따른 State 전환 담당 (ex -> 일정 높이 이하면 낙하산 자동으로 펼치기)
	/// </summary>
	void HandleStateTransitionByHeight();
	
	/// <summary>
	/// 현재 높이(고도) Update
	/// </summary>
	void UpdateCurrentHeight();

	void UpdatePlayerSkyDiveHUD();

private:

	class AC_BasicCharacter* OwnerCharacter{};
	class AC_Player* OwnerPlayer{};

	//ESkyDivingState SkyDivingState = ESkyDivingState::LANDING;
	ESkyDivingState SkyDivingState = ESkyDivingState::READY;

private: // 중력가속 사용 x -> 직접 Velocity.Z를 조절

	float VelocityZLerpDest{};

private: // SkyDiving & Parachuting 자세 speed 관련

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

private: // Player Main camera spring arm 거리 조절 관련

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

	// 원작 수치
	//const float PARACHUTE_DEPLOY_LIMIT_HEIGHT = 43000.f;
	//const float MAX_SKYDIVE_JUMP_ALTITUDE = 155000.f; // 원작 기준 8 x 8 맵 1.5km 상공 맥시멈에서 뛰어내림

	const float PARACHUTE_DEPLOY_LIMIT_HEIGHT	= 10000.f;
	const float MAX_SKYDIVE_JUMP_ALTITUDE		= 155000.f; // 원작 기준 8 x 8 맵 1.5km 상공 맥시멈에서 뛰어내림

private:
	
	// 현재 낙하 높이(고도)
	float CurrentHeight{};


};
