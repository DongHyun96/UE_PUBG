// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/C_ParkourComponent.h"
#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "MotionWarpingComponent.h"

#include "Character/Component/ParkourActionStrategy/I_ParkourActionStrategy.h"
#include "Character/Component/ParkourActionStrategy/C_VaultLowActionStrategy.h"
#include "Character/Component/ParkourActionStrategy/C_VaultHighActionStrategy.h"
#include "Character/Component/ParkourActionStrategy/C_MantleLowActionStrategy.h"
#include "Character/Component/ParkourActionStrategy/C_MantleHighActionStrategy.h"

#include "Utility/C_Util.h"

TMap<EParkourActionType, class II_ParkourActionStrategy*> UC_ParkourComponent::ParkourActionStrategies{};

UC_ParkourComponent::UC_ParkourComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	//OwnerCharacter->GetMesh()->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &UC_ParkourComponent::OnParkourAnimMontageEnd);
}


void UC_ParkourComponent::BeginPlay()
{
	Super::BeginPlay();

	// Init ParkourMontageMap
	ParkourMontageMap.Add(EParkourActionType::VAULTING_LOW,  LowVaultingMontages);
	ParkourMontageMap.Add(EParkourActionType::VAULTING_HIGH, HighVaultingMontages);
	ParkourMontageMap.Add(EParkourActionType::MANTLING_LOW,  LowMantlingMontages);
	ParkourMontageMap.Add(EParkourActionType::MANTLING_HIGH, HighMantlingMontages);

	/*
	UAction* action{ NewObject<UAction>(GetTransientPackage(), 
                                    UAction::StaticClass()),
			            
	*/

	// Init ParkourAction Strategies
	if (ParkourActionStrategies.IsEmpty())
	{
		//TStrongObjectPtr<UC_VaultLowActionStrategy> VaultLowActionStrategy =
		//	TStrongObjectPtr<UC_VaultLowActionStrategy>(NewObject<UC_VaultLowActionStrategy>(GetTransientPackage()));

		II_ParkourActionStrategy* VaultingLowStrategy = NewObject<UC_VaultLowActionStrategy>();
		VaultingLowStrategy->_getUObject()->AddToRoot(); // GC ���� -> ���� : �ݵ�� RemoveFromRoot ȣ���� ��

		II_ParkourActionStrategy* VaultingHighStrategy = NewObject<UC_VaultHighActionStrategy>();
		VaultingHighStrategy->_getUObject()->AddToRoot();

		II_ParkourActionStrategy* MantleLowStrategy = NewObject<UC_MantleLowActionStrategy>();
		MantleLowStrategy->_getUObject()->AddToRoot();

		II_ParkourActionStrategy* MantleHighStrategy = NewObject<UC_MantleHighActionStrategy>();
		MantleHighStrategy->_getUObject()->AddToRoot();

		ParkourActionStrategies.Add(EParkourActionType::VAULTING_LOW,  VaultingLowStrategy);
		ParkourActionStrategies.Add(EParkourActionType::VAULTING_HIGH, VaultingHighStrategy);
		ParkourActionStrategies.Add(EParkourActionType::MANTLING_LOW,  MantleLowStrategy);
		ParkourActionStrategies.Add(EParkourActionType::MANTLING_HIGH, MantleHighStrategy);
	}
}


void UC_ParkourComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// ���� Game�� �������̰� �����ִ� ĳ���͵��� �������� ���� �ʿ�� ��
	if (EndPlayReason == EEndPlayReason::Type::Destroyed) return;

	// static ������� �޸� ����
	if (!ParkourActionStrategies.IsEmpty())
	{
		for (TPair<EParkourActionType, II_ParkourActionStrategy*>& pair : ParkourActionStrategies)
			pair.Value->_getUObject()->RemoveFromRoot();

		ParkourActionStrategies.Empty();
	}
}

void UC_ParkourComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//if (OwnerCharacter->GetMesh()->GetSkeletalMeshAsset() == MainSkeletalMesh)
	//	UC_Util::Print("MainSkeletal");
	//
	//if (OwnerCharacter->GetMesh()->GetAnimClass() == MainAnimInstance)
	//	UC_Util::Print("MainAnimClass");

	//UC_Util::Print(OwnerCharacter->GetCharacterMovement()->GetMovementName());

	if (bPendingMeshUpdateToMainMesh)
	{
		bPendingMeshUpdateToMainMesh = false;
		SwapMesh(false);
		OwnerCharacter->SetCanMove(true);
		//bIsCurrentlyWarping = false;
	}

	// Testing
	//OwnerCharacter->GetMesh()->GetBoneTransform()
	// LeftHandMiddle2
	// RightToeBase
	/*const FName LeftHandName = "LeftHandMiddle2";
	const FName RightToeName = "RightToeBase";
	static float _Max{};
	FVector HandLocation = OwnerCharacter->GetMesh()->GetBoneTransform(LeftHandName).GetTranslation();
	FVector FootLocation = OwnerCharacter->GetMesh()->GetBoneTransform(RightToeName).GetTranslation();
	_Max = FMath::Max(_Max, HandLocation.Z - FootLocation.Z);
	UC_Util::Print(_Max);*/
}

bool UC_ParkourComponent::TryExecuteParkourAction()
{
	if (OwnerCharacter->GetPoseState() != EPoseState::STAND) return false;

	// Init CurParkourDesc
	FParkourDescriptor CurParkourDesc{};

	// Check front obstacle and obstacle's hit location
	if (!CheckParkourTarget(CurParkourDesc)) return false;

	UC_Util::Print("1", FColor::MakeRandomColor(), 10.f);

	// ��� �Ÿ����� ��ֹ��� �������� �����ִ��� ����
	if (!InitVerticleHitPositionsAndLandPos(CurParkourDesc)) return false;
	UC_Util::Print("2", FColor::MakeRandomColor(), 10.f);

	if (!InitCurParkourActionStrategy(CurParkourDesc)) return false;
	UC_Util::Print("3", FColor::MakeRandomColor(), 10.f);

	OwnerCharacter->SetCanMove(false);
	OwnerCharacter->SetNextSpeed(0.f);
	if (OwnerPlayer) OwnerPlayer->SetStrafeRotationToIdleStop();

	SwapMesh(true);

	OwnerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
	OwnerCharacter->SetActorEnableCollision(false);

	// ParkourActionStrategy ����
	CurParkourActionStrategy->UseMotionWarpActionStrategy(OwnerCharacter, CurParkourDesc);
	return true;
}

void UC_ParkourComponent::SwapMesh(bool ToRootedMesh)
{
	if (!RootedSkeletalMesh || !RootedAnimInstanceClass)
	{
		UC_Util::Print("From UC_ParkourComponent : Var not inited!", FColor::Red, 10.f);
		return;
	}

	UC_Util::Print("Swapping Mesh", FColor::Red, 3.f);

	static FTransform MainRelativeTransform{};

	if (ToRootedMesh)
	{
		// �̹� �ش� mesh�� ��
		if (OwnerCharacter->GetMesh()->GetSkeletalMeshAsset() == RootedSkeletalMesh) return;

		MainSkeletalMesh = OwnerCharacter->GetMesh()->GetSkeletalMeshAsset();
		//MainAnimInstance = OwnerCharacter->GetMesh()->GetAnimClass();

		MainRelativeTransform = OwnerCharacter->GetMesh()->GetRelativeTransform();

		OwnerCharacter->GetMesh()->SetSkeletalMesh(RootedSkeletalMesh);
		OwnerCharacter->GetMesh()->SetAnimInstanceClass(RootedAnimInstanceClass);
		OwnerCharacter->GetMesh()->SetRelativeTransform(MainRelativeTransform);
	}
	else
	{
		// �̹� �ش� mesh�� ��
		if (OwnerCharacter->GetMesh()->GetSkeletalMeshAsset() == MainSkeletalMesh) return;

		OwnerCharacter->GetMesh()->SetSkeletalMesh(MainSkeletalMesh);
		OwnerCharacter->GetMesh()->SetAnimInstanceClass(MainAnimInstanceClass);
		OwnerCharacter->GetMesh()->SetRelativeTransform(MainRelativeTransform);
	}
}

bool UC_ParkourComponent::CheckParkourTarget(FParkourDescriptor& CurParkourDesc)
{
	static const float FORWARD_CHECK_LENGTH = 80.f;

	FHitResult				HitResult{};
	FCollisionQueryParams	CollisionParams{};
	CollisionParams.AddIgnoredActor(OwnerCharacter);

	TArray<AActor*> AttachedActors{};
	OwnerCharacter->GetAttachedActors(AttachedActors);
	CollisionParams.AddIgnoredActors(AttachedActors);

	// ������ �� �ִ� �������� üũ
	FVector Start	= OwnerCharacter->GetActorLocation() + FVector::UnitZ() * 175.f;
	FVector Dest	= Start + OwnerCharacter->GetActorForwardVector() * FORWARD_CHECK_LENGTH;

	bool NotPossibleHeight = GetWorld()->SweepSingleByChannel
	(
		HitResult,
		Start,
		Dest,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(5.f),
		CollisionParams
	);

	// ���� �� �� �ִ� ���̰� �ƴ�
	if (NotPossibleHeight) return false;

	for (int i = 0; i < 6; i++)
	{
		FVector StartLocation	= OwnerCharacter->GetActorLocation() + FVector::UnitZ() * (150.f - i * 30.f); // ������ �Ʒ��� �˻� ����
		FVector DestLocation	= StartLocation + OwnerCharacter->GetActorForwardVector() * FORWARD_CHECK_LENGTH;

		bool HasHit = GetWorld()->SweepSingleByChannel
		(
			HitResult,
			StartLocation,
			DestLocation,
			FQuat::Identity,
			ECC_Visibility,
			FCollisionShape::MakeSphere(5.f),
			CollisionParams
		);

		DrawDebugCylinder(GetWorld(), StartLocation, DestLocation, 5.f, 4, FColor::Red, true);
		//DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 5.f, 4, FColor::Yellow, true);

		if (HasHit)
		{
			//if (i < 4) // High Parkour else Low Parkour
			CurParkourDesc.bIsLowAction				= (i < 4) ? false : true;
			CurParkourDesc.FirstObstacleHitLocation = HitResult.Location;
			return true;
		}
	}
	return false;
}

bool UC_ParkourComponent::InitVerticleHitPositionsAndLandPos(FParkourDescriptor& CurParkourDesc)
{
	// Check Distance

	FCollisionQueryParams CollisionParams{};
	CollisionParams.AddIgnoredActor(OwnerCharacter);

	TArray<AActor*> AttachedActors{};
	OwnerCharacter->GetAttachedActors(AttachedActors);
	CollisionParams.AddIgnoredActors(AttachedActors);
	
	bool HasHit{};

	FVector MantlingMiddlePos{};

	for (int distanceLevel = 0; distanceLevel < 4; distanceLevel++)
	{
		FVector StartLocation = CurParkourDesc.FirstObstacleHitLocation +
								FVector::UnitZ() * 200.f +
								OwnerCharacter->GetActorForwardVector() * (50.f * distanceLevel);
		FVector DestLocation = StartLocation - FVector::UnitZ() * 200.f;

		FHitResult HitResult{};

		HasHit = GetWorld()->SweepSingleByChannel
		(
			HitResult,
			StartLocation,
			DestLocation,
			FQuat::Identity,
			ECC_Visibility,
			FCollisionShape::MakeSphere(10.f),
			CollisionParams
		);

		if (HasHit)
		{
			CurParkourDesc.VerticleHitPositions.Add(HitResult.Location);
			continue;
		}

		// ù Start pos�� ����� �������� ��
		if (distanceLevel == 0) return false;

		// Set Landing Position
		FVector Start = HitResult.TraceStart + OwnerCharacter->GetActorForwardVector() * 80.f;
		FVector End   = Start - FVector::UnitZ() * 1000.f; // TODO - �ʹ� ���� ���̷� LandPos �������� ���� -> Test / �̷� ������ Vaulting���� �� ó��
		HitResult = {};

		HasHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, CollisionParams);

		if (HasHit)
		{
			CurParkourDesc.LandPos = HitResult.Location;
			CurParkourDesc.bLandPosInited = true;
			return true;
		}
	}

	return false;
}

bool UC_ParkourComponent::InitCurParkourActionStrategy(const FParkourDescriptor& CurParkourDesc)
{
	UC_Util::Print("InitCurParkourActionStrategy", FColor::Red, 10.f);

	CurParkourActionStrategy = nullptr;

	const int DIST_LEVEL = CurParkourDesc.VerticleHitPositions.Num();

	if (DIST_LEVEL <= 0)
	{
		UC_Util::Print("From InitCurParkourActionStrategy : DIST_LEVEL <= 0", FColor::Red, 10.f);
		return false;
	}

	// ������ Vaulting ó�� �ؾ��� �� - Mantling�� ������ ������� ���� ��
	if (DIST_LEVEL == 1)
	{
		if (!CurParkourDesc.bLandPosInited) 
		{
			UC_Util::Print("From InitCurParkourActionStrategy : Vault but Land Pos not inited", FColor::Red, 10.f);
			return false;
		}

		CurParkourActionStrategy = ParkourActionStrategies[CurParkourDesc.bIsLowAction ?
			EParkourActionType::VAULTING_LOW : EParkourActionType::VAULTING_HIGH];
		return true;
	}

	bool ObstaclePossibleToVault{};	// Vault ������ ������������ üũ
	bool DistancePossibleToVault = DIST_LEVEL <= 3; // Vault�� �� �ִ� �Ÿ�����
	
	float FirstPosZ = CurParkourDesc.VerticleHitPositions[0].Z;
	for (int distLevel = 1; distLevel < DIST_LEVEL; distLevel++)
		ObstaclePossibleToVault = CurParkourDesc.VerticleHitPositions[distLevel].Z - FirstPosZ < 50.f;

	// ������ Mantling���� ó���ؾ� �� ��
	if (!DistancePossibleToVault || !ObstaclePossibleToVault)
	{
		CurParkourActionStrategy = ParkourActionStrategies[CurParkourDesc.bIsLowAction ?
			EParkourActionType::MANTLING_LOW : EParkourActionType::MANTLING_HIGH];
		return true;
	}

	// ���̰� ���� ���� DistanceLevel 1 �ʰ��̸� ������ Mantling
	if (!CurParkourDesc.bIsLowAction && DIST_LEVEL > 1)
	{
		CurParkourActionStrategy = ParkourActionStrategies[CurParkourDesc.bIsLowAction ?
			EParkourActionType::MANTLING_LOW : EParkourActionType::MANTLING_HIGH];
		return true;
	}

	if (bHasTryVaulting)
	{
		if (!CurParkourDesc.bLandPosInited)
		{
			UC_Util::Print("From InitCurParkourActionStrategy : Vault but Land Pos not inited", FColor::Red, 10.f);
			return false;
		}

		CurParkourActionStrategy = ParkourActionStrategies[CurParkourDesc.bIsLowAction ?
			EParkourActionType::VAULTING_LOW : EParkourActionType::VAULTING_HIGH];
		return true;
	}
	else
	{
		CurParkourActionStrategy = ParkourActionStrategies[CurParkourDesc.bIsLowAction ?
			EParkourActionType::MANTLING_LOW : EParkourActionType::MANTLING_HIGH];
		return true;
	}
}

void UC_ParkourComponent::OnParkourAnimMontageEnd()
{
	UC_Util::Print("OnParkourEnd", FColor::Cyan, 10.f);

	OwnerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	OwnerCharacter->SetActorEnableCollision(true);

	OwnerCharacter->GetMesh()->GetAnimInstance()->StopAllMontages(0);

	bPendingMeshUpdateToMainMesh = true; // ���� Update Tick���� Main skeletal mesh�� ���ư�
}

