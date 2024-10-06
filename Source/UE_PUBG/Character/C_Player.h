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

private:

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

	class UC_HUDWidget* GetHUDWidget() const { return HUDWidget; }

	class UC_PingSystemComponent* GetPingSystemComponent() const { return PingSystemComponent; }

private:
	void SetPoseState(EPoseState InPoseState) { Super::SetPoseState(InPoseState); }

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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* DetectionSphere;

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
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

private:
	/// <summary>
	/// Tick �Լ����� ȣ��� �Լ� / AimPunching ����
	/// </summary>
	void HandleCameraAimPunching(float DeltaTime);

public:

	/// <summary>
	/// Camera AimPunching �ǽ�
	/// </summary>
	/// <param name="CamPunchingDirection"> : ī�޶� Aim Punching�� ���� </param>
	/// <param name="CamPunchIntensity"> : �ش� �������� �󸶳� ������ </param>
	/// <param name="CamRotationPunchingXDelta"> : X Rotation(Roll) ȸ���� </param>
	/// <param name="InPunchingLerpFactor"> : Punching Lerp ���� </param>
	void ExecuteCameraAimPunching
	(
		FVector CamPunchingDirection,
		float CamPunchIntensity,
		float CamRotationPunchingXDelta = 0.f,
		float InPunchingLerpFactor = 8.f
	);

	/// <summary>
	/// ī�޶� Shake ����
	/// </summary>
	/// <param name="ShakeScale"></param>
	void ExecuteCameraShake(float ShakeScale = 1.f);

private:

	/// <summary>
	/// Tick �Լ����� ȣ��� �Լ� / FlashBangEffect ���� ó��
	/// </summary>
	void HandleFlashBangEffect(float DeltaTime);

	/// <summary>
	/// �÷��̾ ���� �ٶ󺸴� ȭ���� ĸ���ϱ�
	/// </summary>
	void CaptureScene();

public:

	/// <summary>
	/// FlashBangEffect ����
	/// </summary>
	/// <param name="Duration"> : ���ӽð� / ���ӽð��� ���� �������� effect�� ���ӽð����� ª����  Update���� ���� </param>
	void ExecuteFlashBangEffect(float Duration);


public:
	/// <summary>
	/// ���ؿ� ���� ī�޶� ����
	/// </summary>
	void SetToAimKeyPress();
	void SetToAimDownSight();

	void BackToMainCamera();

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UTimelineComponent* CameraTransitionTimeline;

	//TimeLine �������� �� ȣ�� �Ǵ� �Լ�������
	FOnTimelineFloat InterpFunction{};
	//TimeLine �� ������ ȣ�� �Ǵ� �Լ�������
	FOnTimelineEvent TimelineFinished{};
	UFUNCTION(CallInEditor)
	void HandleInterpolation(float Value);

	UFUNCTION()
	void OnTimelineFinished();

	FVector InitialCameraLocation;
	FRotator InitialCameraRotation;

	void SetTimeLineComponentForMovingCamera();

	class UCurveFloat* CreateCurveFloatForSwitchCamera();

	UPROPERTY(EditDefaultsOnly)
	UCurveFloat* CurveFloatForSwitchCameraChange{};

private:
	void SpawnConsumableItemForTesting();

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

private: // Camera Aim Punching ����
	
	// ���� Camera local location & local rotation
	FVector		MainCamOriginLocalLocation{};
	FVector		AimCamOriginLocalLocation{};
	FRotator	MainCamOriginLocalRotation{};
	FRotator	AimCamOriginLocalRotation{};

	// AimCamera ���� �ʿ�

	// AimPunching�� �����ų Camera Local Location ��ġ ��ǥ
	FVector MainCamPunchingDestLocation{};
	FVector AimCamPunchingDestLocation{};

	// AimPunching�� �����ų Camera Local Rotation ��ġ ��ǥ
	FRotator MainCamPunchingDestRotation{};
	FRotator AimCamPunchingDestRotation{};

	float PunchingLerpFactor = 8.f;

protected: 
	// Camera Shake
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "CameraShake")
	TSubclassOf<UCameraShakeBase> CameraShakeClass{};

protected: // Flash Bang �ǰ� Effect ����

	class APostProcessVolume* PostProcessVolume{};
	float FlashBangEffectDuration{};

	float PostProcessInitialIntensity{};

	class USceneCaptureComponent2D* SceneCaptureComponent{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UTextureRenderTarget2D* RenderTarget{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UC_ScreenShotWidget* ScreenShotWidget{};

protected:
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UC_HUDWidget* HUDWidget{};

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UC_PingSystemComponent* PingSystemComponent{};


public: // Consumable Item Testing
	
	//// TODO : �����
	//UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	//class AC_ConsumableItem* ConsumableItem{};
	//
	//UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	//TSubclassOf<class AC_ConsumableItem> ConsumableItemClass{};
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TArray<TSubclassOf<class AC_ConsumableItem>> ConsumableItemClasses{};
	
	int ConsumableIterator{};

	TArray<class AC_ConsumableItem*> ConsumableItems{};

private:

	TMap<EPoseState, FVector> MainSpringArmRelativeLocationByPoseMap{};
	FVector MainSpringArmRelativeLocationDest{}; // Spring Arm Relative Location ��ġ Lerp��ų Destination ��


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
