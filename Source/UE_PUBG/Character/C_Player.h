// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "GameFramework/Character.h"
#include "Character/C_BasicCharacter.h"
#include "C_Player.generated.h"


USTRUCT(BlueprintType)
struct FPoseAnimMontage
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TMap<EPoseState, FPriorityAnimMontage> LeftMontages{};
		
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TMap<EPoseState, FPriorityAnimMontage> RightMontages{};
};

/**
 * 
 */
UCLASS()
class UE_PUBG_API AC_Player : public AC_BasicCharacter
{
	GENERATED_BODY()

public:
	AC_Player();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public: // Input mapped actions

	void Move(const struct FInputActionValue& Value);
	
	/// <summary>
	/// Move�� ���� �� ȣ���, AnimCharacter���� Speed Lerp�� �� setting �� Turn in place ���� �ʱ� setting 
	/// </summary>
	void MoveEnd(const struct FInputActionValue& Value);

	void Look(const struct FInputActionValue& Value);
	void Walk(const struct FInputActionValue& Value);
	void Sprint();


	void Crouch();
	void Crawl();

	void OnJump();
	void CancelTurnInPlaceMotion();
	//Alt Ű ������ �̸� ��õ��Ź
	void HoldDirection();
	void ReleaseDirection();
	void HandleControllerRotation(float DeltaTime);

	// Number input mappings
	void OnNum1();
	void OnNum2();
	void OnNum4();
	void OnNum5();

	void OnXKey();
	void OnBKey();
	void OnRKey();

	// TODO : ���⿡ ���� ���콺 ��ư Ŭ�� ó���� �ص�, �ٸ� ó���� �ʿ��� ����
	void OnMLBStarted();
	void OnMLBOnGoing();
	void OnMLBCompleted();

	void OnMRBStarted();
	void OnMRBOnGoing();
	void OnMRBCompleted();

	//��ȣ�ۿ�(F)
	//������Ʈ�� �������� ������ �� ����?
	//�ƴϸ� �׳� UObject�� �����
	void Interaction();

/// <summary>
/// ������ ��ȣ�ۿ��� ���� ������ �Լ�.
/// ���� ���� �����۰��� �浿�� �����ϴ� �Լ�.
/// </summary>
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* DetectionSphere;

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	class UC_InputComponent* GetInvenComponent() { return MyInputComponent; }//�θ� Ŭ������ ����
public:
	//Aim Press Camera
	UPROPERTY(EditDefaultsOnly)
	class UCameraComponent* AimCamera;
	UPROPERTY(EditDefaultsOnly)
	class USpringArmComponent* AimSpringArmTemp;
protected:
	//Aim Press Camera ��ġ ���� �Լ�
	void HandleAimPressCameraLocation();
protected:

	/*
	ī�޶� �ٶ󺸴� �������� ��ü�� ���ư��� �⺻ StrafeOn ���� :
	BPC_Player(self) : Use Control Rotation Yaw			= true
	Spring Arm : Use Pawn Control Rotation				= true
	Character Movement : Orient Rotation to Movement	= false;

	������ ���� �� Use Control Rotation Yaw : false

	*/

	/// <summary>
	/// <para> ĳ���Ͱ� �������� ��, ĳ���Ͱ� ������Ʈ�� �ٶ󺸴� ����� ��Ʈ�ѷ�(ī�޶�)�� </para>
	/// <para> �ٶ󺸴� ������ ���� 90�� �̻��̸�, Turn In place�� ĳ���� ���� </para>
	/// </summary>
	void HandleTurnInPlace();
	
	/// <summary>
	/// ���� ���� ���� Rotation ���� ����� ���ư���
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void SetStrafeRotationToIdleStop();

private:

	/// <summary>
	/// Turn Anim Montage �ʱ�ȭ
	/// </summary>
	void InitTurnAnimMontageMap();
public:
	/// <summary>
	/// ���ؿ� ���� ī�޶� ����
	/// </summary>
	void SetToAimKeyPress();
	void SetToAimDownSight();
	void BackToMainCamera();
protected:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly) 
	class UC_InputComponent* MyInputComponent{};   

protected: // Turn in place �ִ� ��Ÿ�� ����

	/// <summary>
	/// �� HandState�� PoseState�� ���� TurnAnimMontage ��
	/// </summary>
	UPROPERTY(BluePrintReadWrite, EditAnywhere)
	TMap<EHandState, FPoseAnimMontage> TurnAnimMontageMap{};

	TArray<class AC_Item*> NearInventory;

	/*
	UENUM(BlueprintType)
	enum class EHandState : uint8
	{
		UNARMED,
		WEAWPON_GUN,
		WEAPON_MELEE,
		WEAPON_THROWABLE
	};
	*/
	
	/*
	enum class EPoseState : uint8
	{
		STAND,
		CROUCH,
		CRAWL
	};
	*/
};
