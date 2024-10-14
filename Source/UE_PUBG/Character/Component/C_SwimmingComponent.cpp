// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/C_SwimmingComponent.h"

#include "Components/ShapeComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"

#include "Utility/C_Util.h"

//#include "WaterBodyActor.h"
#include "GameFramework/PhysicsVolume.h"
#include "GameFramework/CharacterMovementComponent.h"

UC_SwimmingComponent::UC_SwimmingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	WaterDetectionCollider = CreateDefaultSubobject<UCapsuleComponent>("WaterDetectionCollider");
	//WaterDetectionCollider->SetupAttachment(OwnerCharacter->GetRootComponent());
}


void UC_SwimmingComponent::BeginPlay()
{
	Super::BeginPlay();

	WaterDetectionCollider->OnComponentBeginOverlap.AddDynamic(this, &UC_SwimmingComponent::OnWaterDetectionColliderBeginOverlap);
	WaterDetectionCollider->OnComponentEndOverlap.AddDynamic  (this, &UC_SwimmingComponent::OnWaterDetectionColliderEndOverlap);

	const FName ATTACH_SOCKET_NAME = "SwimmingColliderSocket";
	
	WaterDetectionCollider->AttachToComponent
	(
		OwnerCharacter->GetMesh(),
		FAttachmentTransformRules(EAttachmentRule::KeepRelative, true),
		ATTACH_SOCKET_NAME
	);

	// Player로 Casting 시도
	OwnerPlayer = Cast<AC_Player>(OwnerCharacter);
}


void UC_SwimmingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	HandleSwimmingState();

	switch (SwimmingState)
	{
	case ESwimmingState::ON_GROUND:			UC_Util::Print("OnGround");
		break;
	case ESwimmingState::SWIMMING_SURFACE:	UC_Util::Print("Surface");
		break;
	case ESwimmingState::SWIMMING_UNDER:	UC_Util::Print("Under");
		break;
	case ESwimmingState::MAX:
		break;
	default:
		break;
	}
}

void UC_SwimmingComponent::HandlePlayerMovement(const FVector2D& MovementVector)
{
	OwnerPlayer->GetCharacterMovement()->bUseControllerDesiredRotation	= false;
	OwnerPlayer->GetCharacterMovement()->bOrientRotationToMovement		= true;

	// Update Max walk speed
	OwnerPlayer->UpdateMaxWalkSpeed(MovementVector);

	if (OwnerPlayer->Controller)
	{
		FRotator Rotation = OwnerPlayer->GetController()->GetControlRotation();

		if (SwimmingState == ESwimmingState::SWIMMING_SURFACE)
		{
			if (Rotation.Pitch < 90.f) // 위로 가는 회전 제거
				Rotation = FRotator(0.f, Rotation.Yaw, Rotation.Roll);
			else if (Rotation.Pitch < 290.f) // SwimmingSurface 해제 임계치
				SwimmingState = ESwimmingState::SWIMMING_UNDER;
		}

		const FVector ForwardDirection = FRotationMatrix(Rotation).GetUnitAxis(EAxis::X);
		const FVector   RightDirection = FRotationMatrix(Rotation).GetUnitAxis(EAxis::Y);

		OwnerPlayer->AddMovementInput(ForwardDirection, MovementVector.X);
		OwnerPlayer->AddMovementInput(RightDirection, MovementVector.Y);

		OwnerPlayer->SetNextSpeed(OwnerPlayer->GetCharacterMovement()->MaxWalkSpeed); // AnimCharacter에서 Speed Lerp할 값 setting
	}

	// Surface or Under check
	//const float SURFACE_SWIM_DELTA_LIMIT = 20.f;
	//
	//if (GetWaterDepth(OwnerCharacter->GetActorLocation()) - GetCharacterDepth() < SURFACE_SWIM_DELTA_LIMIT)
	//	 SwimmingState = ESwimmingState::SWIMMING_SURFACE;
	//else SwimmingState = ESwimmingState::SWIMMING_UNDER;

}

void UC_SwimmingComponent::SetOwnerCharacter(AC_BasicCharacter* InOwnerCharacter)
{
	OwnerCharacter = InOwnerCharacter;
	//WaterDetectionCollider->AttachToComponent
	//(
	//	OwnerCharacter->GetRootComponent(),
	//	FAttachmentTransformRules::KeepRelativeTransform
	//);
}

void UC_SwimmingComponent::HandleSwimmingState()
{
	if (SwimmingState == ESwimmingState::ON_GROUND) return;

	float WaterDepth = GetWaterDepth(OwnerCharacter->GetActorLocation());

	// 수영 가능한 깊이인지 check
	if ( WaterDepth <= CAN_WALK_DEPTH_LIMIT)
	{
		StopSwimming();
		return;
	}

	// Surface or under check
	const float SURFACE_SWIM_DELTA_LIMIT = 20.f;
	
	SwimmingState = (WaterDepth - GetCharacterDepth() < SURFACE_SWIM_DELTA_LIMIT) ? 
					 ESwimmingState::SWIMMING_SURFACE : ESwimmingState::SWIMMING_UNDER;

	// // 표면에서 수영중인지 아니면 Under인지는 Movement에서 역으로 체크
}

void UC_SwimmingComponent::OnWaterDetectionColliderBeginOverlap
(
	UPrimitiveComponent*	OverlappedComponent,
	AActor*					OtherActor,
	UPrimitiveComponent*	OtherComp,
	int32					OtherBodyIndex, 
	bool					bFromSweep,
	const FHitResult&		SweepResult
)
{
	UC_Util::Print("Swimming Collider Begin Overlap");

	// PostProcessVolume의 위치와 크기 정보를 가져옴
	FVector PostProcessLocation = OtherActor->GetActorLocation();
	FBoxSphereBounds Bounds = OtherActor->GetComponentsBoundingBox();

	// 윗면 Z 좌표 계산 (중심 Z 좌표 + 절반 높이)
	EnteredWaterZ = PostProcessLocation.Z + Bounds.BoxExtent.Z;
	//EnteredWaterZ	= WaterDetectionCollider->GetComponentLocation().Z - WaterDetectionCollider->GetScaledCapsuleHalfHeight();
	
	FVector Center  = FVector(WaterDetectionCollider->GetComponentLocation().X, WaterDetectionCollider->GetComponentLocation().Y, EnteredWaterZ);
	DrawDebugSphere(GetWorld(), Center, 5.f, 10, FColor::Red, true);

	StartSwimming();
	//if (GetWaterDepth(OwnerCharacter->GetActorLocation()) < CAN_WALK_DEPTH_LIMIT) return;
}

void UC_SwimmingComponent::OnWaterDetectionColliderEndOverlap
(
	UPrimitiveComponent*	OverlappedComponent, 
	AActor*					OtherActor,
	UPrimitiveComponent*	OtherComp,
	int32					OtherBodyIndex
)
{
	UC_Util::Print("Swimming Collider End Overlap");
}

void UC_SwimmingComponent::StartSwimming()
{
	// 이미 수영하는 중
	if (SwimmingState != ESwimmingState::ON_GROUND) return;

	OwnerCharacter->GetPhysicsVolume()->bWaterVolume = true;
	OwnerCharacter->SetCanMove(true);
	OwnerCharacter->SetPoseState(OwnerCharacter->GetPoseState(), EPoseState::STAND);
	OwnerCharacter->LaunchCharacter(OwnerCharacter->GetActorUpVector() * 0.005f, false, false);

	SwimmingState = ESwimmingState::SWIMMING_SURFACE;
}

void UC_SwimmingComponent::StopSwimming()
{
	OwnerCharacter->GetPhysicsVolume()->bWaterVolume = false;
	SwimmingState = ESwimmingState::ON_GROUND;
}

float UC_SwimmingComponent::GetWaterDepth(const FVector& Position)
{
	FCollisionQueryParams CollisionParams{};
	CollisionParams.AddIgnoredActor(OwnerCharacter);

	TArray<AActor*> AttachedActors{};
	OwnerCharacter->GetAttachedActors(AttachedActors);
	CollisionParams.AddIgnoredActors(AttachedActors);

	// OwnerCharacter의 현재 위치의 수심 체크하기
	FHitResult HitResult{};
	FVector CharacterLocation	= OwnerCharacter->GetActorLocation();
	FVector StartLocation		= FVector(CharacterLocation.X, CharacterLocation.Y, EnteredWaterZ);
	FVector DestLocation		= FVector(CharacterLocation.X, CharacterLocation.Y, EnteredWaterZ - 10000); // 100m 밑까지 수심 체크

	bool HasHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, DestLocation, ECC_Visibility, CollisionParams);

	return (!HasHit) ? 0.f : HitResult.Distance;
}

float UC_SwimmingComponent::GetCharacterDepth()
{
	FCollisionQueryParams CollisionParams{};
	CollisionParams.AddIgnoredActor(OwnerCharacter);

	TArray<AActor*> AttachedActors{};
	OwnerCharacter->GetAttachedActors(AttachedActors);
	CollisionParams.AddIgnoredActors(AttachedActors);

	// OwnerCharacter의 현재 위치의 수심 체크하기
	FHitResult HitResult{};
	FVector StartLocation		= WaterDetectionCollider->GetComponentLocation();
	FVector DestLocation		= FVector(StartLocation.X, StartLocation.Y, StartLocation.Z - 10000.f); // 100m 밑까지 수심 체크

	bool HasHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, DestLocation, ECC_Visibility, CollisionParams);

	return (!HasHit) ? 0.f : HitResult.Distance;

}

