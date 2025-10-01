// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/C_BasicCharacter.h"
#include "C_Player.generated.h"

/// <summary>
/// Pose 별 Turn left, right anim montage 구조체
/// </summary>
USTRUCT(BlueprintType)
struct FPoseTurnInPlaceAnimMontage
{
	GENERATED_BODY()

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

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SetPlayerMappingContext();

	/// <summary>
	/// 레벨의 모든 Actor의 BeginPlay 이 후 호출될 함수
	/// </summary>
	void OnPostWorldBeginPlay();

	// 낙하중 바람 소리 재생
	UFUNCTION(BlueprintImplementableEvent)
	void PlaySkyDivingSound();

	// 낙하중 바람 소리 정지
	UFUNCTION(BlueprintImplementableEvent)
	void StopSkyDivingSound();

	// 낙하중 바람 소리가 중간에 끝나면 재생 
	// TODO : looping으로 처리한 지금, 필요한 기능인지 체크하기.
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateSkyDivingSound();

	// 투척류 휠 토글
	UFUNCTION(BlueprintImplementableEvent)
	void ToggleThrowablegWeaponWheel();

	// 힐템 휠 토글
	UFUNCTION(BlueprintImplementableEvent)
	void ToggleConsumableWheel();
private:

	/// <summary>
	/// Alt키 (Hold Direction) Handling
	/// </summary>
	/// <param name="DeltaTime"></param>
	void HandleControllerRotation(float DeltaTime);

	/// <summary>
	/// MainSpringArm 로컬 위치 Dest변수값으로 계속 Lerp 시키기
	/// </summary>
	void HandleLerpMainSpringArmToDestRelativeLocation(const float& DeltaTime);

public: // Getters and setters

	//TMap<EHandState, FPoseTurnInPlaceAnimMontage> TurnAnimMontageMap{};
	FPoseTurnInPlaceAnimMontage& GetPoseTurnAnimMontage(EHandState InHandState) { return TurnAnimMontageMap[InHandState]; }

	TMap<EHandState, FPoseTurnInPlaceAnimMontage>& GetLowerBodyTurnAnimMontageMap() { return LowerBodyTurnAnimMontageMap; }
	FPoseTurnInPlaceAnimMontage& GetLowerPoseTurnAnimMontage(EHandState InHandState) { return LowerBodyTurnAnimMontageMap[InHandState]; }

	void SetMainSpringArmRelativeLocationDest(EPoseState InNextPoseState) { MainSpringArmRelativeLocationDest = MainSpringArmRelativeLocationByPoseMap[InNextPoseState]; }
	void SetAimingSpringArmRelativeLocationDest(EPoseState InNextPoseState) { AimingSpringArmRelativeLocationDest = AimingSpringArmRelativeLocationByPoseMap[InNextPoseState]; }

public:

	UFUNCTION(BlueprintCallable)
	class UC_HUDWidget*		GetHUDWidget()		const { return HUDWidget; }
	class UC_MainMapWidget* GetMainMapWidget()	const { return MainMapWidget; }
	class UC_GameOverWidget* GetGameOverWidget() const { return GameOverWidget; }
	 
	class UC_PingSystemComponent* GetPingSystemComponent() const { return PingSystemComponent; }

	class USpringArmComponent* GetMainSpringArm() const { return C_MainSpringArm; }

	UFUNCTION(BlueprintCallable)
	class UC_InvenSystem* GetInvenSystem() { return InvenSystem; }

	class UC_CameraEffectComponent* GetCameraEffectComponent() const { return CameraEffectComponent; }

	AC_Item* GetCurOutLinedItem() { return CurOutLinedItem; }

	void SetCanFireWhileCrawl();

	class UC_PlayerSkyDivingComponent* GetPlayerSkyDivingComponent() const { return PlayerSkyDivingComponent; }

	UUserWidget* GetThrowingWheel() { return ThrowableWheelWidget; }

	UUserWidget* GetConsumableWheel() { return ConsumableWheelWidget; }

	class UC_CrosshairWidgetComponent* GetCrosshairWidgetComponent() { return CrosshairWidgetComponent; }

	class UC_PlayerDeafenedHandler* GetDeafenedHandler() const { return DeafenedHandler; }

	class UC_InputComponent* GetInputComponent() const { return MyInputComponent; }
		
	class AC_PreviewCharacter* GetPreviewCharacter() const { return PreviewCharacter; }
	
public:
	/// <summary>
	/// 자세 바꾸기 통합 처리
	/// </summary>
	/// <param name="InChangeFrom"> : 바꾸기 전 자세 </param>
	/// <param name="InChangeTo"> : 바꿀 자세 </param>
	/// <returns> : 제대로 바꾸었다면 return true </returns>
	bool SetPoseState(EPoseState InChangeFrom, EPoseState InChangeTo) override;

protected:

	/// <summary>
	/// 아이템 상호작용을 위한 변수와 함수.
	/// 구를 통해 아이템과의 충동을 감지하는 함수.
	/// </summary>\
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	//class USphereComponent* DetectionSphere;

	//UFUNCTION()
	//void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	//
	//UFUNCTION()
	//void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//자식 단계에서 OverlapBegin에서 사용할 함수 Template method
	virtual void HandleOverlapBegin(AActor* OtherActor) override;

	//자식 단계에서 OverlapBegin에서 사용할 함수 Template method
	virtual void HandleOverlapEnd(AActor* OtherActor) override;

	/// <summary>
	/// 아웃라인을 그릴 적합한 아이템 찾는 함수.
	/// </summary>
	/// <returns></returns>
	AC_Item* FindBestInteractable();

	/// <summary>
	/// 현재 그리고 있는 아웃라인의 주인(아이템)을 교체 및 유지 확인을 하는 함수.
	/// </summary>
	/// <param name="InteractableItem"></param>
	void UpdateInteractable(AC_Item* InteractableItem);

	/// <summary>
	/// 카메라에서 라인트레이스를 통해서 아이템을 감지하면 아이템의 outline을 그리고 감지에서 벗어나면 꺼주기
	/// </summary>
	void DrawingItemOutLine();

	void PoolingBullets() override;

private:
	void CharacterDead(const FKillFeedDescriptor& KillFeedDescriptor) override;
	void EnableRagdoll() override;

	void DestroyCharacter() override;

public:
	class UCameraComponent* GetMainCamera() const { return MainCamera; }
	UCameraComponent* GetAimCamera() const { return AimCamera; }
protected:
	//Aim Press Camera 위치 조정 함수
	void SetAimPressCameraLocation();

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

public:

	void SetControllerPitchLimits(EPoseState InCurrentState);
	
public:

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

public:

	/// <summary>
	/// 조준에 따른 카메라 변경
	/// </summary>
	void SetToAimKeyPress();
	void SetToAimDownSight();

	void BackToMainCamera();
	void HandleStatesWhileMovingCrawl();
	bool GetIsTooCloseToAimGun();
	bool CurrentCanAimGun = false;
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
	/// 조준에 따른 카메라 변경
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
	AC_Item* CurOutLinedItem = nullptr;

protected:
	//Aim Press Camera
	UPROPERTY(EditDefaultsOnly)
	class UCameraComponent* AimCamera{};
	UPROPERTY(EditDefaultsOnly)
	USpringArmComponent* AimSpringArmTemp{};

protected: // Camera

	UPROPERTY(BluePrintReadWrite, EditAnywhere)
	UCameraComponent* MainCamera{};

	UPROPERTY(BluePrintReadWrite, EditAnywhere)
	USpringArmComponent* C_MainSpringArm{};

	FQuat InitialMainCameraRelativeRotation;

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

protected:
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UC_HUDWidget* HUDWidget{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UC_MainMapWidget* MainMapWidget{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UC_GameOverWidget* GameOverWidget{};	

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UUserWidget* ThrowableWheelWidget = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UUserWidget* ConsumableWheelWidget = nullptr;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UC_InvenSystem* InvenSystem{};
	
protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UC_PingSystemComponent* PingSystemComponent{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UC_CameraEffectComponent* CameraEffectComponent{};

private:

	// 부모의 SkyDivingComponent 객체와 동일한 객체
	UC_PlayerSkyDivingComponent* PlayerSkyDivingComponent{};

private: // MainSpringArm & AimingSpringArm Lerp Destinations

	TMap<EPoseState, FVector> MainSpringArmRelativeLocationByPoseMap{};
	FVector MainSpringArmRelativeLocationDest{}; // Spring Arm Relative Location 위치 Lerp시킬 Destination 값

	TMap<EPoseState, FVector> AimingSpringArmRelativeLocationByPoseMap{};
	FVector AimingSpringArmRelativeLocationDest{}; // Spring Arm Relative Location 위치 Lerp시킬 Destination 값

	float SpringArmRelativeLocationLerpSpeed = 5.f;


protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	UC_CrosshairWidgetComponent* CrosshairWidgetComponent{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UC_PlayerDeafenedHandler* DeafenedHandler{};
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class AC_PreviewCharacter* PreviewCharacter{};
};


