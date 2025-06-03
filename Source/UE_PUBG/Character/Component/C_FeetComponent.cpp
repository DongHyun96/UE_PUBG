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
#include "Components/BoxComponent.h"


UC_FeetComponent::UC_FeetComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	FeetWaterDetectionCollider = CreateDefaultSubobject<UCapsuleComponent>("FeetWaterDetectionCollider");
}

void UC_FeetComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<AC_BasicCharacter>(GetOwner());

	FeetWaterDetectionCollider->OnComponentBeginOverlap.AddDynamic(this, &UC_FeetComponent::OnFeetWaterDetectionColliderBeginOverlap);
	FeetWaterDetectionCollider->OnComponentEndOverlap.AddDynamic(this, &UC_FeetComponent::OnFeetWaterDetectionColliderEndOverlap);
	
	FeetWaterDetectionCollider->AttachToComponent(OwnerCharacter->GetRootComponent(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
}

void UC_FeetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsValid(OwnerCharacter)) return;
	
	HandleFeetWaterColliderStatusByPoseState(DeltaTime);

	if (Cast<AC_Player>(OwnerCharacter)) if (bIsLegOnWater) UC_Util::Print("LegOnWater");
	
	if (OwnerCharacter->GetPoseState() == EPoseState::CRAWL || OwnerCharacter->GetSwimmingComponent()->IsSwimming())
	{
		Data.LeftDistance		= FVector::ZeroVector;
		Data.RightDistance		= FVector::ZeroVector;
		Data.RootBoneDistance	= FVector::ZeroVector;
		Data.LeftRotation		= FRotator::ZeroRotator;
		Data.RightRotation		= FRotator::ZeroRotator;
		return;
	}


	float LeftDistance{}, RightDistance{};
	FRotator LeftRotation{}, RightRotation{};
	
	Trace(LeftSocket, LeftDistance, LeftRotation, CurrentLeftSurfaceType);
	Trace(RightSocket, RightDistance, RightRotation, CurrentRightSurfaceType);

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

void UC_FeetComponent::Trace(FName InName, float& OutDistance, FRotator& OutRotation, EPhysicalSurface& OutSurfaceType)
{
	FVector Socket = OwnerCharacter->GetMesh()->GetSocketLocation(InName);

	float PosZ = OwnerCharacter->GetActorLocation().Z;

	FVector Start = FVector(Socket.X, Socket.Y, PosZ);

	PosZ = Start.Z - OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - TraceDistance;

	FVector End = FVector(Socket.X, Socket.Y, PosZ);

	FHitResult HitResult{};
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
	);

	// DrawDebugLine(GetWorld(), Start, HitResult.ImpactPoint, FColor::Red);

	OutDistance = 0;
	OutRotation = FRotator::ZeroRotator;

	if (!IsHit)
	{
		OutDistance = HitResult.Distance;
		OutSurfaceType = SurfaceType_Default;
		return;
	}

	float Length = (HitResult.ImpactPoint - HitResult.TraceEnd).Size();

	OutDistance = Length - TraceDistance + OffsetDistance;

	float Roll = UKismetMathLibrary::DegAtan2(HitResult.Normal.Y, HitResult.Normal.Z);
	float Pitch = -UKismetMathLibrary::DegAtan2(HitResult.Normal.X, HitResult.Normal.Z);

	Pitch = FMath::Clamp(Pitch, -15.0f, 15.0f);
	Roll  = FMath::Clamp(Roll, -15.0f, 15.0f);

	OutRotation = FRotator(Pitch, 0, Roll);

	OutSurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());
}

void UC_FeetComponent::PlaySoundCue(EPhysicalSurface InCurSurFaceType, FVector InLocation, float InVolumeMultiplier)
{
	if (!SurfaceTypeToSoundCueMap.Contains(InCurSurFaceType)) return;
	
	USoundCue* SoundCue = SurfaceTypeToSoundCueMap[InCurSurFaceType];
	if (!SoundCue) return;
	
	if (OwnerCharacter->IsLocallyControlled())
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

void UC_FeetComponent::OnFeetWaterDetectionColliderBeginOverlap
(
	UPrimitiveComponent*	OverlappedComponent,
	AActor*					OtherActor,
	UPrimitiveComponent*	OtherComp,
	int32					OtherBodyIndex,
	bool					bFromSweep,
	const FHitResult&		SweepResult
)
{
	UC_Util::Print("Feet Water Collider BeginOverlap", FColor::Red, 10.f);
	bIsLegOnWater = true;
}

void UC_FeetComponent::OnFeetWaterDetectionColliderEndOverlap
(
	UPrimitiveComponent*	OverlappedComponent,
	AActor*					OtherActor,
	UPrimitiveComponent*	OtherComp,
	int32					OtherBodyIndex
)
{
	UC_Util::Print("Feet Water Collider EndOverlap", FColor::Red, 10.f);
	bIsLegOnWater = false;
}

void UC_FeetComponent::HandleFeetWaterColliderStatusByPoseState(float DeltaTime)
{
	if (OwnerCharacter->GetPoseState() == EPoseState::CRAWL) return;

	static const TMap<EPoseState, float> PoseByFeetWaterColliderZLocationDest =
	{
		{EPoseState::STAND,		-21.595007f},
		{EPoseState::CROUCH,	0.f},
		{EPoseState::CRAWL,		0.f}
	};

	FVector RelativeLocation = FeetWaterDetectionCollider->GetRelativeLocation(); 
	float Z = FMath::Lerp
	(
		FeetWaterDetectionCollider->GetRelativeLocation().Z,
		PoseByFeetWaterColliderZLocationDest[OwnerCharacter->GetPoseState()],
		DeltaTime * 10.f
	);
	RelativeLocation.Z = Z;
	
	FeetWaterDetectionCollider->SetRelativeLocation(RelativeLocation);
}

void UC_FeetComponent::OnPoseStateCrawlToAny()
{
	FeetWaterDetectionCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void UC_FeetComponent::OnPoseStateChangedToCrawl()
{
	FeetWaterDetectionCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
