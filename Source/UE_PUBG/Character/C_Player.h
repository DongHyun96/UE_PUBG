// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "GameFramework/Character.h"
#include "Components/TimelineComponent.h"
#include "Character/C_BasicCharacter.h"
#include "C_Player.generated.h"


/// <summary>
/// Pose�� Turn left, right anim montage ����ü
/// </summary>
USTRUCT(BlueprintType)
struct FPoseTurnInPlaceAnimMontage
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

	void SetPlayerMappingContext();
private:

	/// <summary>
	/// AltŰ (Hold Direction) Handling
	/// </summary>
	/// <param name="DeltaTime"></param>
	void HandleControllerRotation(float DeltaTime);

	/// <summary>
	/// MainSpringArm ���� ��ġ Dest���������� ��� Lerp ��Ű��
	/// </summary>
	void HandleLerpMainSpringArmToDestRelativeLocation(float DeltaTime);

public: // Getters and setters

	//TMap<EHandState, FPoseTurnInPlaceAnimMontage> TurnAnimMontageMap{};
	FPoseTurnInPlaceAnimMontage& GetPoseTurnAnimMontage(EHandState InHandState) { return TurnAnimMontageMap[InHandState]; }

	TMap<EHandState, FPoseTurnInPlaceAnimMontage>& GetLowerBodyTurnAnimMontageMap() { return LowerBodyTurnAnimMontageMap; }
	FPoseTurnInPlaceAnimMontage& GetLowerPoseTurnAnimMontage(EHandState InHandState) { return LowerBodyTurnAnimMontageMap[InHandState]; }

	void SetSpringArmRelativeLocationDest(EPoseState InNextPoseState) { MainSpringArmRelativeLocationDest = MainSpringArmRelativeLocationByPoseMap[InNextPoseState]; }
	void SetAimingSpringArmRelativeLocationDest(EPoseState InNextPoseState) { AimingSpringArmRelativeLocationDest = AimingSpringArmRelativeLocationByPoseMap[InNextPoseState]; }

	class UC_HUDWidget* GetHUDWidget() const { return HUDWidget; }
	 
	class UC_PingSystemComponent* GetPingSystemComponent() const { return PingSystemComponent; }

	class USpringArmComponent* GetMainSpringArm() const { return C_MainSpringArm; }

	UFUNCTION(BlueprintCallable)
	class UC_InvenSystem* GetInvenSystem() { return InvenSystem; }

	class UC_CameraEffectComponent* GetCameraEffectComponent() const { return CameraEffectComponent; }


	void SetCanFireWhileCrawl();
public:
	bool GetIsHighEnoughToFall() override;
public:
	/// <summary>
	/// �ڼ� �ٲٱ� ���� ó��
	/// </summary>
	/// <param name="InChangeFrom"> : �ٲٱ� �� �ڼ� </param>
	/// <param name="InChangeTo"> : �ٲ� �ڼ� </param>
	/// <returns> : ����� �ٲپ��ٸ� return true </returns>
	bool SetPoseState(EPoseState InChangeFrom, EPoseState InChangeTo) override;

protected:

	/// <summary>
	/// ������ ��ȣ�ۿ��� ���� ������ �Լ�.
	/// ���� ���� �����۰��� �浿�� �����ϴ� �Լ�.
	/// </summary>
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	//class USphereComponent* DetectionSphere;

	//UFUNCTION()
	//void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	//
	//UFUNCTION()
	//void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	// 
	//�ڽ� �ܰ迡�� OverlapBegin���� ����� �Լ� Template method
	virtual void HandleOverlapBegin(AActor* OtherActor) override;

	//�ڽ� �ܰ迡�� OverlapBegin���� ����� �Լ� Template method
	virtual void HandleOverlapEnd(AActor* OtherActor) override;
protected:
	//Aim Press Camera
	UPROPERTY(EditDefaultsOnly)
	class UCameraComponent* AimCamera{};
	UPROPERTY(EditDefaultsOnly)
	class USpringArmComponent* AimSpringArmTemp{};

protected: // Camera

	UPROPERTY(BluePrintReadWrite, EditAnywhere)
	class UCameraComponent* MainCamera{};

	UPROPERTY(BluePrintReadWrite, EditAnywhere)
	class USpringArmComponent* C_MainSpringArm{};

	FQuat InitialMainCameraRelativeRotation;

public:
	UCameraComponent* GetMainCamera() { return MainCamera; }
	UCameraComponent* GetAimCamera() const { return AimCamera; }
protected:
	//Aim Press Camera ��ġ ���� �Լ�
	void SetAimPressCameraLocation();

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
	/// <para> ĳ���Ͱ� Aiming ���� ��,��Ʈ�ѷ�(ī�޶�)���� ��� Turn In place�� ĳ���� ����</para>
	/// </summary>
	void HandleTurnInPlaceWhileAiming();

	void HandlePlayerRotationWhileAiming();

public:

	void ClampControllerRotationPitchWhileCrawl(EPoseState InCurrentState);
	
public:

	/// <summary>
	/// Turn in place�� ������ �� anim notify�� ���� ȣ��, ���� ���� ���� Rotation ���� ����� ���ư���
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void SetStrafeRotationToIdleStop();

protected:
	float AimingTurnInPlaceTimeCount = 0.0f;
	float SavedYaw = 0.0f;
	float DeltaYaw = 0.0f;

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
	void HandleStatesWhileMovingCrawl();

private:

	class UC_TimelineUtility* CameraTransitionTimelineComponent;
	float PrevMoveSpeed = 0;

public:
	UFUNCTION(CallInEditor)
	void HandleCameraTransitionInterpolation(float Value);

	UFUNCTION()
	void OnCameraTransitionTimelineFinished();

private:
	FVector InitialCameraLocation;
	FRotator InitialCameraRotation;
public:
	void SetTimeLineComponentForMovingCamera();

public:
	/// <summary>
	/// ���ؿ� ���� ī�޶� ����
	/// </summary>

	void RecoilController();
private:
	class UC_TimelineUtility* RecoilTimeline;
public:

	UFUNCTION(CallInEditor)
	void HandleRecoilInterpolation(float Value);

	UFUNCTION()
	void OnRecoilTimelineFinished();

	void SetRecoilTimeLineComponent();

	void SetRecoilTimelineValues(float InGunRPM);
	void SetRecoilFactorByPose();

private:
	float PlayerRecoilFactorByPose = 1.0f;

protected:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly) 
	class UC_InputComponent* MyInputComponent{};

protected: // Turn in place �ִ� ��Ÿ�� ����

	/// <summary>
	/// �� HandState�� PoseState�� ���� TurnAnimMontage ��
	/// </summary>
	UPROPERTY(BluePrintReadWrite, EditAnywhere)
	TMap<EHandState, FPoseTurnInPlaceAnimMontage> TurnAnimMontageMap{};

	/// <summary>
	/// �� HandState�� PoseState�� ���� Lower Body TurnAnimMontage �� : Lower body�� ���� ����� �ʿ��� �� ���
	/// </summary>
	UPROPERTY(BluePrintReadWrite, EditDefaultsOnly)
	TMap<EHandState, FPoseTurnInPlaceAnimMontage> LowerBodyTurnAnimMontageMap{};

protected:
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UC_HUDWidget* HUDWidget{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UC_InvenSystem* InvenSystem{};
protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UC_PingSystemComponent* PingSystemComponent{};

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UC_CameraEffectComponent* CameraEffectComponent{};

private:

	TMap<EPoseState, FVector> MainSpringArmRelativeLocationByPoseMap{};
	FVector MainSpringArmRelativeLocationDest{}; // Spring Arm Relative Location ��ġ Lerp��ų Destination ��

	TMap<EPoseState, FVector> AimingSpringArmRelativeLocationByPoseMap{};
	FVector AimingSpringArmRelativeLocationDest{}; // Spring Arm Relative Location ��ġ Lerp��ų Destination ��


protected:
	//�Ѿ� Object Pooling (World���� �۾��� ����)
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TArray<class AC_Bullet*> PooledBullets;

	void PoolingBullets();

	FCollisionQueryParams LineTraceCollisionParams{};

	void SetLineTraceCollisionIgnore();

public:
	TArray<AC_Bullet*>& GetBullets() { return PooledBullets; }

	FCollisionQueryParams& GetLineTraceCollisionParams() { return LineTraceCollisionParams; }

protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	class UC_CrosshairWidgetComponent* CrosshairWidgetComponent{};

public:
	UC_CrosshairWidgetComponent* GetCrosshairWidgetComponent() { return CrosshairWidgetComponent; }
	
};
