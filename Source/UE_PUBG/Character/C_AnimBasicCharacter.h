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

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Speed{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsFalling{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
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

	UFUNCTION(BlueprintCallable, Category = "CustomEvent")
	void AnimNotify_OnStartTransition_Stand_To_Falling();


	UFUNCTION(BlueprintCallable, Category = "CustomEvent")
	void AnimNotify_OnStartTransition_RunningJump_To_Falling();

	UFUNCTION(BlueprintCallable, Category = "CustomEvent")
	void AnimNotify_OnEndTransition_HardLand_To_Stand();

	UFUNCTION(BlueprintCallable, Category = "CustomEvent")
	void AnimNotify_OnEndTransition_Falling_To_Standing();

	UFUNCTION(BlueprintCallable, Category = "CustomEvent")
	void AnimNotify_OnEndTransition_Falling_To_HardLand();

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

	float AimOffsetLerpDelayTime = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FFeetData FeetData{};

protected:

	// Radian Crawl Rotation Angle
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float CrawlRotationAngle{};

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	ESkyDivingState SkyDivingState{};

};


