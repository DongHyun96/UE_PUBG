// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_CameraEffectComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE_PUBG_API UC_CameraEffectComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UC_CameraEffectComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetOwnerPlayer(class AC_Player* InOwnerPlayer) { OwnerPlayer = InOwnerPlayer; }

private:
	/// <summary>
	/// Tick 함수에서 호출될 함수 / AimPunching 관여
	/// </summary>
	void HandleCameraAimPunching(const float& DeltaTime);

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
	void HandleFlashBangEffect(const float& DeltaTime);

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

private:

	class AC_Player* OwnerPlayer{};
	
	class UCameraComponent* MainCamera{};
	class UCameraComponent* AimCamera{};

private:

	// 기존 Camera local location & local rotation
	FVector		MainCamOriginLocalLocation{};
	FVector		AimCamOriginLocalLocation{};
	FRotator	MainCamOriginLocalRotation{};
	FRotator	AimCamOriginLocalRotation{};

private: // Camera Aim Punching 관련

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
	
private: // Flash Bang 피격 Effect 관련
	
	APostProcessVolume* PostProcessVolume{};
	float FlashBangEffectDuration{};

	float PostProcessInitialIntensity{};

	class USceneCaptureComponent2D* SceneCaptureComponent{};

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UTextureRenderTarget2D* RenderTarget{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UC_ScreenShotWidget* ScreenShotWidget{};

private:

	// 한 번이라도 Punched 되었는지 조사
	bool bEverPunched{};

protected: // Player의 경우에만 수류탄 및 섬광탄 피격 당했을 때 먹먹함 effect와 섬광탄 피격음을 재생시키면 됨

	// 섬광탄 피격 당했을 때의 Player 피격음
	UPROPERTY(BluePrintReadWrite, EditDefaultsOnly)
	USoundBase* FlashBangStunnedSound{};
	
private:

	UAudioComponent* StunnedAudioComponent{};
	bool bHasStunnedAudioComponentFadeOutStart = true;
};


