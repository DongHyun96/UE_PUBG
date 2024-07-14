// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "C_AnimBasicCharacter.generated.h"

enum class EHandState : uint8;
enum class EPoseState : uint8;

/**
 * 
 */

UCLASS()
class UE_PUBG_API UC_AnimBasicCharacter : public UAnimInstance
{
	GENERATED_BODY()

	virtual void NativeBeginPlay() override;

	/// <summary>
	/// AnimGraph������ transition�� data�� ������Ʈ
	/// </summary>
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class AC_BasicCharacter* OwnerCharacter{}; // owner


protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Speed{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsFalling{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsJumping{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Direction{}; // Direction Angle

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FRotator CSpineRotation = FRotator(0);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FQuat CHeadLookAtRotation= FQuat(0);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bCanCharacterMove = true;


	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EHandState HandState{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EPoseState PoseState{};
	UFUNCTION(BlueprintCallable, Category = "CustomEvent")
	void AnimNotify_OnStartTransition_Stand_To_Falling();


	UFUNCTION(BlueprintCallable, Category = "CustomEvent")
	void AnimNotify_OnStartTransition_RunningJump_To_Falling();

	UFUNCTION(BlueprintCallable, Category = "CustomEvent")
	void AnimNotify_OnEndTransition_HardLand_To_Stand();

	UFUNCTION(BlueprintCallable, Category = "CustomEvent")
	void AnimNotify_OnEndTransition_Falling_To_Standing();

	void ControlHeadRotation();

	void RilfeLeftHandIK();
};
