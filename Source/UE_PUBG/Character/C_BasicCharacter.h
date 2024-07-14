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
	THROW_THROWABLE,		// ���������� ������ �ڼ��� ��
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

	// AnimCharacter���� ������ Speed�� ���� Lerp destination ��
	float NextSpeed{};
	// AltŰ ������ �ִ��� üũ
	bool bIsHoldDirection = false;
	// AltŰ ���Ⱦ����� üũ
	bool bIsAltPressed = false;
	FRotator CharacterMovingDirection;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bCanMove = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsJumping = false;
protected:
	// ������ ���� �� �屸�� component
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UC_EquippedComponent* EquippedComponent{};

	//�κ��丮(����) component
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UC_InvenComponent* InvenComponent{};
protected:
	
	// ���� ��� ����, �Ǵ� ������ ����� PriorityAnimMontage
	UPROPERTY(BlueprintReadOnly)
	FPriorityAnimMontage CurPriorityAnimMontage{};


};
