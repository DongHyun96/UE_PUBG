// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/C_FeetComponent.h"
#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"

#include "Kismet/kismetSystemLibrary.h"
#include "Kismet/kismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Utility/C_Util.h"

//#include "DrawDebugHelpers.h"
#include "C_SwimmingComponent.h"
#include "Engine/SkeletalMeshSocket.h"
UC_FeetComponent::UC_FeetComponent()
{
	PrimaryComponentTick.bCanEverTick = true;


}

void UC_FeetComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<AC_BasicCharacter>(GetOwner());
	
}

void UC_FeetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsValid(OwnerCharacter)) return;
	if (OwnerCharacter->GetPoseState() == EPoseState::CRAWL) return;

	float LeftDistance{}, RightDistance{};

	FRotator LeftRotation{}, RightRotation{};

	Trace(LeftSocket, LeftDistance, LeftRotation);
	Trace(RightSocket, RightDistance, RightRotation);

	// UC_Util::Print(RightDistance);

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

	// PlaySoundInTick(DeltaTime);
	HandleFootSounds();
}

void UC_FeetComponent::Trace(FName InName, float& OutDistance, FRotator& OutRotation)
{
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

	// DrawDebugLine(GetWorld(), Start, HitResult.ImpactPoint, FColor::Red);

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

void UC_FeetComponent::PlaySoundInTick(float DeltaTime)
{
	if (OwnerCharacter == nullptr) return;

	//TODO : 속도에 따른 소리 크기

	FVector Velocity = OwnerCharacter->GetVelocity();
	float Speed = Velocity.Size();

	if (Speed < 10.f)
	{
		AccumulatedFootstepTime = 0.f; // 멈췄으면 초기화
		return;
	}

	//float FootstepInterval = FMath::Clamp(670.f / Speed, 0.2f, 1.0f);
	float FootstepInterval = 1.25f - Speed / 670.f;

	AccumulatedFootstepTime += DeltaTime; // 매 프레임 델타타임만큼 누적
	//UC_Util::Print(Speed);
	//UC_Util::Print(FootstepInterval);
	//UC_Util::Print(AccumulatedFootstepTime);

	if (AccumulatedFootstepTime >= FootstepInterval)
	{
		//FVector Velocity = OwnerCharacter->GetVelocity();
		//float Speed = Velocity.Size();

		float VolumeMultiplier = FMath::Clamp(Speed / 670.f, 0.5f, 1.2f);

		PlaySoundCue(CurrentSurfaceType, GetOwner()->GetActorLocation(), VolumeMultiplier);
		AccumulatedFootstepTime = 0.f; // 발소리 내고 다시 초기화
	}
	//// 캐릭터의 속도 계산
	//FVector Velocity = OwnerCharacter->GetVelocity();
	//float Speed = Velocity.Size();  // 캐릭터의 속도

	//if (Speed < 10.f) return;

	//float FootstepInterval = 0.0f;

	//if (Speed > 600.f)
	//{
	//	FootstepInterval = .2;
	//}
	//else if (Speed > 480.f)
	//{
	//	FootstepInterval = .3f;
	//}
	//else if (Speed > 400.f)
	//{
	//	FootstepInterval = .4f;
	//}
	//else if (Speed > 340.f)
	//{
	//	FootstepInterval = .5f;
	//}
	//else if (Speed > 120.f)
	//{
	//	FootstepInterval = .6f;
	//}
	//else
	//{
	//	FootstepInterval = .9f;
	//}
	////UC_Util::Print(Speed);

	//// 속도에 따른 발소리 재생 간격 계산
	//// 속도가 빠르면 소리 간격이 짧고, 느리면 길어진다.
	////float FootstepInterval = FMath::Clamp(670.f / Speed , 0.2f, 1.f);  // 속도가 500을 넘지 않도록 클램프
	////UC_Util::Print(FootstepInterval);

	//// 일정 시간마다 발소리 재생
	//// GetWorld()->GetTimeSeconds()로 게임 시작 시간 이후의 경과 시간을 확인
	//if (GetWorld()->GetTimeSeconds() - LastFootstepTime >= FootstepInterval)
	//{
	//	PlaySoundCue(CurrentSurfaceType, GetOwner()->GetActorLocation());
	//	LastFootstepTime = GetWorld()->GetTimeSeconds();  // 발소리가 재생된 시간 기록
	//}
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

void UC_FeetComponent::HandleFootSounds()
{
	if (OwnerCharacter->GetMainState() != EMainState::IDLE)		return;
	if (OwnerCharacter->GetSwimmingComponent()->IsSwimming())	return;
	if (OwnerCharacter->GetPoseState() == EPoseState::CRAWL)	return;

	const float SoleTraceDistance = 100.f;
	const float GroundDetachThreshold = 0.5f; // 발이 지면에서 떨어졌다고 판단되기 시작하는 높이
	
	// Left foot sole 높이 조사
	float CurLeftFootSoleHeight{}, CurRightFootSoleHeight{};
	const FVector LeftFootSoleLocation = OwnerCharacter->GetMesh()->GetSocketLocation(LeftFootSoleSocket);
	const FVector RightFootSoleLocation = OwnerCharacter->GetMesh()->GetSocketLocation(RightFootSoleSocket);
	
	const bool bLeftHit  = GetDistanceToFloor(CurLeftFootSoleHeight, LeftFootSoleLocation, SoleTraceDistance);
	const bool bRightHit = GetDistanceToFloor(CurRightFootSoleHeight, RightFootSoleLocation, SoleTraceDistance);

	if (OwnerCharacter->GetPoseState() == EPoseState::CROUCH)
	{
		CurLeftFootSoleHeight  = FMath::Max(0.f, CurLeftFootSoleHeight - 4.5f);
		CurRightFootSoleHeight = FMath::Max(0.f, CurRightFootSoleHeight - 4.5f);
	}

	/*if (Cast<AC_Player>(OwnerCharacter))
		UC_Util::Print(CurLeftFootSoleHeight);*/

	// 발이 떨어졌다고 판단되면 bSoundPlayed 초기화
	if (!bLeftHit || CurLeftFootSoleHeight > GroundDetachThreshold)   bLeftFootSoundPlayed  = false;
	if (!bRightHit || CurRightFootSoleHeight > GroundDetachThreshold) bRightFootSoundPlayed = false;

	LeftFootSoleMaxDistance  = bLeftHit  ? FMath::Max(LeftFootSoleMaxDistance, CurLeftFootSoleHeight)   : SoleTraceDistance;
	RightFootSoleMaxDistance = bRightHit ? FMath::Max(RightFootSoleMaxDistance, CurRightFootSoleHeight) : SoleTraceDistance;

	const FVector2D FootHeightRangeForVolumeMultiplier = {0.f, 80.f};
	const FVector2D VolumeMultiplierRange = {0.3f, 1.f};
	
	// Trace 검사 성공 시에만 발소리 내기 시도
	// Hit 성공했고, 0.2cm 이하라면, 발소리 내기 시도
	if (bLeftHit && CurLeftFootSoleHeight < 0.2f && !bLeftFootSoundPlayed) 
	{
		// 17 44 70
		const float VolumeMultiplier = FMath::GetMappedRangeValueClamped(FootHeightRangeForVolumeMultiplier, VolumeMultiplierRange, LeftFootSoleMaxDistance);
		PlaySoundCue(CurrentSurfaceType, LeftFootSoleLocation, VolumeMultiplier);
		bLeftFootSoundPlayed = true;
	}

	if (bRightHit && CurRightFootSoleHeight < 0.2f && !bRightFootSoundPlayed)
	{
		const float VolumeMultiplier = FMath::GetMappedRangeValueClamped(FootHeightRangeForVolumeMultiplier, VolumeMultiplierRange, RightFootSoleMaxDistance);
		PlaySoundCue(CurrentSurfaceType, RightFootSoleLocation, VolumeMultiplier);
		bRightFootSoundPlayed = true;
	}
	
}

bool UC_FeetComponent::GetDistanceToFloor(float& OutDistance, const FVector& TraceStartLocation, const float InTraceDistance)
{
	OutDistance = 0.f;
	
	FVector TraceEndLocation = TraceStartLocation - FVector::UnitZ() * InTraceDistance;

	FHitResult HitResult{};
	FCollisionQueryParams CollisionParams{};
	CollisionParams.AddIgnoredActor(OwnerCharacter);

	bool IsHit = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStartLocation, TraceEndLocation, ECC_Visibility, CollisionParams);

	if (!IsHit) return false;

	OutDistance = HitResult.Distance;
	return true;
}
