// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "GameFramework/Character.h"
#include "Components/TimelineComponent.h"
#include "Character/C_BasicCharacter.h"
#include "C_Player.generated.h"


// 二쇱꽍 Test

/// <summary>
/// Pose蹂?Turn left, right anim montage 援ъ“泥?
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
	/// Alt??(Hold Direction) Handling
	/// </summary>
	/// <param name="DeltaTime"></param>
	void HandleControllerRotation(float DeltaTime);

	/// <summary>
	/// MainSpringArm 濡쒖뺄 ?꾩튂 Dest蹂?섍컪?쇰줈 怨꾩냽 Lerp ?쒗궎湲?
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
	/// ?먯꽭 諛붽씀湲??듯빀 泥섎━
	/// </summary>
	/// <param name="InChangeFrom"> : 諛붽씀湲????먯꽭 </param>
	/// <param name="InChangeTo"> : 諛붽? ?먯꽭 </param>
	/// <returns> : ?쒕?濡?諛붽씀?덈떎硫?return true </returns>
	bool SetPoseState(EPoseState InChangeFrom, EPoseState InChangeTo) override;

protected:

	/// <summary>
	/// ?꾩씠???곹샇?묒슜???꾪븳 蹂?섏? ?⑥닔.
	/// 援щ? ?듯빐 ?꾩씠?쒓낵??異⑸룞??媛먯??섎뒗 ?⑥닔.
	/// </summary>
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	//class USphereComponent* DetectionSphere;

	//UFUNCTION()
	//void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	//
	//UFUNCTION()
	//void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	// 
	//?먯떇 ?④퀎?먯꽌 OverlapBegin?먯꽌 ?ъ슜???⑥닔 Template method
	virtual void HandleOverlapBegin(AActor* OtherActor) override;

	//?먯떇 ?④퀎?먯꽌 OverlapBegin?먯꽌 ?ъ슜???⑥닔 Template method
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
	//Aim Press Camera ?꾩튂 議곗젙 ?⑥닔
	void SetAimPressCameraLocation();

protected:

	/*
	移대찓?쇨? 諛붾씪蹂대뒗 諛⑺뼢?쇰줈 紐몄껜媛 ?뚯븘媛??湲곕낯 StrafeOn ?ㅼ젙 :
	BPC_Player(self) : Use Control Rotation Yaw			= true
	Spring Arm : Use Pawn Control Rotation				= true
	Character Movement : Orient Rotation to Movement	= false;

	媛留뚰엳 ?덉쓣 ??Use Control Rotation Yaw : false

	*/

	/// <summary>
	/// <para> 罹먮┃?곌? 硫덉떠?덉쓣 ?? 罹먮┃?곌? ?ㅻ툕?앺듃媛 諛붾씪蹂대뒗 諛⑺뼢怨?而⑦듃濡ㅻ윭(移대찓??媛 </para>
	/// <para> 諛붾씪蹂대뒗 諛⑺뼢??媛곸씠 90???댁긽?대㈃, Turn In place濡?罹먮┃??議곗젙 </para>
	/// </summary>
	void HandleTurnInPlace();

	/// <summary>
	/// <para> 罹먮┃?곌? Aiming 以묒씪 ??而⑦듃濡ㅻ윭(移대찓??諛⑺뼢 ?濡?Turn In place濡?罹먮┃??議곗젙</para>
	/// </summary>
	void HandleTurnInPlaceWhileAiming();

	void HandlePlayerRotationWhileAiming();

public:

	void SetControllerPitchLimits(EPoseState InCurrentState);
	
public:

	/// <summary>
	/// Turn in place媛 ?앸궗????anim notify???섑빐 ?몄텧, 硫덉떠 ?덉쓣 ?뚯쓽 Rotation ?명똿 媛믩뱾濡??뚯븘媛湲?
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void SetStrafeRotationToIdleStop();

protected:
	float AimingTurnInPlaceTimeCount = 0.0f;
	float SavedYaw = 0.0f;
	float DeltaYaw = 0.0f;

private:

	/// <summary>
	/// Turn Anim Montage 珥덇린??
	/// </summary>
	void InitTurnAnimMontageMap();

public:

	/// <summary>
	/// 議곗????곕Ⅸ 移대찓??蹂寃?
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
	/// 議곗????곕Ⅸ 移대찓??蹂寃?
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

protected: // Turn in place ?좊떂 紐쏀?二?愿??

	/// <summary>
	/// 媛?HandState? PoseState???곕Ⅸ TurnAnimMontage 留?
	/// </summary>
	UPROPERTY(BluePrintReadWrite, EditAnywhere)
	TMap<EHandState, FPoseTurnInPlaceAnimMontage> TurnAnimMontageMap{};

	/// <summary>
	/// 媛?HandState? PoseState???곕Ⅸ Lower Body TurnAnimMontage 留?: Lower body留??곕줈 ?ъ깮???꾩슂?????ъ슜
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
	FVector MainSpringArmRelativeLocationDest{}; // Spring Arm Relative Location ?꾩튂 Lerp?쒗궗 Destination 媛?

	TMap<EPoseState, FVector> AimingSpringArmRelativeLocationByPoseMap{};
	FVector AimingSpringArmRelativeLocationDest{}; // Spring Arm Relative Location ?꾩튂 Lerp?쒗궗 Destination 媛?


protected:
	//珥앹븣 Object Pooling (World?먯꽌 ?묒뾽???덉젙)
	//UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	//TArray<class AC_Bullet*> PooledBullets;

	//void PoolingBullets();

	FCollisionQueryParams LineTraceCollisionParams{};

	void SetLineTraceCollisionIgnore();

public:

	FCollisionQueryParams& GetLineTraceCollisionParams() { return LineTraceCollisionParams; }

protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	class UC_CrosshairWidgetComponent* CrosshairWidgetComponent{};

public:
	UC_CrosshairWidgetComponent* GetCrosshairWidgetComponent() { return CrosshairWidgetComponent; }
	
};


