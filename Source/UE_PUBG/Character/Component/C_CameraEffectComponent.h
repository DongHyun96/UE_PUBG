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
	/// Tick �Լ����� ȣ��� �Լ� / AimPunching ����
	/// </summary>
	void HandleCameraAimPunching(const float& DeltaTime);

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
	void HandleFlashBangEffect(const float& DeltaTime);

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

private:

	class AC_Player* OwnerPlayer{};
	
	class UCameraComponent* MainCamera{};
	class UCameraComponent* AimCamera{};

private:

	// ���� Camera local location & local rotation
	FVector		MainCamOriginLocalLocation{};
	FVector		AimCamOriginLocalLocation{};
	FRotator	MainCamOriginLocalRotation{};
	FRotator	AimCamOriginLocalRotation{};

private: // Camera Aim Punching ����

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

private: // Flash Bang �ǰ� Effect ����

	class APostProcessVolume* PostProcessVolume{};
	float FlashBangEffectDuration{};

	float PostProcessInitialIntensity{};

	class USceneCaptureComponent2D* SceneCaptureComponent{};

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UTextureRenderTarget2D* RenderTarget{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UC_ScreenShotWidget* ScreenShotWidget{};

		
};
