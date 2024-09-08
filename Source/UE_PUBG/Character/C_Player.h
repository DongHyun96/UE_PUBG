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

public: // HP HUD event ����

	/// <summary>
	/// �������Ʈ �ʿ��� ������ Event Function, HP Bar ������Ʈ�� ���Ѿ� �� �� ȣ��
	/// </summary>
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateHPOnHUD();

public: // HUD event ����

	/// <summary>
	/// �������Ʈ �ʿ��� ������ Event Function
	/// </summary>
	/// <param name="HealUpDestHPValue"> : ���� Heal up block�� Dest Value </param>
	/// <param name="TimeRemain"> : �ߵ����� ���� �ð� </param>
	UFUNCTION(BlueprintImplementableEvent)
	void OnActivatingHealUp(float HealUpDestHPValue, float UsageTime, float UsingTimer);
	//Player->OnActivatingHealUp(HealUpDestHPValue, UsageTime, UsingTimer);

	/// <summary>
	/// �������Ʈ �ʿ��� ������ Event Function
	/// </summary>
	/// <param name="TimeRemain"> : �ߵ����� ���� �ð� </param>
	UFUNCTION(BlueprintImplementableEvent)
	void OnActivatingBooster(float UsageTime, float UsingTimer);

	/// <summary>
	/// �������Ʈ �ʿ��� ������ Event Function, Activating �ð��� �� á�� �� ȣ�� ������ Event
	/// </summary>
	UFUNCTION(BlueprintImplementableEvent)
	void OnConsumableItemActivatingEnd();


	/// <summary>
	/// �������Ʈ���� ������ Event Function, Heal Consumable item �� ActivateCompleted State tick���� ȣ���� Event
	/// </summary>
	/// <param name="BlockDestHP"> : ���� Block�� �� ä��� �ѷ� Destination HP </param>
	/// <param name="BlockTimeRemainRate"> : ���� Heal up block�� �ð� ���� ����(0.f ~ 1.f) </param>
	UFUNCTION(BlueprintImplementableEvent)
	void OnActivateHealUpCompletedTick(float BlockDestHP, float BlockTimeRemainRate);

	/// <summary>
	/// �������Ʈ���� ������ Event Function, consumable item �� Used���� ȣ���� Event
	/// </summary>
	UFUNCTION(BlueprintImplementableEvent)
	void OnConsumableUsed();


	/// <summary>
	/// �������Ʈ���� ������ Event Function, consumable item �� CancelActivating���� ȣ���� Event
	/// </summary>
	UFUNCTION(BlueprintImplementableEvent)
	void OnCancelActivatingConsumableItem();

	/// <summary>
	/// �������Ʈ���� ������ Event Function, �ν�Ʈ �� ������Ʈ �� ����� Event
	/// </summary>
	UFUNCTION(BlueprintImplementableEvent)
	void OnUpdateBoostingHUD(float CurBoosting);

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
	class UC_HUDComponent* HUDComponent{};


public:
	// Consumable Item Testing
	// TODO : �����
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class AC_ConsumableItem* ConsumableItem{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TSubclassOf<class AC_ConsumableItem> ConsumableItemClass{};

private:

	TMap<EPoseState, FVector> MainSpringArmRelativeLocationByPoseMap{};
	FVector MainSpringArmRelativeLocationDest{}; // Spring Arm Relative Location ��ġ Lerp��ų Destination ��

};
