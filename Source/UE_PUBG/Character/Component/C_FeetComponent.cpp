// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/C_FeetComponent.h"
#include "Character/C_BasicCharacter.h"
#include "Kismet/kismetSystemLibrary.h"
#include "Kismet/kismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Utility/C_Util.h"

//#include "DrawDebugHelpers.h"
#include "Engine/SkeletalMeshSocket.h"
// Sets default values for this component's properties
UC_FeetComponent::UC_FeetComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;


}


// Called when the game starts
void UC_FeetComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<AC_BasicCharacter>(GetOwner());
	
}


// Called every frame
void UC_FeetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	float LeftDistance, RightDistance;

	FRotator LeftRotation, RightRotation;

	Trace(LeftSocket, LeftDistance, LeftRotation);
	Trace(RightSocket, RightDistance, RightRotation);

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

void UC_FeetComponent::Trace(FName InName, float& OutDistance, FRotator& OutRotation)
{
	if (OwnerCharacter == nullptr)
		return;

	FVector Socket = OwnerCharacter->GetMesh()->GetSocketLocation(InName);

	float PosZ = OwnerCharacter->GetActorLocation().Z;

	FVector Start = FVector(Socket.X, Socket.Y, PosZ);

	PosZ = Start.Z - OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - TraceDistance;

	FVector End = FVector(Socket.X, Socket.Y, PosZ);

	// FHitResult HitResult;
	// TArray<AActor*> IgnoreActors;
	// IgnoreActors.Add(OwnerCharacter);

	// bool IsHit = UKismetSystemLibrary::LineTraceSingle
	// (
	// 	GetWorld(),
	// 	Start, End,
	// 	ETraceTypeQuery::TraceTypeQuery1,
	// 	true,
	// 	IgnoreActors,
	// 	DrawDebugType,
	// 	HitResult,
	// 	true,
	// 	FLinearColor::Green,
	// 	FLinearColor::Red
	// );
	
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

	OutDistance = 0;
	OutRotation = FRotator::ZeroRotator;


	if (!HitResult.bBlockingHit)
	{
		CurrentSurfaceType = SurfaceType_Default; // 허공이면 SurfaceType_Default로
		return;
	}

	float Length = (HitResult.ImpactPoint - HitResult.TraceEnd).Size();

	OutDistance = Length - TraceDistance + OffsetDistance;

	float Roll = UKismetMathLibrary::DegAtan2(HitResult.Normal.Y, HitResult.Normal.Z);
	float Pitch = -UKismetMathLibrary::DegAtan2(HitResult.Normal.X, HitResult.Normal.Z);

	Pitch = FMath::Clamp(Pitch, -15.0f, 15.0f);
	Roll  = FMath::Clamp(Roll, -15.0f, 15.0f);

	OutRotation = FRotator(Pitch, 0, Roll);

	CurrentSurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());

	//if (bIsNowOnGround && !bWasOnGroundLastFrame)
	//{
	//	PlaySoundCue(CurrentSurfaceType, HitResult.ImpactPoint);
	//}

	//if (bIsNowOnGround && !bWasOnGroundLastFrame)
	//{
	//	if (SurfaceTypeToSoundCueMap.Contains(CurrentSurfaceType))
	//	{
	//		UE_LOG(LogTemp, Warning, TEXT("발소리 재생: SurfaceType %d, Location (%s)"), (int32)CurrentSurfaceType, *HitResult.ImpactPoint.ToString());
	//	}
	//	else
	//	{
	//		UE_LOG(LogTemp, Warning, TEXT("SurfaceTypeToSoundCueMap에 매칭 없음: %d"), (int32)CurrentSurfaceType);
	//	}
	//
	//	PlaySoundCue(CurrentSurfaceType, GetOwner()->GetActorLocation());
	//}
	//
	//bWasOnGroundLastFrame = bIsNowOnGround;

}

void UC_FeetComponent::PlaySoundCue(EPhysicalSurface InCurSurFaceTpye, FVector InLocation)
{
	if (SurfaceTypeToSoundCueMap.Contains(InCurSurFaceTpye))
	{
		USoundCue* SoundCue = SurfaceTypeToSoundCueMap[InCurSurFaceTpye];
		if (SoundCue)
		{
			FVector Velocity = OwnerCharacter->GetVelocity();
			float Speed = Velocity.Size();

			float VolumeMultiplier = FMath::Clamp(Speed / 650, 0.5f, 1.2f);

			UGameplayStatics::PlaySoundAtLocation(this, SoundCue, InLocation, VolumeMultiplier);
		}
	}
}
