// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/C_PoseColliderHandlerComponent.h"

#include "Components/CapsuleComponent.h"
#include "Utility/C_Util.h"

UC_PoseColliderHandlerComponent::UC_PoseColliderHandlerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	CrawlCapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>("CrawlCapsuleComponent");
}


void UC_PoseColliderHandlerComponent::BeginPlay()
{
	Super::BeginPlay();

	CrawlCapsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	OwnerCharacter->GetCapsuleComponent()->SetSimulatePhysics(false);
}


void UC_PoseColliderHandlerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	HandleLerpBodySizeByPose(DeltaTime);
	HandleCrawlColliderRotation(DeltaTime);
}

void UC_PoseColliderHandlerComponent::SetOwnerCharacter(AC_BasicCharacter* InOwnerCharacter)
{
	OwnerCharacter = InOwnerCharacter;
	CrawlCapsuleComponent->AttachToComponent(OwnerCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
}

void UC_PoseColliderHandlerComponent::SetColliderByPoseState(EPoseState InPoseState)
{
	RootColliderHeightRadiusLerpDest	= POSE_BY_ROOTCOLLIDER_HEIGHT_RADIUS[InPoseState];
	MeshZPosLerpDest					= POSE_BY_MESH_Z_POS[InPoseState];

	PoseBySizeLerpFlag = true;
}

bool UC_PoseColliderHandlerComponent::CanChangePoseOnCurrentSurroundEnvironment(EPoseState InChangeTo)
{
	if (OwnerCharacter->GetPoseState() == InChangeTo) return false;

	const float CRAWL_START_OFFSET = 20.f;

	switch (InChangeTo)
	{
	case EPoseState::STAND:
	{
		// Crouch to stand or Crawl to Stand
		FVector StartLocation	 = OwnerCharacter->GetActorLocation();

		StartLocation.Z			+= (OwnerCharacter->GetPoseState() == EPoseState::CROUCH) ? 
								   POSE_BY_ROOTCOLLIDER_HEIGHT_RADIUS[EPoseState::CROUCH].Key - SWEEP_SPHERE_RAD : CRAWL_START_OFFSET;

		FVector DestLocation	 = StartLocation + FVector::UnitZ() * ((OwnerCharacter->GetPoseState() == EPoseState::CROUCH) ? 
								   CROUCH_TO_STAND_SWEEP_DIST : CRAWL_TO_STAND_SWEEP_DIST);

		FCollisionQueryParams CollisionParams{};
		CollisionParams.AddIgnoredActor(OwnerCharacter);
		
		TArray<AActor*> AttachedActors{};
		OwnerCharacter->GetAttachedActors(AttachedActors);
		CollisionParams.AddIgnoredActors(AttachedActors);

		FHitResult HitResult{};

		bool HasHit = GetWorld()->SweepSingleByChannel
		(
			HitResult,
			StartLocation,
			DestLocation,
			FQuat::Identity,
			ECC_Visibility,
			FCollisionShape::MakeSphere(SWEEP_SPHERE_RAD),
			CollisionParams
		);

		if (HasHit) UC_Util::Print(HitResult.GetActor()->GetName());
		
		//DrawDebugSphere(GetWorld(), StartLocation, SWEEP_SPHERE_RAD, 10, FColor::Red, true);
		//DrawDebugSphere(GetWorld(), DestLocation,  SWEEP_SPHERE_RAD, 10, FColor::Red, true);

		return !HasHit;
	}
	case EPoseState::CROUCH: // Crawl to Crouch만 확인 하면 됨
	{
		if (OwnerCharacter->GetPoseState() == EPoseState::STAND) return true; // Stand to crouch -> 언제든 자세를 바꿀 수 있음

		// Crawl to Crouch test

		FVector StartLocation	 = OwnerCharacter->GetActorLocation();
		StartLocation.Z			+= CRAWL_START_OFFSET;
		FVector DestLocation	 = StartLocation + FVector::UnitZ() * CRAWL_TO_CROUCH_SWEEP_DIST;

		FCollisionQueryParams CollisionParams{};
		CollisionParams.AddIgnoredActor(OwnerCharacter);

		TArray<AActor*> AttachedActors{};
		OwnerCharacter->GetAttachedActors(AttachedActors);
		CollisionParams.AddIgnoredActors(AttachedActors);

		FHitResult HitResult{};

		bool HasHit = GetWorld()->SweepSingleByChannel
		(
			HitResult,
			StartLocation,
			DestLocation,
			FQuat::Identity,
			ECC_Visibility,
			FCollisionShape::MakeSphere(SWEEP_SPHERE_RAD),
			CollisionParams
		);

		if (HasHit) UC_Util::Print(HitResult.GetActor()->GetName());

		//DrawDebugSphere(GetWorld(), StartLocation, SWEEP_SPHERE_RAD, 10, FColor::Red, true);
		//DrawDebugSphere(GetWorld(), DestLocation,  SWEEP_SPHERE_RAD, 10, FColor::Red, true);
		
		return !HasHit;
	}
	case EPoseState::CRAWL:
	{
		// 높낮이가 높다하면 이미 떨어지는 상태이므로 Crawl 자체의 처리를 하지 않음 -> 높이 말고 경사도만 체크를 해주면 됨
		TPair<float, float> ImpactDistances{};
		float SlopeDegree = GetCrawlSlopeDegree(ImpactDistances, true);
		return FMath::Abs(SlopeDegree) < CRAWL_DEGREE_LIMIT;
	}
	case EPoseState::POSE_MAX: default: return false;
	}
}

void UC_PoseColliderHandlerComponent::HandleLerpBodySizeByPose(const float& DeltaTime)
{
	static const float LERP_DIFF_EPSILON = 0.2f;

	if (!PoseBySizeLerpFlag) return;

	float Radius{}, Height{};
	OwnerCharacter->GetCapsuleComponent()->GetUnscaledCapsuleSize(Radius, Height);

	FVector MeshLocation = OwnerCharacter->GetMesh()->GetRelativeLocation();
	float MeshZPos = MeshLocation.Z;

	// Lerp Finish
	if (FMath::Abs(Height - RootColliderHeightRadiusLerpDest.Key)	< LERP_DIFF_EPSILON &&
		FMath::Abs(Radius - RootColliderHeightRadiusLerpDest.Value) < LERP_DIFF_EPSILON &&
		FMath::Abs(MeshZPos - MeshZPosLerpDest)						< LERP_DIFF_EPSILON)
	{
		OwnerCharacter->GetCapsuleComponent()->SetCapsuleSize
		(
			RootColliderHeightRadiusLerpDest.Value,
			RootColliderHeightRadiusLerpDest.Key
		);

		OwnerCharacter->GetMesh()->SetRelativeLocation(FVector(MeshLocation.X, MeshLocation.Y, MeshZPosLerpDest));

		OwnerCharacter->GetCapsuleComponent()->SetSimulatePhysics(true);
		CrawlCapsuleComponent->SetCollisionEnabled((OwnerCharacter->GetPoseState() == EPoseState::CRAWL) ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
		OwnerCharacter->GetCapsuleComponent()->SetSimulatePhysics(false);

		OwnerCharacter->SetCanMove(true);
		PoseBySizeLerpFlag = false;

		return;
	}

	// Lerp CapsuleComponent size
	Height = FMath::Lerp(Height, RootColliderHeightRadiusLerpDest.Key,   DeltaTime * PoseBySizePosLerpSpeed);
	Radius = FMath::Lerp(Radius, RootColliderHeightRadiusLerpDest.Value, DeltaTime * PoseBySizePosLerpSpeed);

	OwnerCharacter->GetCapsuleComponent()->SetCapsuleSize(Radius, Height);

	// Lerp Mesh Location Z Pos
	MeshZPos = FMath::Lerp(MeshZPos, MeshZPosLerpDest, DeltaTime * PoseBySizePosLerpSpeed);

	FVector DestLocation = FVector(MeshLocation.X, MeshLocation.Y, MeshZPos);
	OwnerCharacter->GetMesh()->SetRelativeLocation(DestLocation);
}

void UC_PoseColliderHandlerComponent::HandleCrawlColliderRotation(const float& DeltaTime)
{
	if (OwnerCharacter->GetIsPoseTransitioning()) return;

	static const float LERP_SPEED		= 20.f;
	static const float HEIGHT_OFFSET	= 50.f;
	TPair<float, float> ImpactDistances{};

	if (OwnerCharacter->GetPoseState() != EPoseState::CRAWL) return;

	FRotator CrawlRelativeRotation	= CrawlCapsuleComponent->GetRelativeRotation();
	CrawlSlopeAngle					= GetCrawlSlopeAngle(ImpactDistances, HEIGHT_OFFSET);
	float SlopeDegree				= FMath::RadiansToDegrees(CrawlSlopeAngle);

	//UC_Util::Print(SlopeDegree);
	if (!CanCrawlOnSlope(SlopeDegree, ImpactDistances))
	{
		// 자세 전환 시도하기
		if (OwnerCharacter->SetPoseState(EPoseState::CRAWL, EPoseState::STAND))		return;
		if (OwnerCharacter->SetPoseState(EPoseState::CRAWL, EPoseState::CROUCH))	return;
	}

	CrawlCapsuleComponent->SetRelativeRotation(FRotator(90.f + SlopeDegree, 0.f, 0.f));
}

float UC_PoseColliderHandlerComponent::GetCrawlSlopeAngle(OUT TPair<float, float>& ImpactDistances, const float& HeightOffset, const bool& EnableDebugLine)
{
	FVector HeadStartLocation	= OwnerCharacter->GetActorLocation() + OwnerCharacter->GetActorForwardVector() * 75.f + FVector::UnitZ() * HeightOffset;
	FVector PelvisStartLocation = OwnerCharacter->GetActorLocation() + FVector::UnitZ() * HeightOffset;

	float SlopeAngle = GetCrawlSlopeAngle(HeadStartLocation, PelvisStartLocation, ImpactDistances, EnableDebugLine);
	ImpactDistances.Key -= HeightOffset;
	ImpactDistances.Value -= HeightOffset;

	return SlopeAngle;
}

float UC_PoseColliderHandlerComponent::GetCrawlSlopeAngle
(
	const FVector&				HeadStartLocation,
	const FVector&				PelvisStartLocation,
	OUT TPair<float, float>&	ImpactDistances,
	const bool&					EnableDebugLine
)
{
	FVector HeadDestLocation	= HeadStartLocation   - FVector::UnitZ() * CRAWL_LINETRACE_TEST_DIST;
	FVector PelvisDestLocation	= PelvisStartLocation - FVector::UnitZ() * CRAWL_LINETRACE_TEST_DIST;

	FCollisionQueryParams CollisionParams{};
	CollisionParams.AddIgnoredActor(OwnerCharacter);

	TArray<AActor*> AttachedActors{};
	OwnerCharacter->GetAttachedActors(AttachedActors);
	CollisionParams.AddIgnoredActors(AttachedActors);

	FHitResult HeadHitResult{};
	FHitResult PelvisHitResult{};

	bool HasHeadHit		= GetWorld()->LineTraceSingleByChannel(HeadHitResult, HeadStartLocation, HeadDestLocation, ECC_Visibility, CollisionParams);
	bool HasPelvisHit	= GetWorld()->LineTraceSingleByChannel(PelvisHitResult, PelvisStartLocation, PelvisDestLocation, ECC_Visibility, CollisionParams);

	if (EnableDebugLine)
	{
		DrawDebugLine(GetWorld(), HeadStartLocation, HeadHitResult.ImpactPoint, FColor::Red, true);
		DrawDebugLine(GetWorld(), PelvisStartLocation, PelvisHitResult.ImpactPoint, FColor::Red, true);
	}

	if (!HasHeadHit || !HasPelvisHit) return false;
	// TODO : Length 체크
	//if (HeadHitResult.ImpactPoint.Length() )

	// 경사도 체크
	//HeadHitResult.ImpactPoint
	float A				= (HeadHitResult.ImpactPoint - PelvisHitResult.ImpactPoint).Length();
	float B				= FMath::Abs(HeadHitResult.ImpactPoint.Z - PelvisHitResult.ImpactPoint.Z);
	float SlopeAngle	= FMath::Asin(B / A);

	// 부호 지정
	SlopeAngle = (HeadHitResult.Distance <= PelvisHitResult.Distance) ? SlopeAngle : -SlopeAngle;

	ImpactDistances = { HeadHitResult.Distance, PelvisHitResult.Distance };

	return SlopeAngle;
}

float UC_PoseColliderHandlerComponent::GetCrawlSlopeDegree(OUT TPair<float, float>& ImpactDistances, const float& HeightOffset, const bool& EnableDebugLine)
{
	FVector HeadStartLocation	= OwnerCharacter->GetActorLocation() + OwnerCharacter->GetActorForwardVector() * 75.f + FVector::UnitZ() * HeightOffset;
	//FVector HeadStartLocation	= OwnerCharacter->GetActorLocation() + OwnerCharacter->GetActorForwardVector() * 75.f;
	FVector PelvisStartLocation = OwnerCharacter->GetActorLocation() + FVector::UnitZ() * HeightOffset;
	//FVector PelvisStartLocation = OwnerCharacter->GetActorLocation() + FVector::UnitZ();

	float SlopeDegree = GetCrawlSlopeDegree(HeadStartLocation, PelvisStartLocation, ImpactDistances, EnableDebugLine);
	ImpactDistances.Key		-= HeightOffset;
	ImpactDistances.Value	-= HeightOffset;

	return SlopeDegree;
}

float UC_PoseColliderHandlerComponent::GetCrawlSlopeDegree
(
	const FVector&				HeadStartLocation,
	const FVector&				PelvisStartLocation,
	OUT TPair<float, float>&	ImpactDistances,
	const bool&					EnableDebugLine
)
{
	float SlopeAngle = GetCrawlSlopeAngle(HeadStartLocation, PelvisStartLocation, ImpactDistances, EnableDebugLine);
	return FMath::RadiansToDegrees(SlopeAngle);
	//FVector HeadDestLocation	= HeadStartLocation   - FVector::UnitZ() * CRAWL_LINETRACE_TEST_DIST;
	//FVector PelvisDestLocation	= PelvisStartLocation - FVector::UnitZ() * CRAWL_LINETRACE_TEST_DIST;

	//FCollisionQueryParams CollisionParams{};
	//CollisionParams.AddIgnoredActor(OwnerCharacter);

	//TArray<AActor*> AttachedActors{};
	//OwnerCharacter->GetAttachedActors(AttachedActors);
	//CollisionParams.AddIgnoredActors(AttachedActors);

	//FHitResult HeadHitResult{};
	//FHitResult PelvisHitResult{};

	//bool HasHeadHit		= GetWorld()->LineTraceSingleByChannel(HeadHitResult, HeadStartLocation, HeadDestLocation, ECC_Visibility, CollisionParams);
	//bool HasPelvisHit	= GetWorld()->LineTraceSingleByChannel(PelvisHitResult, PelvisStartLocation, PelvisDestLocation, ECC_Visibility, CollisionParams);

	//if (EnableDebugLine)
	//{
	//	DrawDebugLine(GetWorld(), HeadStartLocation, HeadHitResult.ImpactPoint, FColor::Red, true);
	//	DrawDebugLine(GetWorld(), PelvisStartLocation, PelvisHitResult.ImpactPoint, FColor::Red, true);
	//}

	//if (!HasHeadHit || !HasPelvisHit) return false;
	//// TODO : Length 체크
	////if (HeadHitResult.ImpactPoint.Length() )

	//// 경사도 체크
	////HeadHitResult.ImpactPoint
	//float A				= (HeadHitResult.ImpactPoint - PelvisHitResult.ImpactPoint).Length();
	//float B				= FMath::Abs(HeadHitResult.ImpactPoint.Z - PelvisHitResult.ImpactPoint.Z);
	//float SlopeDegree	= FMath::RadiansToDegrees(FMath::Asin(B / A));

	//// 부호 지정
	//SlopeDegree = (HeadHitResult.Distance <= PelvisHitResult.Distance) ? SlopeDegree : -SlopeDegree;

	//ImpactDistances = { HeadHitResult.Distance, PelvisHitResult.Distance };

	//return SlopeDegree;
}

bool UC_PoseColliderHandlerComponent::CanCrawlOnSlope(const FVector& HeadStartLocation, const FVector& PelvisStartLocation)
{
	TPair<float, float> ImpactDistances{};

	float SlopeDegree = FMath::Abs(GetCrawlSlopeDegree(HeadStartLocation, PelvisStartLocation, ImpactDistances));

	return FMath::Abs(SlopeDegree)	< CRAWL_DEGREE_LIMIT &&
		ImpactDistances.Key			< CRAWL_GROUND_DIST_LIMIT &&
		ImpactDistances.Value		< CRAWL_GROUND_DIST_LIMIT;
}

bool UC_PoseColliderHandlerComponent::CanCrawlOnSlope(const float& SlopeDegree, TPair<float, float>& ImpactDistances)
{
	return FMath::Abs(SlopeDegree)	< CRAWL_DEGREE_LIMIT &&
		ImpactDistances.Key			< CRAWL_GROUND_DIST_LIMIT &&
		ImpactDistances.Value		< CRAWL_GROUND_DIST_LIMIT;
}

