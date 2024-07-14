// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "C_BasicCharacter.generated.h"

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
	TURN_IN_PLACE,
	ATTACK,
	DRAW_SHEATH_WEAPON,
	THROW_THROWABLE,		// 실질적으로 던지는 자세일 때
	PRIORITY_MAX
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

public: // Getters and setters

	EHandState GetHandState() const { return HandState; }
	EPoseState GetPoseState() const { return PoseState; }
	void SetHandState(EHandState InHandState) { HandState = InHandState; }

	float GetNextSpeed() const { return NextSpeed; }
	void SetNextSpeed(float InNextSpeed) { NextSpeed = InNextSpeed; }
	bool GetIsJumping() const { return bIsJumping; }
	UFUNCTION(BlueprintCallable)
	class UC_EquippedComponent* GetEquippedComponent() const { return EquippedComponent; }
	class UC_InvenComponent* GetInvenComponent() const { return InvenComponent; }
	void SetCanMove(bool InCanMove) { bCanMove = InCanMove; }
	bool GetCanMove() const {return bCanMove;}
	void SetIsJumping(bool InIsJumping) { bIsJumping = InIsJumping; }

	class UC_InvenComponent* GetInvenComponent() { return InvenComponent; }


protected:

	// Current hand state
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EHandState HandState{};

	// Current post state 
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EPoseState PoseState{};

protected: // Camera

	UPROPERTY(BluePrintReadWrite, EditDefaultsOnly)
	class UCameraComponent* MainCamera{};

protected:

	// AnimCharacter에서 참조할 Speed의 다음 Lerp destination 값
	float NextSpeed{};
	// Alt키 누르고 있는지 체크
	bool bIsHoldDirection = false;
	// Alt키 눌렸었는지 체크
	bool bIsAltPressed = false;
	FRotator CharacterMovingDirection;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bCanMove = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsJumping = false;
protected:
	// 장착된 무기 및 장구류 component
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UC_EquippedComponent* EquippedComponent{};

	//인벤토리(가방) component
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UC_InvenComponent* InvenComponent{};
protected:
	
	// 현재 재생 중인, 또는 직전에 재생한 PriorityAnimMontage
	UPROPERTY(BlueprintReadOnly)
	FPriorityAnimMontage CurPriorityAnimMontage{};


};
