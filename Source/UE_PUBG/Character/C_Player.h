// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "GameFramework/Character.h"
#include "Components/TimelineComponent.h"
#include "Character/C_BasicCharacter.h"
#include "C_Player.generated.h"


/// <summary>
/// Pose별 Turn left, right anim montage 구조체
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

public: // Input mapped actions

	void Move(const struct FInputActionValue& Value);
	
	/// <summary>
	/// Move가 끝날 때 호출됨, AnimCharacter에서 Speed Lerp할 값 setting 및 Turn in place 동작 초기 setting 
	/// </summary>
	void MoveEnd(const struct FInputActionValue& Value);

	void Look(const struct FInputActionValue& Value);

	void Crouch();
	void Crawl();

	void OnJump();
	void CancelTurnInPlaceMotion();
	//Alt 키 누를때 이름 추천부탁
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

	// TODO : 무기에 대한 마우스 버튼 클릭 처리만 해둠, 다른 처리도 필요할 예정
	void OnMLBStarted();
	void OnMLBOnGoing();
	void OnMLBCompleted();

	void OnMRBStarted();
	void OnMRBOnGoing();
	void OnMRBCompleted();

	void OnSprintStarted();
	void OnSprintReleased();

	void OnWalkStarted();
	void OnWalkReleased();

	//상호작용(F)
	//오브젝트의 묶음별로 만들어야 할 수도?
	//아니면 그냥 UObject로 만들기
	void Interaction();

/// <summary>
/// 아이템 상호작용을 위한 변수와 함수.
/// 구를 통해 아이템과의 충동을 감지하는 함수.
/// </summary>
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* DetectionSphere;

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:

	class UC_InputComponent* GetInputComponent() { return MyInputComponent; }//부모 클래스에 존재


public:
	//Aim Press Camera
	UPROPERTY(EditDefaultsOnly)
	class UCameraComponent* AimCamera;
	UPROPERTY(EditDefaultsOnly)
	class USpringArmComponent* AimSpringArmTemp;
protected:
	//Aim Press Camera 위치 조정 함수
	void HandleAimPressCameraLocation();

protected:

	/*
	카메라가 바라보는 방향으로 몸체가 돌아가는 기본 StrafeOn 설정 :
	BPC_Player(self) : Use Control Rotation Yaw			= true
	Spring Arm : Use Pawn Control Rotation				= true
	Character Movement : Orient Rotation to Movement	= false;

	가만히 있을 시 Use Control Rotation Yaw : false

	*/

	/// <summary>
	/// <para> 캐릭터가 멈춰있을 때, 캐릭터가 오브젝트가 바라보는 방향과 컨트롤러(카메라)가 </para>
	/// <para> 바라보는 방향의 각이 90도 이상이면, Turn In place로 캐릭터 조정 </para>
	/// </summary>
	void HandleTurnInPlace();

	/// <summary>
	/// <para> 캐릭터가 Aiming 중일 때,컨트롤러(카메라)방향 대로 Turn In place로 캐릭터 조정</para>
	/// </summary>
	void HandleTurnInPlaceWhileAiming();

	void HandlePlayerRotationWhileAiming();

	void ClampControllerRotationPitchWhileCrawl(EPoseState InCurrentState);
	
	/// <summary>
	/// Turn in place가 끝났을 시 anim notify에 의해 호출, 멈춰 있을 때의 Rotation 세팅 값들로 돌아가기
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void SetStrafeRotationToIdleStop();

protected:
	float AimingTurnInPlaceTimeCount = 0.0f;
	float SavedYaw = 0.0f;
	float DeltaYaw = 0.0f;

private:

	/// <summary>
	/// Turn Anim Montage 초기화
	/// </summary>
	void InitTurnAnimMontageMap();

private:
	/// <summary>
	/// Tick 함수에서 호출될 함수 / AimPunching 관여
	/// </summary>
	void HandleCameraAimPunching(float DeltaTime);

public:

	/// <summary>
	/// Camera AimPunching 실시
	/// </summary>
	/// <param name="CamPunchingDirection"> : 카메라를 Aim Punching할 방향 </param>
	/// <param name="CamPunchIntensity"> : 해당 방향으로 얼마나 보낼지 </param>
	/// <param name="CamRotationPunchingXDelta"> : X Rotation(Roll) 회전량 </param>
	/// <param name="InPunchingLerpFactor"> : Punching Lerp 인자 </param>
	void ExecuteCameraAimPunching
	(
		FVector CamPunchingDirection,
		float CamPunchIntensity,
		float CamRotationPunchingXDelta = 0.f,
		float InPunchingLerpFactor = 8.f
	);

	/// <summary>
	/// 카메라 Shake 수행
	/// </summary>
	/// <param name="ShakeScale"></param>
	void ExecuteCameraShake(float ShakeScale = 1.f);

private:

	/// <summary>
	/// Tick 함수에서 호출될 함수 / FlashBangEffect 관련 처리
	/// </summary>
	void HandleFlashBangEffect(float DeltaTime);

	/// <summary>
	/// 플레이어가 현재 바라보는 화면을 캡쳐하기
	/// </summary>
	void CaptureScene();

public:

	/// <summary>
	/// FlashBangEffect 수행
	/// </summary>
	/// <param name="Duration"> : 지속시간 / 지속시간이 현재 진행중인 effect의 지속시간보다 짧으면  Update하지 않음 </param>
	void ExecuteFlashBangEffect(float Duration);


public:
	/// <summary>
	/// 조준에 따른 카메라 변경
	/// </summary>
	void SetToAimKeyPress();
	void SetToAimDownSight();

	void BackToMainCamera();

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UTimelineComponent* CameraTransitionTimeline;

	//TimeLine 진행중일 때 호출 되는 함수포인터
	FOnTimelineFloat InterpFunction{};
	//TimeLine 이 끝나면 호출 되는 함수포인터
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
protected:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly) 
	class UC_InputComponent* MyInputComponent{};

protected: // Turn in place 애님 몽타주 관련

	/// <summary>
	/// 각 HandState와 PoseState에 따른 TurnAnimMontage 맵
	/// </summary>
	UPROPERTY(BluePrintReadWrite, EditAnywhere)
	TMap<EHandState, FPoseTurnInPlaceAnimMontage> TurnAnimMontageMap{};

	/// <summary>
	/// 각 HandState와 PoseState에 따른 Lower Body TurnAnimMontage 맵 : Lower body만 따로 재생이 필요할 시 사용
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

private: // Camera Aim Punching 관련
	
	// 기존 Camera local location & local rotation
	FVector		MainCamOriginLocalLocation{};
	FVector		AimCamOriginLocalLocation{};
	FRotator	MainCamOriginLocalRotation{};
	FRotator	AimCamOriginLocalRotation{};

	// AimCamera 또한 필요

	// AimPunching을 적용시킬 Camera Local Location 위치 좌표
	FVector MainCamPunchingDestLocation{};
	FVector AimCamPunchingDestLocation{};

	// AimPunching을 적용시킬 Camera Local Rotation 위치 좌표
	FRotator MainCamPunchingDestRotation{};
	FRotator AimCamPunchingDestRotation{};

	float PunchingLerpFactor = 8.f;

protected: 
	// Camera Shake
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "CameraShake")
	TSubclassOf<UCameraShakeBase> CameraShakeClass{};

protected: // Flash Bang 피격 Effect 관련

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




};
