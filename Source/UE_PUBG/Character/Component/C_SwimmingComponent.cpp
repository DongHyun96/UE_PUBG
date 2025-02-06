// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/C_SwimmingComponent.h"
#include "Character/Component/C_StatComponent.h"
#include "Components/ShapeComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"
#include "Character/Component/C_EquippedComponent.h"
#include "Character/Component/C_PoseColliderHandlerComponent.h"
#include "Character/Component/C_SkyDivingComponent.h"

#include "Utility/C_Util.h"

//#include "WaterBodyActor.h"
#include "GameFramework/PhysicsVolume.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "HUD/C_HUDWidget.h"
#include "HUD/C_OxygenWidget.h"

const float UC_SwimmingComponent::CAN_WALK_DEPTH_LIMIT = 100.f;


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
	UpdateOxygenAmount(DeltaTime);

	/*switch (SwimmingState)
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
	}*/
}

void UC_SwimmingComponent::HandlePlayerMovement(const FVector2D& MovementVector)
{
	bIsHandlingPlayerMovementOnCurrentTick = true;
	OwnerCharacter->GetPoseColliderHandlerComponent()->SetColliderBySwimmingMovingState(true);

	OwnerPlayer->GetCharacterMovement()->bUseControllerDesiredRotation	= false;
	OwnerPlayer->GetCharacterMovement()->bOrientRotationToMovement		= true;
	OwnerPlayer->bUseControllerRotationYaw								= false;

	//if (OwnerPlayer->GetIsHoldDirection() || OwnerPlayer->GetIsAltPressed())
	//	OwnerPlayer->bUseControllerRotationYaw = false;

	if (OwnerPlayer->GetIsHoldDirection() || OwnerPlayer->GetIsAltPressed())
		OwnerPlayer->GetCharacterMovement()->bOrientRotationToMovement = false;

	// Update Max walk speed
	OwnerPlayer->UpdateMaxWalkSpeed(MovementVector);

	if (OwnerPlayer->Controller)
	{
		FRotator Rotation = (OwnerPlayer->GetIsHoldDirection() || OwnerPlayer->GetIsAltPressed()) ?
							FRotator(0.f, OwnerPlayer->GetActorRotation().Yaw, 0.f) : OwnerPlayer->GetController()->GetControlRotation();

		if (SwimmingState == ESwimmingState::SWIMMING_SURFACE)
		{
			if ((0.f <= Rotation.Pitch && Rotation.Pitch < 90.f) || 275.f < Rotation.Pitch) // 위 및 아래로 가는 회전 제거
				Rotation = FRotator(0.f, Rotation.Yaw, Rotation.Roll);
			else if (Rotation.Pitch < 275.f) // SwimmingSurface 해제 임계치
				SwimmingState = ESwimmingState::SWIMMING_UNDER;
		}


		const FVector ForwardDirection = FRotationMatrix(Rotation).GetUnitAxis(EAxis::X);
		const FVector   RightDirection = FRotationMatrix(Rotation).GetUnitAxis(EAxis::Y);

		OwnerPlayer->AddMovementInput(ForwardDirection, MovementVector.X);
		OwnerPlayer->AddMovementInput(RightDirection, MovementVector.Y);

		OwnerPlayer->SetNextSpeed(OwnerPlayer->GetCharacterMovement()->MaxWalkSpeed); // AnimCharacter에서 Speed Lerp할 값 setting
	}
}

void UC_SwimmingComponent::OnSwimmingCKey()
{
	SwimmingState = ESwimmingState::SWIMMING_UNDER;

	if (!bIsHandlingPlayerMovementOnCurrentTick)
	{
		OwnerPlayer->GetCharacterMovement()->bUseControllerDesiredRotation	= true;
		OwnerPlayer->GetCharacterMovement()->bOrientRotationToMovement		= true;
		OwnerPlayer->bUseControllerRotationYaw								= true;
		//OwnerPlayer->GetCharacterMovement()->bOrientRotationToMovement = false;
	}

	OwnerPlayer->UpdateMaxWalkSpeed(FVector2D::Zero()); // Swimming일 때 방향은 상관 없음

	if (OwnerPlayer->Controller)
	{
		static const float DOWNWARD_PITCH = 270.f;

		FRotator Rotation	= OwnerPlayer->GetController()->GetControlRotation();
		Rotation.Pitch		= DOWNWARD_PITCH;
		Rotation.Roll		= 0.f;
		
		const FVector ForwardDirection = FRotationMatrix(Rotation).GetUnitAxis(EAxis::X);
		OwnerPlayer->AddMovementInput(ForwardDirection, 1.f);
		OwnerPlayer->SetNextSpeed(OwnerPlayer->GetCharacterMovement()->MaxWalkSpeed);
	}
}

void UC_SwimmingComponent::OnSwimmingSpaceBarKey()
{
	static const float UPWARD_PITCH = 89.f;

	if (!bIsHandlingPlayerMovementOnCurrentTick)
	{
		OwnerPlayer->GetCharacterMovement()->bUseControllerDesiredRotation	= true;
		OwnerPlayer->GetCharacterMovement()->bOrientRotationToMovement		= true;
		OwnerPlayer->bUseControllerRotationYaw								= true;
		//OwnerPlayer->GetCharacterMovement()->bOrientRotationToMovement = false;
	}

	OwnerPlayer->UpdateMaxWalkSpeed(FVector2D::Zero()); // Swimming일 때 방향은 상관 없음

	if (OwnerPlayer->Controller)
	{
		FRotator Rotation	= OwnerPlayer->GetController()->GetControlRotation();
		Rotation.Pitch		= UPWARD_PITCH;
		Rotation.Roll		= 0.f;
		
		const FVector ForwardDirection = FRotationMatrix(Rotation).GetUnitAxis(EAxis::X);
		OwnerPlayer->AddMovementInput(ForwardDirection, 1.f);
		OwnerPlayer->SetNextSpeed(OwnerPlayer->GetCharacterMovement()->MaxWalkSpeed);
	}
}

void UC_SwimmingComponent::OnSwimmingMoveEnd()
{
	bIsHandlingPlayerMovementOnCurrentTick = false;
	OwnerCharacter->GetPoseColliderHandlerComponent()->SetColliderByPoseState(EPoseState::STAND);
	OwnerCharacter->GetPoseColliderHandlerComponent()->SetColliderBySwimmingMovingState(false);
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

	FVector CharacterLocation = OwnerCharacter->GetActorLocation();

	float WaterDepth = GetWaterDepth(CharacterLocation);

	// 수영 가능한 깊이인지 check
	if (WaterDepth <= CAN_WALK_DEPTH_LIMIT)
	{
		StopSwimming();
		return;
	}

	// Surface or under check
	const float SURFACE_SWIM_DELTA_LIMIT = 20.f;

	float CharacterDepth = GetCharacterDepth();

	// 수면 위로 나감 방지
	if (CharacterDepth > WaterDepth)
	{
		SwimmingState = ESwimmingState::SWIMMING_SURFACE;

		float DetectionColliderZ = WaterDetectionCollider->GetComponentLocation().Z;

		CharacterLocation -= FVector::UnitZ() * (DetectionColliderZ - EnteredWaterZ);

		OwnerCharacter->SetActorLocation(CharacterLocation);

		return;
	}
	
	SwimmingState = (WaterDepth - CharacterDepth < SURFACE_SWIM_DELTA_LIMIT) ? 
					 ESwimmingState::SWIMMING_SURFACE : ESwimmingState::SWIMMING_UNDER;

	// Surface에서 수영중이고 멈춰있을 때 처리
	if (SwimmingState == ESwimmingState::SWIMMING_SURFACE && OwnerCharacter->GetNextSpeed() == 0.f)
	{
		float DetectionColliderZ = WaterDetectionCollider->GetComponentLocation().Z;

		CharacterLocation += (DetectionColliderZ < EnteredWaterZ) ? 
							  FVector::UnitZ() * (EnteredWaterZ - DetectionColliderZ) :
							 -FVector::UnitZ() * (DetectionColliderZ - EnteredWaterZ);

		OwnerCharacter->SetActorLocation(CharacterLocation);
	}
}

void UC_SwimmingComponent::UpdateOxygenAmount(const float& DeltaTime)
{
	static const float OXYGEN_INCREASE_PER_SEC = 10.f;  // 0 -> 100 회복시간 총 15초
	static const float OXYGEN_DECREASE_PER_SEC = 6.66f; // 100 -> 0 피해시간 총 20초

	switch (SwimmingState)
	{
	case ESwimmingState::ON_GROUND: case ESwimmingState::SWIMMING_SURFACE: // 숨 회복
		if (OwnerCharacter->GetStatComponent()->GetCurOxygen() >= 100.f) return;
		OwnerCharacter->GetStatComponent()->AddOxygen(OXYGEN_INCREASE_PER_SEC * DeltaTime);
		return;
	case ESwimmingState::SWIMMING_UNDER: // 숨 깎기
		if (OwnerCharacter->GetStatComponent()->GetCurOxygen() <= 0.f) return;
		OwnerCharacter->GetStatComponent()->AddOxygen(-OXYGEN_DECREASE_PER_SEC * DeltaTime);
		return;
	default:
		return;
	}
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
	UC_Util::Print(OtherActor->GetName(), FColor::Cyan, 10.f);


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

	UC_Util::Print("Start Swimming", FColor::Red, 10.f);

	// SkyDiving Parachuting state 도중 물로 착지했을 때의 예외처리
	if (OwnerCharacter->GetMainState() == EMainState::SKYDIVING)
		OwnerCharacter->GetSkyDivingComponent()->OnCharacterLandedOnWater();

	OwnerCharacter->GetPhysicsVolume()->bWaterVolume = true;
	OwnerCharacter->SetCanMove(true);
	OwnerCharacter->SetPoseState(OwnerCharacter->GetPoseState(), EPoseState::STAND);
	OwnerCharacter->GetCharacterMovement()->Velocity.Z = 0.f;
	//OwnerCharacter->LaunchCharacter(OwnerCharacter->GetActorUpVector() * 0.0005f, false, false);
	OwnerCharacter->GetEquippedComponent()->ChangeCurWeapon(EWeaponSlot::NONE);

	SwimmingState = ESwimmingState::SWIMMING_SURFACE;
	OwnerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Swimming);

	// Player의 HUD 업데이트
	if (OwnerPlayer) OwnerPlayer->GetHUDWidget()->GetOxygenWidget()->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	// Pawn과의 충돌 block 끄기 (Pawn과의 충돌처리를 하면서 물 밖으로 나가버릴 수 있는 버그가 있음)
	OwnerCharacter->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	OwnerCharacter->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Ignore);
	OwnerCharacter->GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	OwnerCharacter->GetMesh()->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Ignore);
}

void UC_SwimmingComponent::StopSwimming()
{
	OwnerCharacter->GetPhysicsVolume()->bWaterVolume = false;
	SwimmingState = ESwimmingState::ON_GROUND;

	// PoseCollider 원형으로 다시 재배치
	OwnerCharacter->GetPoseColliderHandlerComponent()->SetColliderBySwimmingMovingState(false);

	if (OwnerPlayer) OwnerPlayer->GetHUDWidget()->GetOxygenWidget()->SetVisibility(ESlateVisibility::Hidden);

	// Pawn과의 충돌 다시 켜주기
	OwnerCharacter->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	OwnerCharacter->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Block);
	OwnerCharacter->GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	OwnerCharacter->GetMesh()->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Block);
}

float UC_SwimmingComponent::GetWaterDepth(const FVector& Position)
{
	FCollisionQueryParams CollisionParams{};
	CollisionParams.AddIgnoredActor(OwnerCharacter);

	TArray<AActor*> AttachedActors{};
	OwnerCharacter->GetAttachedActors(AttachedActors);
	CollisionParams.AddIgnoredActors(AttachedActors);

	// ObjectQueryParams 설정 (Pawn 채널 무시)
	FCollisionObjectQueryParams ObjectQueryParams{};
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);

	// OwnerCharacter의 현재 위치의 수심 체크하기
	FHitResult HitResult{};
	FVector CharacterLocation	= OwnerCharacter->GetActorLocation();
	FVector StartLocation		= FVector(CharacterLocation.X, CharacterLocation.Y, EnteredWaterZ);
	FVector DestLocation		= FVector(CharacterLocation.X, CharacterLocation.Y, EnteredWaterZ - 10000); // 100m 밑까지 수심 체크

	//bool HasHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, DestLocation, ECC_WorldStatic, CollisionParams);

	bool HasHit = GetWorld()->LineTraceSingleByObjectType
	(
		HitResult,
		StartLocation,
		DestLocation,
		ObjectQueryParams,
		CollisionParams
	);

	return (!HasHit) ? 0.f : HitResult.Distance;
}

float UC_SwimmingComponent::GetCharacterDepth()
{
	FCollisionQueryParams CollisionParams{};
	CollisionParams.AddIgnoredActor(OwnerCharacter);

	TArray<AActor*> AttachedActors{};
	OwnerCharacter->GetAttachedActors(AttachedActors);
	CollisionParams.AddIgnoredActors(AttachedActors);

	// ObjectQueryParams 설정
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);

	// OwnerCharacter의 현재 위치의 수심 체크하기
	FHitResult HitResult{};
	FVector StartLocation		= WaterDetectionCollider->GetComponentLocation();
	FVector DestLocation		= FVector(StartLocation.X, StartLocation.Y, StartLocation.Z - 10000.f); // 100m 밑까지 수심 체크

	//bool HasHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, DestLocation, ECC_WorldStatic, CollisionParams);

	bool HasHit = GetWorld()->LineTraceSingleByObjectType
	(
		HitResult,
		StartLocation,
		DestLocation,
		ObjectQueryParams,
		CollisionParams
	);

	return (!HasHit) ? 0.f : HitResult.Distance;

}



