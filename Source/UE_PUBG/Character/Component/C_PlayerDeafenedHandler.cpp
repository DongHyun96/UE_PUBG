// Fill out your copyright notice in the Description page of Project Settings.


#include "C_PlayerDeafenedHandler.h"

#include "Camera/CameraComponent.h"
#include "Character/C_Player.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Utility/C_Util.h"

UC_PlayerDeafenedHandler::UC_PlayerDeafenedHandler()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UC_PlayerDeafenedHandler::BeginPlay()
{
	Super::BeginPlay();
	PlayerMainCamera = OwnerPlayer->GetMainCamera();

	UnderWaterAudioComponent = NewObject<UAudioComponent>(this);
	UnderWaterAudioComponent->SetAutoActivate(false);
	UnderWaterAudioComponent->bAllowSpatialization = true;
	UnderWaterAudioComponent->AttachToComponent(OwnerPlayer->GetRootComponent(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true));
	UnderWaterAudioComponent->SetSound(UnderWaterSFX);

	CameraWaterCheckingCollider = Cast<UShapeComponent>(OwnerPlayer->GetDefaultSubobjectByName("CameraWaterCheckingCollider"));
	if (!CameraWaterCheckingCollider)
	{
		UC_Util::Print("From UC_PlayerDeafenedHandler::BeginPlay : CameraWaterCheckingCollider casting failed!", FColor::Red, 10.f);
		return;
	}

	CameraWaterCheckingCollider->OnComponentBeginOverlap.AddDynamic(this, &UC_PlayerDeafenedHandler::OnCameraWaterBeginOverlap);
}

void UC_PlayerDeafenedHandler::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Update Deafened Timer
	DeafenedTime = FMath::Max(DeafenedTime - DeltaTime, 0.f);

	// Check if camera exited water
	if (!bPendingCheckingCameraUnderWaterOnTickComponent && CameraWaterCheckingCollider->GetComponentLocation().Z > CameraWaterEnteredZ)
	{
		UC_Util::Print("Cam exits!");
		bIsCameraUnderWater = false;

		if (UnderWaterAudioComponent->IsPlaying())
			UnderWaterAudioComponent->Stop();
	}

	bPendingCheckingCameraUnderWaterOnTickComponent = false;

	// DeafenedEffectTime이 다 줄어들었고, Camera가 UnderWater밑이 아니면 DeafenedSoundMix 지우기
	if (DeafenedTime <= 0.f && !bIsCameraUnderWater)
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

void UC_PlayerDeafenedHandler::OnCameraWaterBeginOverlap
(
	UPrimitiveComponent*	OverlappedComp,
	AActor*					OtherActor,
	UPrimitiveComponent*	OtherComp,
	int32					OtherBodyIndex,
	bool					bFromSweep,
	const FHitResult&		SweepResult
)
{
	FString Str = "Entered Z : " + FString::SanitizeFloat(OtherActor->GetActorLocation().Z);
	
	// Tick pending을 한 번 걸어주어야 TickComponent에서 Exit 처리가 바로 안되고 잘 처리됨
	bPendingCheckingCameraUnderWaterOnTickComponent = true;
	
	UC_Util::Print(Str, FColor::MakeRandomColor());
	if (bIsCameraUnderWater) return; // 이미 물 속에 들어와 있는 상황
	
	CameraWaterEnteredZ = OtherActor->GetActorLocation().Z;
	bIsCameraUnderWater = true;
	
	if (!bDeafened)
	{
		UGameplayStatics::PushSoundMixModifier(this, DeafenedMix);
		bDeafened = true;
	}

	if (!UnderWaterAudioComponent->IsPlaying())
		UnderWaterAudioComponent->Play();
}
