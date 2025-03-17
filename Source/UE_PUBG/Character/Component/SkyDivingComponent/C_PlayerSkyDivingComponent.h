// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Component/SkyDivingComponent/C_SkyDivingComponent.h"
#include "C_PlayerSkyDivingComponent.generated.h"

/**
 * 
 */

/// <summary>
/// 자세 speed 관련 struct
/// </summary>
struct FDivingSpeeds
{
	FDivingSpeeds() {}

	FDivingSpeeds(float MinWalk, float MaxWalk, float BackKeyZSpeed, float ZSpeedMin, float ZSpeedMax) :
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

UCLASS()
class UE_PUBG_API UC_PlayerSkyDivingComponent : public UC_SkyDivingComponent
{
	GENERATED_BODY()

public:

	UC_PlayerSkyDivingComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:

	/// <summary>
	/// <para> SkyDiving시에 Player InputComponent Move 함수 내에서 호출될 함수 </para>
	/// <para> 주의 : Player의 Movement만 처리할 예정 </para>
	/// </summary>
	/// <param name="MovementVector"></param>
	void HandlePlayerMovement(const FVector2D& MovementVector);

	/// <summary>
	/// Player Move의 MoveEnd 처리 담당
	/// </summary>
	void OnPlayerSkyMoveEnd();

private:

	/// <summary>
	/// Player 카메라 SpringArm 길이 Lerp 시키기
	/// </summary>
	/// <param name="DeltaTime"></param>
	void LerpPlayerMainCameraArmLength(const float& DeltaTime);

	void UpdatePlayerSkyDiveHUD();

	/// <summary>
	/// 중력 가속도를 사용하는 대신 OwnerPlayer VelocityZ를 직접 조정
	/// </summary>
	/// <param name="DeltaTime"></param>
	void LerpVelocityZ(const float& DeltaTime);

private:
	/// <summary>
	/// SetSkyDivingState -> Set to SKYDIVING STATE 내부의 Template method
	/// </summary>
	void SetStateToSkyDivingState() override;

	/// <summary>
	/// SetSkyDivingState -> Set to PARACHUTING STATE 내부의 Template method
	/// </summary>
	void SetStateToParachutingState() override;

	/// <summary>
	/// SetSkyDivingState -> Set to LANDING STATE 내부의 Template method
	/// </summary>
	void SetStateToLandingState() override;

private:

	/// <summary>
	/// Parachuting 상태일 때 물 위로 착륙했을 때 호출될 예외처리 함수
	/// </summary>
	void OnCharacterLandedOnWater();

private:

	class AC_Player* OwnerPlayer{};

private: // Player Main camera spring arm 거리 조절 관련

	const float PLAYER_READY_MAINCAM_ARMLENGTH		= 3000.f; // 비행기 안에 탑승했을 때의 ArmLength
	const float PLAYER_PARACHUTE_MAINCAM_ARMLENGTH	= 600.f;
	const float PLAYER_SKYDIVE_MAINCAM_ARMLENGTH	= 400.f;
	const float PLAYER_ORIGIN_MAINCAM_ARMLENGTH		= 220.f;
	
private: // SkyDiving & Parachuting 자세 speed 관련

	const TMap<ESkyDivingState, FDivingSpeeds> STATE_DIVINGSPEEDS =
	{
		{
			// Dummy data
			ESkyDivingState::READY,
			FDivingSpeeds(0.f, 0.f, 0.f, 0.f, 0.f)
		},
		{
			ESkyDivingState::SKYDIVING, 
			FDivingSpeeds(0.f, 3000.f, -2500.f, -3000.f, -6000.f) 
		},
		{
			ESkyDivingState::PARACHUTING,
			FDivingSpeeds(400.f, 1000.f, -300.f, -500.f, -2000.f)
		},
		{
			// Landing 이후에 MoveEnd 처리 버그를 막기 위한 Dummy data
			ESkyDivingState::LANDING,
			FDivingSpeeds(0.f, 0.f, 0.f, 0.f, 0.f)
		}
	};
		

private: // 중력가속 사용 x -> 직접 Velocity.Z를 조절

	float VelocityZLerpDest{};
	
};

