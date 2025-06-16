// Fill out your copyright notice in the Description page of Project Settings.


#include "C_PlayerDeafenedHandler.h"

#include "Camera/CameraComponent.h"
#include "Character/C_Player.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Utility/C_Util.h"

FCollisionQueryParams UC_PlayerDeafenedHandler::CollisionParams{};

UC_PlayerDeafenedHandler::UC_PlayerDeafenedHandler()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UC_PlayerDeafenedHandler::BeginPlay()
{
	Super::BeginPlay();
	PlayerMainCamera = OwnerPlayer->GetMainCamera();
	
	CollisionParams.bReturnPhysicalMaterial = true;

	UnderWaterAudioComponent = NewObject<UAudioComponent>(this);
	UnderWaterAudioComponent->SetAutoActivate(false);
	UnderWaterAudioComponent->bAllowSpatialization = true;
	UnderWaterAudioComponent->AttachToComponent(OwnerPlayer->GetRootComponent(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true));
	UnderWaterAudioComponent->SetSound(UnderWaterSFX);
}

void UC_PlayerDeafenedHandler::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	DeafenedTime = FMath::Max(DeafenedTime - DeltaTime, 0.f);

	// DeafenedEffectTime이 다 줄어들었고, Camera가 UnderWater밑이 아니면 DeafenedSoundMix 지우기
	if (DeafenedTime <= 0.f && !HandleMainCameraUnderWater())
	{
		UGameplayStatics::ClearSoundMixModifiers(this);
		UnderWaterAudioComponent->Stop();
		bDeafened = false;
	}
}

void UC_PlayerDeafenedHandler::ExecuteDeafenedEffect(float DeafenedDuration)
{
	DeafenedTime = DeafenedDuration;
	
	if (bDeafened) return; // 이미 Deafened 처리가 된 상태라면 SoundMix Push를 또 다시 하지 않음
	
	UGameplayStatics::PushSoundMixModifier(this, DeafenedMix);
	bDeafened = true;
}

bool UC_PlayerDeafenedHandler::HandleMainCameraUnderWater()
{
	static const ECollisionChannel CameraWaterCheckChannel = ECC_GameTraceChannel3;
	static EPhysicalSurface WaterSurfaceType = SurfaceType9; // SurfaceType 9번이 Water로 지정되어 있음
	
	FVector Start = PlayerMainCamera->GetComponentLocation();
	FVector End = Start + FVector::UnitZ() * 500.f;

	FHitResult HitResult{};

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, CamWaterTraceChannel, CollisionParams);

	DrawDebugLine(GetWorld(), Start, bHit ? HitResult.ImpactPoint : End, FColor::Red);

	// 카메라가 물 속이 아님
	if (!bHit || UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get()) != WaterSurfaceType)
	{
		// UC_Util::Print("Not hit");
		return false;
	}
	
	DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10.f, 10, FColor::Green);

	// 카메라가 물 속에 있을 때
	
	if (!bDeafened)
	{
		UGameplayStatics::PushSoundMixModifier(this, DeafenedMix);
		bDeafened = true;
	}

	if (!UnderWaterAudioComponent->IsPlaying())
		UnderWaterAudioComponent->Play();
	
	return true;
}