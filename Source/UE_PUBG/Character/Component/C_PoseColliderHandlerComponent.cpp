// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/C_PoseColliderHandlerComponent.h"

#include "Character/C_Player.h"
#include "Components/CapsuleComponent.h"
#include "Character/Component/C_EquippedComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HUD/C_HUDWidget.h"
#include "HUD/C_InformWidget.h"
#include "Singleton/C_GameSceneManager.h"
#include "Utility/C_Util.h"

const ECollisionChannel UC_PoseColliderHandlerComponent::PoseCheckerChannel = ECC_GameTraceChannel1;

const TMap<EPoseState, TPair<float, float>> UC_PoseColliderHandlerComponent::POSE_BY_ROOTCOLLIDER_HEIGHT_RADIUS =
{
	{EPoseState::STAND,		{88.f, 34.f}},
	{EPoseState::CROUCH,	{67.f, 34.f}},
	{EPoseState::CRAWL,		{20.f, 20.f}}
	//{EPoseState::CRAWL,		{1.f, 1.f}}
};

const TMap<EPoseState, float> UC_PoseColliderHandlerComponent::POSE_BY_MESH_Z_POS =
{
	{EPoseState::STAND,		-90.f},
	{EPoseState::CROUCH,	-64.f},
	{EPoseState::CRAWL,		-20.f}
};

const float UC_PoseColliderHandlerComponent::SWEEP_SPHERE_RAD				= 34.f;
const float UC_PoseColliderHandlerComponent::CROUCH_TO_STAND_SWEEP_DIST		= (88.f - 67.f) * 2.f;
const float UC_PoseColliderHandlerComponent::CRAWL_TO_STAND_SWEEP_DIST		= 105.f;
const float UC_PoseColliderHandlerComponent::CRAWL_TO_CROUCH_SWEEP_DIST		= 60.f;
const float UC_PoseColliderHandlerComponent::CRAWL_LINETRACE_TEST_DIST		= 500.f;
const float UC_PoseColliderHandlerComponent::CRAWL_DEGREE_LIMIT				= 35.f; // 기어갈 수 없는 경사도 Limit
const float UC_PoseColliderHandlerComponent::CRAWL_GROUND_DIST_LIMIT		= 50.f;

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
	
	/*UC_Util::Print(OwnerCharacter->GetCapsuleComponent()->GetComponentLocation().Z, FColor::Red);
	UC_Util::Print(OwnerCharacter->GetMesh()->GetComponentLocation().Z, FColor::Blue);
	UC_Util::Print(OwnerCharacter->GetMesh()->GetRelativeLocation().Z, FColor::Yellow);*/

	/*FVector HipSocketLocation = OwnerCharacter->GetMesh()->GetSocketLocation("Hips");
	UC_Util::Print("HipSocketLocation : " + FString::FromInt(HipSocketLocation.Z), FColor::Red);
	UC_Util::Print("Character Location : " + FString::FromInt(OwnerCharacter->GetActorLocation().Z), FColor::Blue);*/
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

void UC_PoseColliderHandlerComponent::SetColliderBySwimmingMovingState(const bool& IsMoving)
{
	const TPair<float, float> PrevHeightRadius = RootColliderHeightRadiusLerpDest;
	
	RootColliderHeightRadiusLerpDest = POSE_BY_ROOTCOLLIDER_HEIGHT_RADIUS[IsMoving ? EPoseState::CRAWL : EPoseState::STAND];
	if (PrevHeightRadius == RootColliderHeightRadiusLerpDest) return;
	
	MeshZPosLerpDest = POSE_BY_MESH_Z_POS[EPoseState::STAND];
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
		OwnerCharacter->GetEquippedComponent()->AddAttachedPartsActorsToIgnoreActors(CollisionParams);

		FHitResult HitResult{};
		
		bool HasHit = GetWorld()->SweepSingleByChannel
		(
			HitResult,
			StartLocation,
			DestLocation,
			FQuat::Identity,
			PoseCheckerChannel,
			FCollisionShape::MakeSphere(SWEEP_SPHERE_RAD),
			CollisionParams
		);
		
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
			PoseCheckerChannel,
			FCollisionShape::MakeSphere(SWEEP_SPHERE_RAD),
			CollisionParams
		);

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
	if (FMath::Abs(Height - RootColliderHeightRadiusLerpDest.Key)	 < LERP_DIFF_EPSILON &&
		FMath::Abs(Radius - RootColliderHeightRadiusLerpDest.Value)  < LERP_DIFF_EPSILON &&
		FMath::Abs(MeshZPos - MeshZPosLerpDest)						 < LERP_DIFF_EPSILON)
	{
		OwnerCharacter->GetCapsuleComponent()->SetCapsuleSize
		(
			RootColliderHeightRadiusLerpDest.Value,
			RootColliderHeightRadiusLerpDest.Key
		);

		OwnerCharacter->GetMesh()->SetRelativeLocation(FVector(MeshLocation.X, MeshLocation.Y, MeshZPosLerpDest));

		OwnerCharacter->SetCanMove(true);
		PoseBySizeLerpFlag = false;
		
		if (OwnerCharacter->GetPoseState() == EPoseState::CRAWL)
		{
			OwnerCharacter->GetCapsuleComponent()->SetSimulatePhysics(true);
			CrawlCapsuleComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			OwnerCharacter->GetCapsuleComponent()->SetSimulatePhysics(false);
		}
		else
		{
			if (OwnerCharacter->GetSwimmingComponent()->IsSwimming()) return;
			// 5 10
			CrawlCapsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			// 자세 전환 이후 바닥면에 끼이는 상황 방지
			// 자세별 Hips의 위치와 Character의 상대적 위치를 보아, ActorLocation을 조정해야하는 상황이면 조정하여 골반 위치로 ActorLocation을 동기화 처리시킴
			static const TMap<EPoseState, TPair<float, float>> PoseByHipActorDiffLimitAndAdjustValue =
			{
				{EPoseState::STAND,  {20.f, 15.f}},
				{EPoseState::CROUCH, {25.f, 20.f}}
			};

			const float HipLocation   = OwnerCharacter->GetMesh()->GetSocketLocation("Hips").Z;  
			const float HipActorZDiff = OwnerCharacter->GetActorLocation().Z - HipLocation;
			
			if (HipActorZDiff < PoseByHipActorDiffLimitAndAdjustValue[OwnerCharacter->GetPoseState()].Key)
			{
				UC_Util::Print("Adjusting Character Location", FColor::Red, 10.f);
				
				FVector NewLocation = OwnerCharacter->GetActorLocation();
				NewLocation.Z       = HipLocation + PoseByHipActorDiffLimitAndAdjustValue[OwnerCharacter->GetPoseState()].Value;
				
				OwnerCharacter->SetActorLocation(NewLocation);
			}
		}
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
	if (OwnerCharacter->GetPoseState() != EPoseState::CRAWL) return;

	static const float HEIGHT_OFFSET	= 50.f;
	TPair<float, float> ImpactDistances{};

	CrawlSlopeAngle		= GetCrawlSlopeAngle(ImpactDistances, HEIGHT_OFFSET);
	float SlopeDegree	= FMath::RadiansToDegrees(CrawlSlopeAngle);

	//UC_Util::Print(SlopeDegree);
	if (!CanCrawlOnSlope(SlopeDegree, ImpactDistances))
	{
		if (AC_Player* Player = Cast<AC_Player>(OwnerCharacter)) Player->GetHUDWidget()->GetInformWidget()->AddPlayerWarningLog("CRAWL BLOCKED!");

		CrawlSlopeAngle = 0.f;
		
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

	bool HasHeadHit		= GetWorld()->LineTraceSingleByChannel(HeadHitResult, HeadStartLocation, HeadDestLocation, PoseCheckerChannel, CollisionParams);
	bool HasPelvisHit	= GetWorld()->LineTraceSingleByChannel(PelvisHitResult, PelvisStartLocation, PelvisDestLocation, PoseCheckerChannel, CollisionParams);

	/*bool HasHeadHit = GetWorld()->SweepSingleByChannel(HeadHitResult, HeadStartLocation, HeadDestLocation, FQuat::Identity,
		PoseCheckerChannel, FCollisionShape::MakeSphere(20.f), CollisionParams);

	bool HasPelvisHit = GetWorld()->SweepSingleByChannel(PelvisHitResult, PelvisStartLocation, PelvisDestLocation, FQuat::Identity,
		PoseCheckerChannel, FCollisionShape::MakeSphere(20.f), CollisionParams);*/
	

	if (EnableDebugLine)
	{
		/*DrawDebugSphere(GetWorld(), HeadStartLocation, 20.f, 12, FColor::Red, false, 0.5f);
		DrawDebugSphere(GetWorld(), HeadDestLocation, 20.f, 12, FColor::Red, false, 0.5f);
		DrawDebugLine(GetWorld(), HeadStartLocation, HeadDestLocation, FColor::Red, false, 0.5f);

		DrawDebugSphere(GetWorld(), PelvisStartLocation, 20.f, 12, FColor::Red, false, 0.5f);
		DrawDebugSphere(GetWorld(), PelvisDestLocation, 20.f, 12, FColor::Red, false, 0.5f);
		DrawDebugLine(GetWorld(), PelvisStartLocation, PelvisDestLocation, FColor::Red, false, 0.5f);*/
		DrawDebugLine(GetWorld(), HeadStartLocation, HeadHitResult.ImpactPoint, FColor::Red, false, 0.5f);
		DrawDebugLine(GetWorld(), PelvisStartLocation, PelvisHitResult.ImpactPoint, FColor::Red, false, 0.5f);
	}

	if (!HasHeadHit || !HasPelvisHit) return 0.f;
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
}

bool UC_PoseColliderHandlerComponent::CanCrawlOnSlope(const FVector& HeadStartLocation, const FVector& PelvisStartLocation)
{
	TPair<float, float> ImpactDistances{};

	float SlopeDegree = FMath::Abs(GetCrawlSlopeDegree(HeadStartLocation, PelvisStartLocation, ImpactDistances));

	return FMath::Abs(SlopeDegree)	< CRAWL_DEGREE_LIMIT &&
		ImpactDistances.Key			< CRAWL_GROUND_DIST_LIMIT &&
		ImpactDistances.Value		< CRAWL_GROUND_DIST_LIMIT;
}

bool UC_PoseColliderHandlerComponent::CanCrawlOnSlope(const float& SlopeDegree, const TPair<float, float>& ImpactDistances)
{
	return FMath::Abs(SlopeDegree)	< CRAWL_DEGREE_LIMIT &&
		ImpactDistances.Key			< CRAWL_GROUND_DIST_LIMIT &&
		ImpactDistances.Value		< CRAWL_GROUND_DIST_LIMIT;
}



