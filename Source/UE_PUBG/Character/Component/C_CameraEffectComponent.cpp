// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/C_CameraEffectComponent.h"

#include "Character/C_Player.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/GameplayStatics.h"

#include "Utility/C_Util.h"

UC_CameraEffectComponent::UC_CameraEffectComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SceneCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>("SceneCaptureComponent");
}

void UC_CameraEffectComponent::BeginPlay()
{
	Super::BeginPlay();

	// Init MainCamera & AimCamera
	MainCamera	= OwnerPlayer->GetMainCamera();
	AimCamera	= OwnerPlayer->GetAimCamera();

	// AimPunching ���ƿ� �� ���� Local ��ǥ��
	MainCamOriginLocalLocation	= MainCamera->GetRelativeLocation();
	MainCamOriginLocalRotation	= MainCamera->GetRelativeRotation();
	MainCamPunchingDestLocation = MainCamOriginLocalLocation;
	MainCamPunchingDestRotation = MainCamOriginLocalRotation;

	AimCamOriginLocalLocation	= AimCamera->GetRelativeLocation();
	AimCamOriginLocalRotation	= AimCamera->GetRelativeRotation();
	AimCamPunchingDestLocation	= AimCamOriginLocalLocation;
	AimCamPunchingDestRotation	= AimCamOriginLocalRotation;

	// PostProcessVolume �ʱ�ȭ
	TArray<AActor*> PPVolumes{};
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APostProcessVolume::StaticClass(), PPVolumes);

	if (!PPVolumes.IsEmpty()) PostProcessVolume = Cast<APostProcessVolume>(PPVolumes[0]);
	PostProcessInitialIntensity = PostProcessVolume->Settings.BloomIntensity;

	//ScreenShotWidget->AddToViewport();
}


void UC_CameraEffectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	HandleCameraAimPunching(DeltaTime);
	HandleFlashBangEffect(DeltaTime);
}

void UC_CameraEffectComponent::HandleCameraAimPunching(const float& DeltaTime)
{
	if (!IsValid(MainCamera) || !IsValid(AimCamera)) return;

	FVector		MainCamPos = MainCamera->GetRelativeLocation();
	FVector		AimCamPos = AimCamera->GetRelativeLocation();
	FRotator	MainCamRot = MainCamera->GetRelativeRotation();
	FRotator	AimCamRot = AimCamera->GetRelativeRotation();

	if (FVector::Dist(MainCamPos, MainCamPunchingDestLocation) < 0.1f) // Destination���� �����ߴٰ� �Ǵ�
	{
		// �ٽñ� ����ġ�� ����
		MainCamPunchingDestLocation = MainCamOriginLocalLocation;
		MainCamPunchingDestRotation.Roll = MainCamOriginLocalRotation.Roll;
	}

	if (FVector::Dist(AimCamPos, AimCamPunchingDestLocation) < 0.1f)
	{
		AimCamPunchingDestLocation = AimCamOriginLocalLocation;
		AimCamPunchingDestRotation.Roll = AimCamOriginLocalRotation.Roll;
	}

	//if (FMath::Abs(MainCamRot.Roll - MainCamPunchingDestRotation.Roll) < 0.1f)
	//if (FMath::Abs(AimCamRot.Roll - AimCamPunchingDestRotation.Roll) < 0.1f)

	MainCamPos = FMath::Lerp(MainCamPos, MainCamPunchingDestLocation, PunchingLerpFactor * DeltaTime);
	AimCamPos = FMath::Lerp(AimCamPos, AimCamPunchingDestLocation, PunchingLerpFactor * DeltaTime);

	MainCamRot.Roll = FMath::Lerp(MainCamRot.Roll, MainCamPunchingDestRotation.Roll, PunchingLerpFactor * DeltaTime);
	AimCamRot.Roll = FMath::Lerp(AimCamRot.Roll, AimCamPunchingDestRotation.Roll, PunchingLerpFactor * DeltaTime);

	MainCamera->SetRelativeLocation(MainCamPos);
	AimCamera->SetRelativeLocation(AimCamPos);
	MainCamera->SetRelativeRotation(MainCamRot);
	AimCamera->SetRelativeRotation(AimCamRot);

	//UC_Util::Print(float(AimCamera->GetRelativeRotation().Roll));
	//UC_Util::Print(float(MainCamera->GetRelativeRotation().Roll));

}

void UC_CameraEffectComponent::ExecuteCameraAimPunching
(
	FVector CamPunchingDirection,
	float CamPunchIntensity,
	float CamRotationPunchingXDelta,
	float InPunchingLerpFactor
)
{
	// TODO : ���� AimDownSight�̸� �ٸ��� ó�� (Character Animation���� ó���ؾ� �� ��)
	// TODO : Aim Down�� ���� Main Camera�� �����ϰ� ��� �ϰ� ó�� (�ڼ� �ٲ� ���� ���� ī�޶� ���� ���� ��Ī�� �Ͼ�� ������ ����� ��)

	CamPunchingDirection.Normalize();

	MainCamPunchingDestLocation = MainCamOriginLocalLocation + CamPunchingDirection * CamPunchIntensity;
	AimCamPunchingDestLocation = AimCamOriginLocalLocation + CamPunchingDirection * CamPunchIntensity;

	MainCamPunchingDestRotation.Roll = MainCamOriginLocalRotation.Roll + CamRotationPunchingXDelta;
	AimCamPunchingDestRotation.Roll = AimCamOriginLocalRotation.Roll + CamRotationPunchingXDelta;

	PunchingLerpFactor = InPunchingLerpFactor;
}

void UC_CameraEffectComponent::ExecuteCameraShake(float ShakeScale)
{
	APlayerController* PlayerController = Cast<APlayerController>(OwnerPlayer->GetController());

	if (PlayerController && PlayerController->PlayerCameraManager && CameraShakeClass)
		PlayerController->PlayerCameraManager->StartCameraShake(CameraShakeClass, ShakeScale);
}

void UC_CameraEffectComponent::HandleFlashBangEffect(const float& DeltaTime)
{
	FlashBangEffectDuration -= DeltaTime;

	// End of Effect
	if (FlashBangEffectDuration <= 0.f)
	{
		FlashBangEffectDuration = 0.f;

		PostProcessVolume->Settings.BloomIntensity = FMath::Lerp(PostProcessVolume->Settings.BloomIntensity, PostProcessInitialIntensity, DeltaTime * 10.f);

		// TODO : Capture�� �ܻ� �����

		return;
	}

	PostProcessVolume->Settings.BloomIntensity = 1000.f;
	//PostProcessVolume->Settings.BloomIntensity = 500.f;
}

void UC_CameraEffectComponent::CaptureScene()
{
	if (!SceneCaptureComponent)
	{
		UC_Util::Print("From UC_CameraEffectComponent::CaptureScene : SceneCaptureComponent not initialized!", FColor::Red, 5.f);
		return;
	}
	if (!RenderTarget)
	{
		UC_Util::Print("From UC_CameraEffectComponent::CaptureScene : RenderTarget not initialized!", FColor::Red, 5.f);
		return;
	}

	APlayerController* PlayerController = Cast<APlayerController>(OwnerPlayer->GetController());

	if (!IsValid(PlayerController))
	{
		UC_Util::Print("From UC_CameraEffectComponent::CaptureScene : PlayerController Invalid!", FColor::Red, 5.f);
		return;
	}

	AActor* ViewTarget = PlayerController->PlayerCameraManager->GetViewTarget();
	if (!IsValid(ViewTarget))
	{
		UC_Util::Print("From UC_CameraEffectComponent::CaptureScene : ViewTarget Invalid!", FColor::Red, 5.f);
		return;
	}

	UCameraComponent* CameraComponent = ViewTarget->FindComponentByClass<UCameraComponent>();

	if (!CameraComponent)
	{
		UC_Util::Print("From UC_CameraEffectComponent::CaptrueScene : Camera Component not found!", FColor::Red, 5.f);
		return;
	}

	SceneCaptureComponent->SetWorldTransform(CameraComponent->GetComponentTransform());
	SceneCaptureComponent->FOVAngle = CameraComponent->FieldOfView;

	SceneCaptureComponent->TextureTarget = RenderTarget;
	SceneCaptureComponent->CaptureScene();

	UTexture2D* NewTexture = RenderTarget->ConstructTexture2D(SceneCaptureComponent, TEXT("CapturedImage"), EObjectFlags::RF_NoFlags, CTF_DeferCompression);

	if (!NewTexture)
	{
		UC_Util::Print("Texture not created!", FColor::Red, 5.f);
		return;
	}

	// �ʿ信 ���� �ؽ��ĸ� �� ������ �� ���� (���� �Ҹ��� �� �𸣰���)
	NewTexture->UpdateResource();

	//FSlateBrush Brush{};
	//Brush.SetResourceObject(NewTexture);
	//ScreenShotWidget->GetDisplayImage()->SetBrush(Brush);
}

void UC_CameraEffectComponent::ExecuteFlashBangEffect(float Duration)
{
	// ������ ���� Duration ������ Duration�� ���Դٸ� ������ FlashBangEffect�� �������� ����
	// TODO PostProcess Bloom intensity ����
	if (FlashBangEffectDuration >= Duration) return;

	if (!IsValid(PostProcessVolume))
	{
		UC_Util::Print("From UC_CameraEffectComponent::ExecuteFlashBangEffect : PostProcessVolume Invalid!", FColor::Cyan, 5.f);
		return;
	}

	FlashBangEffectDuration = Duration;

	//CaptureScene();
	//FString Temp{};
	//FScreenshotRequest::RequestScreenshot(Temp, false, false);
}


