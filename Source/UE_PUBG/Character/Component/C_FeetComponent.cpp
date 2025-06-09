// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/C_FeetComponent.h"
#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"

#include "Kismet/kismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Utility/C_Util.h"


#include "C_SwimmingComponent.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


UC_FeetComponent::UC_FeetComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UC_FeetComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<AC_BasicCharacter>(GetOwner());

	LeftFootSoundDescriptor.FootAudioComponent = NewObject<UAudioComponent>(this);
	LeftFootSoundDescriptor.FootAudioComponent->SetAutoActivate(false);

	// OwnerCharacter가 Player의 경우, 2D Sound로 재생 setting / Enemy의 경우 3D Sound
	LeftFootSoundDescriptor.FootAudioComponent->bAllowSpatialization = (Cast<AC_Player>(OwnerCharacter) == nullptr);

	// AttachToComponent를 하면, 3D 재생의 경우 위치 지정을 하지 않더라도 자동으로 해당 위치에서 재생처리됨
	LeftFootSoundDescriptor.FootAudioComponent->AttachToComponent
	(
		OwnerCharacter->GetRootComponent(),
		FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
		FName("LeftFootSocket")
	);
	
	RightFootSoundDescriptor.FootAudioComponent = NewObject<UAudioComponent>(this);
	RightFootSoundDescriptor.FootAudioComponent->SetAutoActivate(false);
	RightFootSoundDescriptor.FootAudioComponent->AttachToComponent
	(
		OwnerCharacter->GetRootComponent(),
		FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
		FName("RightFootSocket")
	);
	RightFootSoundDescriptor.FootAudioComponent->bAllowSpatialization = (Cast<AC_Player>(OwnerCharacter) == nullptr);
	
	/*FString AddressString = FString::Printf(TEXT("0x%p"), LeftFootSoundDescriptor.SurfaceTypeToSoundCueMap[SurfaceType9]);
	UC_Util::Print(AddressString, FColor::Cyan, 20.f);*/
    
}

void UC_FeetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsValid(OwnerCharacter)) return;

	// 각각, Crawl 자세일 때 / 수영중일 때 / 파쿠르 중일 때 IK 적용 x
	if (OwnerCharacter->GetPoseState() == EPoseState::CRAWL  ||
		OwnerCharacter->GetSwimmingComponent()->IsSwimming() ||
		OwnerCharacter->GetCharacterMovement()->MovementMode == MOVE_Flying)
	{
		Data.LeftDistance		= FVector::ZeroVector;
		Data.RightDistance		= FVector::ZeroVector;
		Data.RootBoneDistance	= FVector::ZeroVector;
		Data.LeftRotation		= FRotator::ZeroRotator;
		Data.RightRotation		= FRotator::ZeroRotator;
		return;
	}

	/*Data.LeftDistance		= FVector::ZeroVector;
	Data.RightDistance		= FVector::ZeroVector;
	Data.RootBoneDistance	= FVector::ZeroVector;
	Data.LeftRotation		= FRotator::ZeroRotator;
	Data.RightRotation		= FRotator::ZeroRotator;
	return;*/


	float LeftDistance{}, RightDistance{};
	FRotator LeftRotation{}, RightRotation{};
	
	Trace(LeftSocket, LeftDistance, LeftRotation, LeftFootSoundDescriptor);
	Trace(RightSocket, RightDistance, RightRotation, RightFootSoundDescriptor);

	//늘어난 다리를 안늘어난 다리에 맞추도록 안늘어난 다리의 길이를 저장
	float Offset = FMath::Min(LeftDistance, RightDistance);

	//Data.RootBoneDistance.Z = Offset;
	//보간을 통해 자연스럽게 만들어줌
	Data.RootBoneDistance.Z = UKismetMathLibrary::FInterpTo(Data.RootBoneDistance.Z, Offset, DeltaTime, InterpSpeed);

	Data.LeftDistance =
		UKismetMathLibrary::VInterpTo(Data.LeftDistance, FVector(0, 0, LeftDistance - Offset), DeltaTime, InterpSpeed);

	Data.RightDistance =
		UKismetMathLibrary::VInterpTo(Data.RightDistance, FVector(0, 0, RightDistance - Offset), DeltaTime, InterpSpeed);


	Data.LeftRotation =
		UKismetMathLibrary::RInterpTo(Data.LeftRotation, LeftRotation, DeltaTime, InterpSpeed);

	Data.RightRotation =
		UKismetMathLibrary::RInterpTo(Data.RightRotation, RightRotation, DeltaTime, InterpSpeed);
}

void UC_FeetComponent::Trace(const FName& InName, float& OutDistance, FRotator& OutRotation, FFootSoundDescriptor& OutSoundDescriptor)
{
	FVector Socket = OwnerCharacter->GetMesh()->GetSocketLocation(InName);

	// 수영을 하기 직전까지 물 발소리 처리를 위한 LineTrace PosZ 위치 지정
	const float StartPosZ = OwnerCharacter->GetSwimmingComponent()->GetWaterDetectionColliderLowestZ();
	FVector Start = FVector(Socket.X, Socket.Y, StartPosZ);

	float EndPosZ = OwnerCharacter->GetActorLocation().Z - OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - TraceDistance;
	FVector End = FVector(Socket.X, Socket.Y, EndPosZ);

	/*FHitResult HitResult{};
	FCollisionQueryParams CollisionParams{};
	CollisionParams.AddIgnoredActor(OwnerCharacter);
	CollisionParams.bReturnPhysicalMaterial = true;

	bool IsHit = GetWorld()->LineTraceSingleByChannel
	(
		HitResult,
		Start,
		End,
		ECC_Visibility,
		CollisionParams
	);*/

	TArray<FHitResult> HitResults{};
	FCollisionQueryParams CollisionParams{};
	CollisionParams.AddIgnoredActor(OwnerCharacter);

	TArray<AActor*> AttachedActors{};
	OwnerCharacter->GetAttachedActors(AttachedActors);
	CollisionParams.AddIgnoredActors(AttachedActors);
	
	CollisionParams.bReturnPhysicalMaterial = true;

	// 물가의 경우, 2번째 Hit된 지물에 Foot IK를 적용시켜야 함 -> Trace 결과도 두 번째 지형지물의 값으로 Reference 초기화 시키기
	bool IsHit = GetWorld()->LineTraceMultiByChannel
	(
		HitResults,
		Start,
		End,
		ECC_Visibility,
		CollisionParams
	);

	OutDistance = 0;
	OutRotation = FRotator::ZeroRotator;

	// DrawDebugLine(GetWorld(), Start, End, FColor::Red, false);
	
	if (!IsHit)
	{
		OutDistance = 0.f;
		OutSoundDescriptor.CurrentSurfaceType = SurfaceType_Default;
		return;
	}

	// for (const FHitResult& HitResult : HitResults) DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 5.f, 10, FColor::Green, false);

	// 첫 Line blocked된 지형지물의 유형이 물이라면, 두 번째 LineTraced된 지형지물의 값을 Foot IK 값으로 사용
	OutSoundDescriptor.CurrentSurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitResults[0].PhysMaterial.Get());

	// 현재 Trace하는 지형지물에 Water가 존재하는지에 따른 FootIK용 HitResult를 지정해주어야 함
	FHitResult HitResultForFootIK{};

	// 현재 Project Setting의 SurfaceType9번이 Water로 지정되어 있음
	if (OutSoundDescriptor.CurrentSurfaceType == SurfaceType9)
	{
		// Water 지형지물만 잡힌 상황 / Foot IK처리를 할 필요 없음(애초에 이 상황은 나오지 않지만 예외처리로 둠)
		if (HitResults.Num() <= 1)
		{
			OutSoundDescriptor.bIsDeepWater = false;
			return;
		}

		HitResultForFootIK = HitResults[1];
		OutSoundDescriptor.bIsDeepWater = (HitResults[0].ImpactPoint.Z - HitResults[1].ImpactPoint.Z > 30.f);
	}
	else HitResultForFootIK = HitResults[0]; // 일반적인 땅 상황

	float Length = (HitResultForFootIK.ImpactPoint - HitResultForFootIK.TraceEnd).Size();

	OutDistance = Length - TraceDistance + OffsetDistance;

	float Roll = UKismetMathLibrary::DegAtan2(HitResultForFootIK.Normal.Y, HitResultForFootIK.Normal.Z);
	float Pitch = -UKismetMathLibrary::DegAtan2(HitResultForFootIK.Normal.X, HitResultForFootIK.Normal.Z);

	Pitch = FMath::Clamp(Pitch, -15.0f, 15.0f);
	Roll  = FMath::Clamp(Roll, -15.0f, 15.0f);

	OutRotation = FRotator(Pitch, 0, Roll);
	// OutSurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitResultForFootIK.PhysMaterial.Get());
}

void UC_FeetComponent::PlaySoundCue(EPhysicalSurface InCurSurFaceType, FVector InLocation, float InVolumeMultiplier, bool bLeftFootSound)
{
	if (OwnerCharacter->GetCharacterMovement()->IsFalling()) return;
	
	const FFootSoundDescriptor& SoundDescriptor = bLeftFootSound ? LeftFootSoundDescriptor : RightFootSoundDescriptor;
	
	if (!SoundDescriptor.SurfaceTypeToSoundCueMap.Contains(InCurSurFaceType)) return;
	
	USoundCue* SoundCue = SoundDescriptor.SurfaceTypeToSoundCueMap[InCurSurFaceType];
	if (!SoundCue) return;
	
	if (Cast<AC_Player>(OwnerCharacter))
	{
		UGameplayStatics::PlaySound2D(this, SoundCue, InVolumeMultiplier);
		// UC_Util::Print("PlaySound2D", FColor::Emerald, 10.f);
	}
	else
	{
		UGameplayStatics::PlaySoundAtLocation(this, SoundCue, InLocation, InVolumeMultiplier);
		// UC_Util::Print("PlaySoundAtLocation", FColor::Emerald, 10.f);
	}
}

void UC_FeetComponent::PlaySoundByAudioComponent(EPhysicalSurface InCurSurFaceType, float InVolumeMultiplier, bool bLeftFootSound)
{
	if (OwnerCharacter->GetCharacterMovement()->IsFalling()) return;
	
	const FFootSoundDescriptor& SoundDescriptor = bLeftFootSound ? LeftFootSoundDescriptor : RightFootSoundDescriptor;
	
	if (!SoundDescriptor.SurfaceTypeToSoundCueMap.Contains(InCurSurFaceType)) return;
	
	USoundCue* SoundCue = SoundDescriptor.SurfaceTypeToSoundCueMap[InCurSurFaceType];
	if (!SoundCue) return;
	
	SoundDescriptor.FootAudioComponent->SetSound(SoundCue);
	SoundDescriptor.FootAudioComponent->SetVolumeMultiplier(InVolumeMultiplier);
	SoundDescriptor.FootAudioComponent->SetBoolParameter("IsDeepWater", SoundDescriptor.bIsDeepWater);
	SoundDescriptor.FootAudioComponent->Play();
}


