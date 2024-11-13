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

	// Player�� Casting �õ�
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
			if ((0.f <= Rotation.Pitch && Rotation.Pitch < 90.f) || 275.f < Rotation.Pitch) // �� �� �Ʒ��� ���� ȸ�� ����
				Rotation = FRotator(0.f, Rotation.Yaw, Rotation.Roll);
			else if (Rotation.Pitch < 275.f) // SwimmingSurface ���� �Ӱ�ġ
				SwimmingState = ESwimmingState::SWIMMING_UNDER;
		}


		const FVector ForwardDirection = FRotationMatrix(Rotation).GetUnitAxis(EAxis::X);
		const FVector   RightDirection = FRotationMatrix(Rotation).GetUnitAxis(EAxis::Y);

		OwnerPlayer->AddMovementInput(ForwardDirection, MovementVector.X);
		OwnerPlayer->AddMovementInput(RightDirection, MovementVector.Y);

		OwnerPlayer->SetNextSpeed(OwnerPlayer->GetCharacterMovement()->MaxWalkSpeed); // AnimCharacter���� Speed Lerp�� �� setting
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

	OwnerPlayer->UpdateMaxWalkSpeed(FVector2D::Zero()); // Swimming�� �� ������ ��� ����

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

	OwnerPlayer->UpdateMaxWalkSpeed(FVector2D::Zero()); // Swimming�� �� ������ ��� ����

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

	// ���� ������ �������� check
	if (WaterDepth <= CAN_WALK_DEPTH_LIMIT)
	{
		StopSwimming();
		return;
	}

	// Surface or under check
	const float SURFACE_SWIM_DELTA_LIMIT = 20.f;

	float CharacterDepth = GetCharacterDepth();

	// ���� ���� ���� ����
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

	// Surface���� �������̰� �������� �� ó��
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
	static const float OXYGEN_INCREASE_PER_SEC = 10.f;  // 0 -> 100 ȸ���ð� �� 15��
	static const float OXYGEN_DECREASE_PER_SEC = 6.66f; // 100 -> 0 ���ؽð� �� 20��

	switch (SwimmingState)
	{
	case ESwimmingState::ON_GROUND: case ESwimmingState::SWIMMING_SURFACE: // �� ȸ��
		if (OwnerCharacter->GetStatComponent()->GetCurOxygen() >= 100.f) return;
		OwnerCharacter->GetStatComponent()->AddOxygen(OXYGEN_INCREASE_PER_SEC * DeltaTime);
		return;
	case ESwimmingState::SWIMMING_UNDER: // �� ���
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


	// PostProcessVolume�� ��ġ�� ũ�� ������ ������
	FVector PostProcessLocation = OtherActor->GetActorLocation();
	FBoxSphereBounds Bounds = OtherActor->GetComponentsBoundingBox();

	// ���� Z ��ǥ ��� (�߽� Z ��ǥ + ���� ����)
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
	// �̹� �����ϴ� ��
	if (SwimmingState != ESwimmingState::ON_GROUND) return;

	UC_Util::Print("Start Swimming", FColor::Red, 10.f);

	// SkyDiving Parachuting state ���� ���� �������� ���� ����ó��
	OwnerCharacter->GetSkyDivingComponent()->OnCharacterLandedOnWater();

	OwnerCharacter->GetPhysicsVolume()->bWaterVolume = true;
	OwnerCharacter->SetCanMove(true);
	OwnerCharacter->SetPoseState(OwnerCharacter->GetPoseState(), EPoseState::STAND);
	OwnerCharacter->GetCharacterMovement()->Velocity.Z = 0.f;
	//OwnerCharacter->LaunchCharacter(OwnerCharacter->GetActorUpVector() * 0.0005f, false, false);
	OwnerCharacter->GetEquippedComponent()->ChangeCurWeapon(EWeaponSlot::NONE);

	SwimmingState = ESwimmingState::SWIMMING_SURFACE;
	OwnerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Swimming);

	// Player�� HUD ������Ʈ
	if (OwnerPlayer) OwnerPlayer->GetHUDWidget()->GetOxygenWidget()->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UC_SwimmingComponent::StopSwimming()
{
	OwnerCharacter->GetPhysicsVolume()->bWaterVolume = false;
	SwimmingState = ESwimmingState::ON_GROUND;

	// PoseCollider �������� �ٽ� ���ġ
	OwnerCharacter->GetPoseColliderHandlerComponent()->SetColliderBySwimmingMovingState(false);

	if (OwnerPlayer) OwnerPlayer->GetHUDWidget()->GetOxygenWidget()->SetVisibility(ESlateVisibility::Hidden);
}

float UC_SwimmingComponent::GetWaterDepth(const FVector& Position)
{
	FCollisionQueryParams CollisionParams{};
	CollisionParams.AddIgnoredActor(OwnerCharacter);

	TArray<AActor*> AttachedActors{};
	OwnerCharacter->GetAttachedActors(AttachedActors);
	CollisionParams.AddIgnoredActors(AttachedActors);

	// OwnerCharacter�� ���� ��ġ�� ���� üũ�ϱ�
	FHitResult HitResult{};
	FVector CharacterLocation	= OwnerCharacter->GetActorLocation();
	FVector StartLocation		= FVector(CharacterLocation.X, CharacterLocation.Y, EnteredWaterZ);
	FVector DestLocation		= FVector(CharacterLocation.X, CharacterLocation.Y, EnteredWaterZ - 10000); // 100m �ر��� ���� üũ

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

	// OwnerCharacter�� ���� ��ġ�� ���� üũ�ϱ�
	FHitResult HitResult{};
	FVector StartLocation		= WaterDetectionCollider->GetComponentLocation();
	FVector DestLocation		= FVector(StartLocation.X, StartLocation.Y, StartLocation.Z - 10000.f); // 100m �ر��� ���� üũ

	bool HasHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, DestLocation, ECC_Visibility, CollisionParams);

	return (!HasHit) ? 0.f : HitResult.Distance;

}

