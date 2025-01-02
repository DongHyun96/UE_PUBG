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

#include "Character/Component/C_EquippedComponent.h"
#include "Item/Weapon/C_Weapon.h"
#include "Item/Weapon/Gun/C_Gun.h"

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

	// ���� Game�� �������̰� �����ִ� ĳ���͵��� Parkour Action �������� ���� �ʿ�� ��
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
		// Swap Mesh to original main skeletal mesh

		bPendingMeshUpdateToMainMesh = false;
		SwapMesh(false);

		// Set CanMove �ð��� ���� ����
		if (CanMoveTimerAfterWarpActionFin <= 0.f)
		{
			UC_Util::Print("SetCanMove True On ParkourComponent", FColor::Red, 10.f);
			OwnerCharacter->SetCanMove(true);
		}
		else
			GetWorld()->GetTimerManager().SetTimer
			(
				TimerHandle,
				this, 
				&UC_ParkourComponent::SetOwnerCharacterCanMoveToTrue, 
				CanMoveTimerAfterWarpActionFin, 
				false
			);

		// ���� ���� ���Ⱑ �־��� �� �ش� ���� �ٽ� ���� �õ�
		OwnerCharacter->GetEquippedComponent()->TryReAttachCurWeaponToHand();

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
	DebugMsgColor = FColor::MakeRandomColor();

	if (OwnerCharacter->GetPoseState() != EPoseState::STAND) return false;

	// Init CurParkourDesc
	FParkourDescriptor CurParkourDesc{};

	CurParkourDesc.CollisionParams.AddIgnoredActor(OwnerCharacter);

	TArray<AActor*> AttachedActors{};
	OwnerCharacter->GetAttachedActors(AttachedActors);
	CurParkourDesc.CollisionParams.AddIgnoredActors(AttachedActors);

	// Init CanMoveTimer
	CanMoveTimerAfterWarpActionFin = 0.f;

	// Check Parkour framework - Low Action ���� �����ؼ� ����ϸ� Low Action���� �켱 ó��
	if (CheckParkourFramework(CurParkourDesc, true))
	{
		ExecuteMotionWarpingAction(CurParkourDesc);
		return true;
	}

	UC_Util::Print("Failed Low Action framework", DebugMsgColor, 10.f);

	// Check Parkour framework - Low Action �˻� ���� / High Action �˻�
	if (!CheckParkourFramework(CurParkourDesc, false)) return false;

	ExecuteMotionWarpingAction(CurParkourDesc);
	return true;
}

void UC_ParkourComponent::ExecuteMotionWarpingAction(const FParkourDescriptor& CurParkourDesc)
{
	InitCurParkourActionStrategy(CurParkourDesc);

	// Parkour motion warping action
	OwnerCharacter->SetCanMove(false);
	OwnerCharacter->SetNextSpeed(0.f);
	if (OwnerPlayer) OwnerPlayer->SetStrafeRotationToIdleStop();

	// ���� ����ִ� ���Ⱑ �����Ѵٸ� ���� ��� �� �ʿ� ���̱� �õ�
	OwnerCharacter->GetEquippedComponent()->TryAttachCurWeaponToHolsterWithoutSheathMotion();

	SwapMesh(true);

	OwnerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
	OwnerCharacter->SetActorEnableCollision(false);

	// ParkourActionStrategy ����
	CurParkourActionStrategy->UseMotionWarpActionStrategy(OwnerCharacter, CurParkourDesc);
}

void UC_ParkourComponent::SwapMesh(bool ToRootedMesh)
{
	if (!RootedSkeletalMesh || !RootedAnimInstanceClass)
	{
		UC_Util::Print("From UC_ParkourComponent::SwapMesh : Var not inited!", FColor::Red, 10.f);
		return;
	}

	if (!IsValid(OwnerCharacter->GetMesh()))
	{
		UC_Util::Print("From UC_ParkourComponent::SwapMesh : Current Mesh not valid!", FColor::Red, 10.f);
		return;
	}

	UC_Util::Print("Swapping Mesh", FColor::Red, 3.f);

	static FTransform MainRelativeTransform{};

	TMap<FName, AActor*> AttachedActors{};

	if (ToRootedMesh)
	{
		// �̹� �ش� mesh�� ��
		if (OwnerCharacter->GetMesh()->GetSkeletalMeshAsset() == RootedSkeletalMesh) return;

		MainSkeletalMesh	  = OwnerCharacter->GetMesh()->GetSkeletalMeshAsset();
		MainRelativeTransform = OwnerCharacter->GetMesh()->GetRelativeTransform();

		// AttachedActors ����
		GetSocketAttachedActors(AttachedActors);

		OwnerCharacter->GetMesh()->SetSkeletalMesh(RootedSkeletalMesh);
		OwnerCharacter->GetMesh()->SetAnimInstanceClass(RootedAnimInstanceClass);
		OwnerCharacter->GetMesh()->SetRelativeTransform(MainRelativeTransform);
	}
	else
	{
		// �̹� �ش� mesh�� ��
		if (OwnerCharacter->GetMesh()->GetSkeletalMeshAsset() == MainSkeletalMesh) return;

		// AttachedActors ����
		GetSocketAttachedActors(AttachedActors);

		OwnerCharacter->GetMesh()->SetSkeletalMesh(MainSkeletalMesh);
		OwnerCharacter->GetMesh()->SetAnimInstanceClass(MainAnimInstanceClass);
		OwnerCharacter->GetMesh()->SetRelativeTransform(MainRelativeTransform);
	}

	ReAttachActorsToSocket(AttachedActors);
}

void UC_ParkourComponent::GetSocketAttachedActors(TMap<FName, AActor*>& AttachedActors)
{
	if (!IsValid(OwnerCharacter->GetMesh()))
	{
		UC_Util::Print("From UC_ParkourComponent::GetSocketAttachedActors : Current SkeletalMesh not valid!", FColor::Red, 10.f);
		return;
	}

	for (USceneComponent* Child : OwnerCharacter->GetMesh()->GetAttachChildren())
	{
		if (AActor* AttachedActor = Child->GetOwner())
		{
			FName SocketName = Child->GetAttachSocketName();
			if (SocketName == NAME_None) continue;

			AttachedActors.Add(SocketName, AttachedActor);
		}
	}
}

void UC_ParkourComponent::ReAttachActorsToSocket(const TMap<FName, AActor*>& PrevAttachedActors)
{
	for (const TPair<FName, AActor*>& Pair : PrevAttachedActors)
	{
		// RootComponent�� Attach ����̸� ����
		if (Pair.Value->GetRootComponent() == OwnerCharacter->GetRootComponent()) continue;

		// �ش� socket�� �� Mesh������ ��ȿ���� Ȯ��
		if (OwnerCharacter->GetMesh()->DoesSocketExist(Pair.Key))
		{
			UC_Util::Print(Pair.Key.ToString(), FColor::Red, 10.f);
			Pair.Value->AttachToComponent(OwnerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, Pair.Key);
		}
		else
		{
			FString DebugMsg = "From UC_ParkourComponent::ReAttachActorsToSocket : " +
				                Pair.Key.ToString() + " socket not found in swapped SkeletalMesh!";
			UC_Util::Print(DebugMsg, FColor::Red, 10.f);
		}
	}
}

bool UC_ParkourComponent::InitVerticalHitPositionsAndLandPos(FParkourDescriptor& CurParkourDesc)
{
	// �ʱ�ȭ
	CurParkourDesc.LandPos = {};
	CurParkourDesc.bLandPosInited = false;
	CurParkourDesc.VerticalHitPositions.Empty();

	// Check Distance
	bool HasHit{};

	// VerticalHitPositions First start ����
	FVector StartLocation	= CurParkourDesc.FirstObstacleHitLocation + FVector::UnitZ() * 50.f;
	FVector DestLocation	= StartLocation - FVector::UnitZ() * 100.f;
	const float SphereRad   = 10.f;
	FHitResult HitResult{};

	HasHit = GetWorld()->SweepSingleByChannel
	(
		HitResult,
		StartLocation,
		DestLocation,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(SphereRad),
		CurParkourDesc.CollisionParams
	);

	DrawDebugCylinder(GetWorld(), StartLocation, DestLocation, SphereRad, 10, FColor::Magenta, true);

	if (!HasHit)
	{
		UC_Util::Print("From InitVerticalHitPositions : First Hit pos not inited!", DebugMsgColor, 10.f);
		return false;
	}// �ʱ� VerticalHitPositions ����� �浹���� �ʾҴٸ� return false

	CurParkourDesc.VerticalHitPositions.Add(HitResult.Location);

	for (int distanceLevel = 1; distanceLevel < 4; distanceLevel++)
	{
		StartLocation = CurParkourDesc.FirstObstacleHitLocation +
								FVector::UnitZ() * 50.f +
								OwnerCharacter->GetActorForwardVector() * (50.f * distanceLevel);
		DestLocation = StartLocation - FVector::UnitZ() * 100.f;

		HitResult = {};

		//GetWorld()->SweepMultiByChannel()

		HasHit = GetWorld()->SweepSingleByChannel
		(
			HitResult,
			StartLocation,
			DestLocation,
			FQuat::Identity,
			ECC_Visibility,
			FCollisionShape::MakeSphere(SphereRad),
			CurParkourDesc.CollisionParams
		);

		if (HasHit)
		{
			CurParkourDesc.VerticalHitPositions.Add(HitResult.Location);
			DrawDebugCylinder(GetWorld(), StartLocation, DestLocation, SphereRad, 10, FColor::Red, true);
			continue; // ���� DistanceLevel ����
		}

		// Set Landing Position
		FVector Start = StartLocation + OwnerCharacter->GetActorForwardVector() * 80.f;
		FVector End   = Start - FVector::UnitZ() * 50000.f; // �ʹ� ���� ���̷� LandPos �������� ���� -> Test / �̷� ������ Vaulting���� ó��
		HitResult = {};

		HasHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, CurParkourDesc.CollisionParams);

		if (!HasHit) return true;

		// FirstVerticalHitPositionZ���� ������ LandPos init ��Ű��
		if (HitResult.Location.Z < CurParkourDesc.VerticalHitPositions[0].Z)
		{
			CurParkourDesc.LandPos = HitResult.Location;
			CurParkourDesc.bLandPosInited = true;
			return true;
		}
	}
	return true;
}

void UC_ParkourComponent::InitCurParkourActionStrategy(const FParkourDescriptor& CurParkourDesc)
{
	CurParkourActionStrategy = nullptr;
	
	if (CurParkourDesc.bMustVault) // ������ Vaulting ó�� �ؾ��� ��
	{
		// Vault Action strategy���� handling�� ��
		//if (!CurParkourDesc.bLandPosInited) return false;

		CurParkourActionStrategy = ParkourActionStrategies[CurParkourDesc.bIsLowAction ?
			EParkourActionType::VAULTING_LOW : EParkourActionType::VAULTING_HIGH];
		return;
	}

	if (CurParkourDesc.bMustMantle) // ������ Mantling���� ó���ؾ� �� ��
	{
		CurParkourActionStrategy = ParkourActionStrategies[CurParkourDesc.bIsLowAction ?
			EParkourActionType::MANTLING_LOW : EParkourActionType::MANTLING_HIGH];
		return;
	}

	// Mantling, Vaulting �� �� ������ ���, Input�� ���� ó��

	if (bHasTryVaulting)
	{
		//if (!CurParkourDesc.bLandPosInited) return false;

		CurParkourActionStrategy = ParkourActionStrategies[CurParkourDesc.bIsLowAction ?
			EParkourActionType::VAULTING_LOW : EParkourActionType::VAULTING_HIGH];
	}
	else
	{
		CurParkourActionStrategy = ParkourActionStrategies[CurParkourDesc.bIsLowAction ?
			EParkourActionType::MANTLING_LOW : EParkourActionType::MANTLING_HIGH];
	}
}

bool UC_ParkourComponent::CheckParkourFramework(FParkourDescriptor& CurParkourDesc, bool bCheckLowAction)
{
	if ((bCheckLowAction) ? !CheckLowParkourTarget(CurParkourDesc) : !CheckHighParkourTarget(CurParkourDesc))
	{
		UC_Util::Print("Failed Checking Parkour Target!", DebugMsgColor, 10.f);
		return false;
	}
	if (!InitVerticalHitPositionsAndLandPos(CurParkourDesc))
	{
		UC_Util::Print("Failed Initing VerticalHitPositions and landPos!", DebugMsgColor, 10.f);
		return false;
	}
	if (!InitMustVaultOrMustMantle(CurParkourDesc))
	{
		UC_Util::Print("Failed Initing Must Vault Or Must Mantle!", DebugMsgColor, 10.f);
		return false;
	}
	if (!CheckSpaceForParkourAction(CurParkourDesc))
	{
		UC_Util::Print("Failed Checking Space for Parkour Action!", DebugMsgColor, 10.f);
		return false;
	}

	UC_Util::Print("Parkour Framework checking succeeded!", DebugMsgColor, 10.f);

	return true;
}

bool UC_ParkourComponent::CheckLowParkourTarget(FParkourDescriptor& CurParkourDesc)
{
	static const float FORWARD_CHECK_LENGTH = 100.f;

	FHitResult HitResult{};

	// Low Parkour Target�� �´��� ���� üũ

	// 85
	FVector Start = OwnerCharacter->GetActorLocation() + FVector::UnitZ() * 85.f;
	FVector Dest = Start + OwnerCharacter->GetActorForwardVector() * FORWARD_CHECK_LENGTH;
	

	bool NotPossibleHeight = GetWorld()->SweepSingleByChannel
	(
		HitResult,
		Start,
		Dest,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(5.f),
		CurParkourDesc.CollisionParams
	);

	if (NotPossibleHeight) return false;

	HitResult = {};

	// 60, 30, 0

	for (int i = 0; i < 3; i++)
	{
		FVector StartLocation	 = OwnerCharacter->GetActorLocation() + FVector::UnitZ() * (60.f - i * 30.f); // ������ �Ʒ��� �˻� ����
		FVector DestLocation	 = StartLocation + OwnerCharacter->GetActorForwardVector() * FORWARD_CHECK_LENGTH;
		const float SphereRadius = 10.f;

		bool HasHit = GetWorld()->SweepSingleByChannel
		(
			HitResult,
			StartLocation,
			DestLocation,
			FQuat::Identity,
			ECC_Visibility,
			FCollisionShape::MakeSphere(SphereRadius),
			CurParkourDesc.CollisionParams
		);

		DrawDebugCylinder(GetWorld(), StartLocation, DestLocation, SphereRadius, 20, FColor::Red, true);

		if (HasHit)
		{
			CurParkourDesc.FirstObstacleHitLocation = HitResult.ImpactPoint;
			CurParkourDesc.bIsLowAction = true;
			return true;
		}
	}

	return false;
}

bool UC_ParkourComponent::CheckHighParkourTarget(FParkourDescriptor& CurParkourDesc)
{
	static const float FORWARD_CHECK_LENGTH = 100.f;

	FHitResult HitResult{};

	// ������ �� �ִ� �������� üũ
	FVector Start = OwnerCharacter->GetActorLocation() + FVector::UnitZ() * 205.f;
	FVector Dest = Start + OwnerCharacter->GetActorForwardVector() * FORWARD_CHECK_LENGTH;

	bool NotPossibleHeight = GetWorld()->SweepSingleByChannel
	(
		HitResult,
		Start,
		Dest,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(5.f),
		CurParkourDesc.CollisionParams
	);

	// ���� �� �� �ִ� ���̰� �ƴ�
	if (NotPossibleHeight) return false;

	HitResult = {};

	// 180, 150, 120, 90

	for (int i = 0; i < 4; i++)
	{
		FVector StartLocation	 = OwnerCharacter->GetActorLocation() + FVector::UnitZ() * (180.f - i * 30.f); // ������ �Ʒ��� �˻� ����
		FVector DestLocation	 = StartLocation + OwnerCharacter->GetActorForwardVector() * FORWARD_CHECK_LENGTH;
		const float SphereRadius = 10.f;

		bool HasHit = GetWorld()->SweepSingleByChannel
		(
			HitResult,
			StartLocation,
			DestLocation,
			FQuat::Identity,
			ECC_Visibility,
			FCollisionShape::MakeSphere(SphereRadius),
			CurParkourDesc.CollisionParams
		);

		DrawDebugCylinder(GetWorld(), StartLocation, DestLocation, SphereRadius, 20, FColor::Red, true);

		if (HasHit)
		{
			CurParkourDesc.FirstObstacleHitLocation = HitResult.ImpactPoint;
			CurParkourDesc.bIsLowAction = false;
			return true;
		}
	}

	return false;
}

bool UC_ParkourComponent::CheckSpaceForParkourAction(FParkourDescriptor& CurParkourDesc)
{
	FHitResult HitResult{};

	// ������ Mantling ó���ؾ��ϴ� ��� - InitMustVaultOrMustMantle ó�� �����ϱ�
	if (CurParkourDesc.bMustMantle) return CheckSpaceForMantling(CurParkourDesc);

	// ������ Vaulting ó���ؾ� �ϴ� ��� 
	if (CurParkourDesc.bMustVault) return CheckSpaceForVaulting(CurParkourDesc);

	// TODO : Vaulting & Mantling ��� ������ ��Ȳ -> MustVaulting MustMantle ���⼭�� üũ�� ��
	
	bool CanMantle = CheckSpaceForMantling(CurParkourDesc);
	bool CanVault = CheckSpaceForVaulting(CurParkourDesc);
	
	if (CanMantle && CanVault)			return true;
	else if (!CanMantle && !CanVault)	return false;
	else if (CanMantle) // Mantle ó���ۿ� ���� ��
	{
		CurParkourDesc.bMustMantle = true;
		CurParkourDesc.bMustVault  = false;
	}
	else // Vault ó�� �ۿ� ���� ��
	{
		CurParkourDesc.bMustMantle = false;
		CurParkourDesc.bMustVault  = true;
	}
	return true;
}

bool UC_ParkourComponent::CheckSpaceForVaulting(const FParkourDescriptor& CurParkourDesc)
{
	if (!CurParkourDesc.bLandPosInited) return false;

	FHitResult HitResult{};

	// �߰����� LandPos���� Vault ��� �� ��ֹ��� �ִ��� �˻�
	FVector StartLocation = CurParkourDesc.VerticalHitPositions[0] + FVector::UnitZ() * 15.f;
	FVector DestLocation  = CurParkourDesc.LandPos;
	DestLocation.Z		  = StartLocation.Z;

	const FVector BoxExtent = { 10.f, 10.f, 10.f };

	bool HasHit = GetWorld()->SweepSingleByChannel
	(
		HitResult,
		StartLocation,
		DestLocation,
		OwnerCharacter->GetActorRotation().Quaternion(),
		ECC_Visibility,
		FCollisionShape::MakeBox(BoxExtent),
		CurParkourDesc.CollisionParams
	);

	DrawDebugBox(GetWorld(), StartLocation, BoxExtent, OwnerCharacter->GetActorRotation().Quaternion(), FColor::Blue, true);
	DrawDebugBox(GetWorld(), DestLocation,  BoxExtent, OwnerCharacter->GetActorRotation().Quaternion(), FColor::Blue, true);

	return !HasHit;
}

bool UC_ParkourComponent::CheckSpaceForMantling(const FParkourDescriptor& CurParkourDesc)
{
	FHitResult HitResult{};

	// Start���� Mantle ��� �� ��ֹ��� �ִ��� �˻�
	FVector StartLocation = CurParkourDesc.VerticalHitPositions[0] + FVector::UnitZ() * 15.f;
	FVector DestLocation  = CurParkourDesc.VerticalHitPositions[1];
	DestLocation.Z		  = StartLocation.Z;

	FVector BoxExtent = { 10.f, 10.f, 10.f };

	bool HasHit = GetWorld()->SweepSingleByChannel
	(
		HitResult,
		StartLocation,
		DestLocation,
		OwnerCharacter->GetActorRotation().Quaternion(),
		ECC_Visibility,
		FCollisionShape::MakeBox(BoxExtent),
		CurParkourDesc.CollisionParams
	);

	DrawDebugBox(GetWorld(), StartLocation, BoxExtent, OwnerCharacter->GetActorRotation().Quaternion(), FColor::Blue, true);
	DrawDebugBox(GetWorld(), DestLocation,  BoxExtent, OwnerCharacter->GetActorRotation().Quaternion(), FColor::Blue, true);

	if (HasHit) return false;

	// ���� ���� ���� ����� ������ �������� �˻�
	FVector BoxLocation	= CurParkourDesc.VerticalHitPositions[1] + FVector::UnitZ() * 100.f;
	BoxExtent			= { 25.f, 25.f, 90.f };

	bool HasOverlapped = GetWorld()->OverlapAnyTestByChannel
	(
		BoxLocation,
		OwnerCharacter->GetActorRotation().Quaternion(),
		ECC_Visibility,
		FCollisionShape::MakeBox(BoxExtent),
		CurParkourDesc.CollisionParams
	);

	DrawDebugBox(GetWorld(), BoxLocation, BoxExtent, OwnerCharacter->GetActorRotation().Quaternion(), FColor::Yellow, true);
	return !HasOverlapped;

	/*StartLocation	  = CurParkourDesc.VerticalHitPositions[0] + FVector::UnitZ() * 100.f;
	DestLocation	  = CurParkourDesc.VerticalHitPositions[1] + FVector::UnitZ() * 100.f;
	BoxExtent		  = { 25.f, 25.f, 90.f };

	HasHit = GetWorld()->SweepSingleByChannel
	(
		HitResult,
		StartLocation,
		DestLocation,
		OwnerCharacter->GetActorRotation().Quaternion(),
		ECC_Visibility,
		FCollisionShape::MakeBox(BoxExtent),
		CurParkourDesc.CollisionParams
	);

	DrawDebugBox(GetWorld(), StartLocation, BoxExtent, OwnerCharacter->GetActorRotation().Quaternion(), FColor::Yellow, true);
	DrawDebugBox(GetWorld(), DestLocation,  BoxExtent, OwnerCharacter->GetActorRotation().Quaternion(), FColor::Yellow, true);

	return !HasHit;*/
}

bool UC_ParkourComponent::InitMustVaultOrMustMantle(FParkourDescriptor& CurParkourDesc)
{
	const int DIST_LEVEL = CurParkourDesc.VerticalHitPositions.Num();

	CurParkourDesc.bMustVault	= false;
	CurParkourDesc.bMustMantle	= false;

	if (DIST_LEVEL <= 0) return false;

	// ������ Vaulting ó�� �ؾ��� �� - Mantling�� ������ ������� ���� ��
	if (DIST_LEVEL == 1)
	{
		CurParkourDesc.bMustVault = true;
		return true;
	}

	bool ObstaclePossibleToVault{};	// Vault ������ ������������ üũ
	bool DistancePossibleToVault = DIST_LEVEL <= 3; // Vault�� �� �ִ� �Ÿ�����

	float FirstPosZ = CurParkourDesc.VerticalHitPositions[0].Z;
	for (int distLevel = 1; distLevel < DIST_LEVEL; distLevel++)
		ObstaclePossibleToVault = CurParkourDesc.VerticalHitPositions[distLevel].Z - FirstPosZ < 50.f;

	// ������ Mantling���� ó���ؾ� �� ��
	if (!DistancePossibleToVault || !ObstaclePossibleToVault)
	{
		CurParkourDesc.bMustMantle = true;
		return true;
	}

	// ���̰� ���� ���� DistanceLevel 1 �ʰ��̸� ������ Mantling
	if (!CurParkourDesc.bIsLowAction && DIST_LEVEL > 1)
	{
		CurParkourDesc.bMustMantle = true;
		return true;
	}

	return true;
}

void UC_ParkourComponent::OnParkourAnimMontageEnd()
{
	UC_Util::Print("OnParkourEnd", FColor::Cyan, 10.f);

	OwnerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	OwnerCharacter->SetActorEnableCollision(true);

	OwnerCharacter->GetMesh()->GetAnimInstance()->StopAllMontages(0);

	bPendingMeshUpdateToMainMesh = true; // ���� Update Tick���� Main skeletal mesh�� ���ư�
}



