// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Item/Weapon/Gun/C_Gun.h"
//#include "Character/C_BasicCharacter.h"
#include "Character/Component/C_FeetComponent.h"
#include "C_AnimBasicCharacter.generated.h"

enum class EMainState : uint8;
enum class EHandState : uint8;
enum class EPoseState : uint8;
enum class ESwimmingState : uint8;
enum class ESkyDivingState : uint8;

/**
 * 
 */

UCLASS()
class UE_PUBG_API UC_AnimBasicCharacter : public UAnimInstance
{
	GENERATED_BODY()

public:

	virtual void NativeBeginPlay() override;

protected:

	/// <summary>
	/// AnimGraph에서의 transition용 data들 업데이트
	/// </summary>
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class AC_BasicCharacter* OwnerCharacter{}; // owner

public:

	FRotator GetCSpineRotation() const { return CSpineRotation; }

	void SetCrawlRotationAngle(const float& Angle) { CrawlRotationAngle = Angle; }

	float GetSpeed() const { return Speed; }

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Speed{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsFalling{};

	// bIsFalling상태 도합 시간 계산
	UPROPERTY(BlueprintReadOnly)
	float MeasuredFallingTime{};

	// 현재 떨어지고 있을 때, 떨어지기 시작한 위치로부터의 높이 (떨어지는 중이 아니라면 0)
	UPROPERTY(BlueprintReadOnly)
	float CurrentFallingHeight{};

	UPROPERTY(BlueprintReadWrite)
	bool bIsJumping{};
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bGunHasGrip{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Direction{}; // Direction Angle
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsAimDownSight{};
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FRotator CSpineRotation = FRotator(0);
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FRotator CAimingRotation = FRotator(0);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FRotator CAimOffsetRotation = FRotator(0);
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FRotator CCurrentAimOffsetRotation = FRotator(0);
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FQuat CHeadLookAtRotation = FQuat(0);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FTransform RifleLeftHandSocket{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bCanCharacterMove = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bCharacterIsWatchingSight = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bCharacterIsSheathing = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsHoldDirection = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsAimingRifle = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsHighEnoughToFall = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsTooCloseToAimGun = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ESwimmingState SwimmingState{};
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bCanUseAimOffset = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsLeftHandIKOn = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EMainState MainState{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EHandState HandState{};
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EPoseState PoseState{};
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EGunType CurGunType{};

protected: // Anim state Transition callback 관련
	
	// Jumping 또는 Falling State에서 Stand 자세로 돌아갈 때 CallBack 
	UFUNCTION(BlueprintCallable)
	virtual void AnimNotify_OnAnyFallingOrJumpingStateToStand();

	// FallingHard to HardLanding Transition callback
	UFUNCTION(BlueprintCallable)
	virtual void AnimNotify_OnFallingHardToHardLanding();	

protected:
	
	void ControlHeadRotation();

	void RilfeLeftHandIK();

	virtual void SetAimOffsetRotation();

	void SetAimingTurnInPlaceRotation();

	void SetLeftHandIKOn();

	void SetCanUseAimOffset();
	void SetIsLeftHandIKOn();
protected:
	float AimingTurnInPlaceTimeCount = 0.0f;
	float SavedYaw = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float DeltaYaw = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FFeetData FeetData{};

protected:

	// Radian Crawl Rotation Angle
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float CrawlRotationAngle{};

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	ESkyDivingState SkyDivingState{};

protected:

	UPROPERTY(BlueprintReadOnly)
	bool bIsActivatingConsumableItem{};

};


