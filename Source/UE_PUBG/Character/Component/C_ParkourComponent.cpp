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
#include "Components/CapsuleComponent.h"
#include "Item/Weapon/C_Weapon.h"
#include "Item/Weapon/Gun/C_Gun.h"

#include "Singleton/C_GameSceneManager.h"

#include "Utility/C_Util.h"

TMap<EParkourActionType, class II_ParkourActionStrategy*> UC_ParkourComponent::ParkourActionStrategies{};
int UC_ParkourComponent::ParkourComponentCount{};

UC_ParkourComponent::UC_ParkourComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	//OwnerCharacter->GetMesh()->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &UC_ParkourComponent::OnParkourAnimMontageEnd);
}


void UC_ParkourComponent::BeginPlay()
{
	Super::BeginPlay();

	++ParkourComponentCount;

	// Init ParkourMontageMap
	ParkourMontageMap.Add(EParkourActionType::VAULTING_LOW,  LowVaultingMontages);
	ParkourMontageMap.Add(EParkourActionType::VAULTING_HIGH, HighVaultingMontages);
	ParkourMontageMap.Add(EParkourActionType::MANTLING_LOW,  LowMantlingMontages);
	ParkourMontageMap.Add(EParkourActionType::MANTLING_HIGH, HighMantlingMontages);

	// Init ParkourAction Strategies
	if (ParkourActionStrategies.IsEmpty())
	{
		II_ParkourActionStrategy* VaultingLowStrategy = NewObject<UC_VaultLowActionStrategy>();
		VaultingLowStrategy->_getUObject()->AddToRoot(); // GC 방지 -> 주의 : 반드시 RemoveFromRoot 호출할 것

		II_ParkourActionStrategy* VaultingHighStrategy = NewObject<UC_VaultHighActionStrategy>();
		VaultingHighStrategy->_getUObject()->AddToRoot();

		II_ParkourActionStrategy* MantleLowStrategy = NewObject<UC_MantleLowActionStrategy>();
		MantleLowStrategy->_getUObject()->AddToRoot();

		II_ParkourActionStrategy* MantleHighStrategy = NewObject<UC_MantleHighActionStrategy>();
		MantleHighStrategy->_getUObject()->AddToRoot();

		GAMESCENE_MANAGER->AddGCProtectedObject(VaultingLowStrategy->_getUObject());
		GAMESCENE_MANAGER->AddGCProtectedObject(VaultingHighStrategy->_getUObject());
		GAMESCENE_MANAGER->AddGCProtectedObject(MantleLowStrategy->_getUObject());
		GAMESCENE_MANAGER->AddGCProtectedObject(MantleHighStrategy->_getUObject());

		ParkourActionStrategies.Add(EParkourActionType::VAULTING_LOW,  VaultingLowStrategy);
		ParkourActionStrategies.Add(EParkourActionType::VAULTING_HIGH, VaultingHighStrategy);
		ParkourActionStrategies.Add(EParkourActionType::MANTLING_LOW,  MantleLowStrategy);
		ParkourActionStrategies.Add(EParkourActionType::MANTLING_HIGH, MantleHighStrategy);
	}

	FString Str = "ParkourComponent Count : " + FString::FromInt(ParkourComponentCount);
	UC_Util::Print(Str, FColor::MakeRandomColor(), 10.f);
}


void UC_ParkourComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (EndPlayReason == EEndPlayReason::Type::Destroyed)
	{
		if (--ParkourComponentCount <= 0)
		{
			if (!ParkourActionStrategies.IsEmpty()) ParkourActionStrategies.Empty(); // GC는 GameSceneManager에서 처리 예정
		}
		return;
	}

	ParkourComponentCount = 0;
	
	if (!ParkourActionStrategies.IsEmpty()) ParkourActionStrategies.Empty(); // GC는 GameSceneManager에서 처리 예정
}

void UC_ParkourComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bPendingMeshUpdateToMainMesh)
	{
		// Swap Mesh to original main skeletal mesh

		bPendingMeshUpdateToMainMesh = false;
		SwapMesh(false);

		// Set CanMove 시간차 관련 설정
		if (CanMoveTimerAfterWarpActionFin <= 0.f)
		{
			//UC_Util::Print("SetCanMove True On ParkourComponent", FColor::Red, 10.f);
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

		// 착용 중인 무기가 있었을 때 해당 무기 다시 장착 시도
		OwnerCharacter->GetEquippedComponent()->TryReAttachCurWeaponToHand();

		//bIsCurrentlyWarping = false;
	}

}

bool UC_ParkourComponent::TryExecuteParkourAction()
{
	DebugMsgColor = FColor::MakeRandomColor();

	if (OwnerCharacter->GetPoseState() != EPoseState::STAND) return false;

	// Init CurParkourDesc
	FParkourDescriptor CurParkourDesc{};

	CurParkourDesc.CollisionParams.AddIgnoredActors(GAMESCENE_MANAGER->GetAllCharacterActors());

	TArray<AActor*> AttachedActors{};
	OwnerCharacter->GetAttachedActors(AttachedActors);
	CurParkourDesc.CollisionParams.AddIgnoredActors(AttachedActors);


	// Init CanMoveTimer
	CanMoveTimerAfterWarpActionFin = 0.f;

	// Check Parkour framework - Low Action 먼저 조사해서 통과하면 Low Action으로 우선 처리
	if (CheckParkourFramework(CurParkourDesc, true))
	{
		ExecuteMotionWarpingAction(CurParkourDesc);
		return true;
	}

	//UC_Util::Print("Failed Low Action framework", DebugMsgColor, 10.f);

	// Check Parkour framework - Low Action 검사 실패 / High Action 검사
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

	// 현재 들고있는 무기가 존재한다면 무기 잠깐 몸 쪽에 붙이기 시도
	OwnerCharacter->GetEquippedComponent()->TryAttachCurWeaponToHolsterWithoutSheathMotion();

	SwapMesh(true);

	OwnerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
	// OwnerCharacter->SetActorEnableCollision(false);
	OwnerCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	// ParkourActionStrategy 실행
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

	static FTransform MainRelativeTransform{};

	TMap<FName, AActor*> AttachedActors{};

	if (ToRootedMesh)
	{
		// 이미 해당 mesh일 때
		if (OwnerCharacter->GetMesh()->GetSkeletalMeshAsset() == RootedSkeletalMesh) return;

		MainSkeletalMesh	  = OwnerCharacter->GetMesh()->GetSkeletalMeshAsset();
		MainRelativeTransform = OwnerCharacter->GetMesh()->GetRelativeTransform();

		// AttachedActors 조사
		GetSocketAttachedActors(AttachedActors);

		OwnerCharacter->GetMesh()->SetSkeletalMesh(RootedSkeletalMesh);
		OwnerCharacter->GetMesh()->SetAnimInstanceClass(RootedAnimInstanceClass);
		OwnerCharacter->GetMesh()->SetRelativeTransform(MainRelativeTransform);
	}
	else
	{
		// 이미 해당 mesh일 때
		if (OwnerCharacter->GetMesh()->GetSkeletalMeshAsset() == MainSkeletalMesh) return;

		// AttachedActors 조사
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
		// RootComponent가 Attach 대상이면 무시
		if (Pair.Value->GetRootComponent() == OwnerCharacter->GetRootComponent()) continue;

		// 해당 socket이 새 Mesh에서도 유효한지 확인
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
	// 초기화
	CurParkourDesc.LandPos = {};
	CurParkourDesc.bLandPosInited = false;
	CurParkourDesc.VerticalHitPositions.Empty();

	// Check Distance
	bool HasHit{};

	// VerticalHitPositions First start 조사
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
		//UC_Util::Print("From InitVerticalHitPositions : First Hit pos not inited!", DebugMsgColor, 10.f);
		return false;
	}// 초기 VerticalHitPositions 제대로 충돌하지 않았다면 return false

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
			continue; // 다음 DistanceLevel 조사
		}

		// Set Landing Position
		FVector Start = StartLocation + OwnerCharacter->GetActorForwardVector() * 80.f;
		FVector End   = Start - FVector::UnitZ() * 50000.f; // 너무 낮은 높이로 LandPos 잡힐수도 있음 -> Test / 이럴 때에는 Vaulting에서 처리
		HitResult = {};

		HasHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, CurParkourDesc.CollisionParams);

		if (!HasHit) return true;

		// FirstVerticalHitPositionZ보다 낮으면 LandPos init 시키기
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
	
	if (CurParkourDesc.bMustVault) // 무조건 Vaulting 처리 해야할 때
	{
		// Vault Action strategy에서 handling할 것
		//if (!CurParkourDesc.bLandPosInited) return false;

		CurParkourActionStrategy = ParkourActionStrategies[CurParkourDesc.bIsLowAction ?
			EParkourActionType::VAULTING_LOW : EParkourActionType::VAULTING_HIGH];
		return;
	}

	if (CurParkourDesc.bMustMantle) // 무조건 Mantling으로 처리해야 할 때
	{
		CurParkourActionStrategy = ParkourActionStrategies[CurParkourDesc.bIsLowAction ?
			EParkourActionType::MANTLING_LOW : EParkourActionType::MANTLING_HIGH];
		return;
	}

	// Mantling, Vaulting 둘 다 가능한 경우, Input에 따른 처리

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
		/*FString str = "Failed Checking Parkour Target!";
		str += bCheckLowAction ? " : LowAction" : " : HighAction";
		UC_Util::Print(str, DebugMsgColor, 10.f);*/
		return false;
	}
	if (!InitVerticalHitPositionsAndLandPos(CurParkourDesc))
	{
		// UC_Util::Print("Failed Initing VerticalHitPositions and landPos!", DebugMsgColor, 10.f);
		return false;
	}
	if (!InitMustVaultOrMustMantle(CurParkourDesc))
	{
		// UC_Util::Print("Failed Initing Must Vault Or Must Mantle!", DebugMsgColor, 10.f);
		return false;
	}
	if (!CheckSpaceForParkourAction(CurParkourDesc))
	{
		// UC_Util::Print("Failed Checking Space for Parkour Action!", DebugMsgColor, 10.f);
		return false;
	}

	// UC_Util::Print("Parkour Framework checking succeeded!", DebugMsgColor, 10.f);

	return true;
}

bool UC_ParkourComponent::CheckLowParkourTarget(FParkourDescriptor& CurParkourDesc)
{
	static const float FORWARD_CHECK_LENGTH = 100.f;

	FHitResult HitResult{};

	// Low Parkour Target이 맞는지 높이 체크

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
		FVector StartLocation	 = OwnerCharacter->GetActorLocation() + FVector::UnitZ() * (60.f - i * 30.f); // 위에서 아래로 검사 진행
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

	// 파쿠르할 수 있는 높이인지 체크
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

	// 파쿠르 할 수 있는 높이가 아님
	if (NotPossibleHeight) return false;

	// 머리 위로 충분한 공간이 있는지 체크해줘야 함
	// DrawDebugBox(GetWorld(), StartLocation, BoxExtent, OwnerCharacter->GetActorRotation().Quaternion(), FColor::Blue, true);
	const FVector CheckSpaceAboveStart = OwnerCharacter->GetActorLocation() + FVector::UnitZ() * 60.f;
	const FVector CheckSpaceAboveDest = OwnerCharacter->GetActorLocation()  + FVector::UnitZ() * 180.f;
	const float CheckSpaceAboveRad = OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius() * 0.65f;
	DrawDebugCylinder(GetWorld(), CheckSpaceAboveStart, CheckSpaceAboveDest, CheckSpaceAboveRad, 20, FColor::Red, true);
	HitResult = {};

	bool HasSomethingAboveHead = GetWorld()->SweepSingleByChannel
	(
		HitResult,
		CheckSpaceAboveStart,
		CheckSpaceAboveDest,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(CheckSpaceAboveRad),
		CurParkourDesc.CollisionParams
	);

	if (HasSomethingAboveHead) return false;
	
	HitResult = {};

	// 180, 150, 120, 90

	for (int i = 0; i < 4; i++)
	{
		FVector StartLocation	 = OwnerCharacter->GetActorLocation() + FVector::UnitZ() * (180.f - i * 30.f); // 위에서 아래로 검사 진행
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

	// 무조건 Mantling 처리해야하는 경우 - InitMustVaultOrMustMantle 처리 먼저하기
	if (CurParkourDesc.bMustMantle) return CheckSpaceForMantling(CurParkourDesc);

	// 무조건 Vaulting 처리해야 하는 경우 
	if (CurParkourDesc.bMustVault) return CheckSpaceForVaulting(CurParkourDesc);

	// Vaulting & Mantling 모두 가능한 상황 -> MustVaulting MustMantle 여기서도 체크할 것
	
	bool CanMantle = CheckSpaceForMantling(CurParkourDesc);
	bool CanVault = CheckSpaceForVaulting(CurParkourDesc);
	
	if (CanMantle && CanVault)		return true;
	if (!CanMantle && !CanVault)	return false;
	if (CanMantle) // Mantle 처리밖에 못할 때
	{
		CurParkourDesc.bMustMantle = true;
		CurParkourDesc.bMustVault  = false;
	}
	else // Vault 처리 밖에 못할 때
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

	// 중간부터 LandPos까지 Vault 경로 상 장애물이 있는지 검사
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

	// Start부터 Mantle 경로 상 장애물이 있는지 검사
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

	DrawDebugBox(GetWorld(), StartLocation, BoxExtent, OwnerCharacter->GetActorRotation().Quaternion(), FColor::Cyan, true);
	DrawDebugBox(GetWorld(), DestLocation,  BoxExtent, OwnerCharacter->GetActorRotation().Quaternion(), FColor::Cyan, true);

	if (HasHit)
	{
		// UC_Util::Print("From CheckSpaceForMantling : Obstacle detected along the mantle path.", DebugMsgColor, 10.f);
		return false;
	}

	// 도착 지점 위로 충분한 공간이 나오는지 검사
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

	// 무조건 Vaulting 처리 해야할 때 - Mantling할 공간이 충분하지 않을 때
	if (DIST_LEVEL == 1)
	{
		CurParkourDesc.bMustVault = true;
		return true;
	}

	bool ObstaclePossibleToVault{};	// Vault 가능한 지형지물인지 체크
	bool DistancePossibleToVault = DIST_LEVEL <= 3; // Vault할 수 있는 거리인지

	float FirstPosZ = CurParkourDesc.VerticalHitPositions[0].Z;
	for (int distLevel = 1; distLevel < DIST_LEVEL; distLevel++)
		ObstaclePossibleToVault = CurParkourDesc.VerticalHitPositions[distLevel].Z - FirstPosZ < 50.f;

	// 무조건 Mantling으로 처리해야 할 때
	if (!DistancePossibleToVault || !ObstaclePossibleToVault)
	{
		CurParkourDesc.bMustMantle = true;
		return true;
	}

	// 높이가 높을 때에 DistanceLevel 1 초과이면 무조건 Mantling
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
	// OwnerCharacter->SetActorEnableCollision(true);
	OwnerCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	
	OwnerCharacter->GetMesh()->GetAnimInstance()->StopAllMontages(0);

	bPendingMeshUpdateToMainMesh = true; // 다음 Update Tick에서 Main skeletal mesh로 돌아감
}

void UC_ParkourComponent::SwapMeshToMainSkeletalMesh()
{
	OwnerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	// OwnerCharacter->SetActorEnableCollision(true);
	OwnerCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	
	OwnerCharacter->GetMesh()->GetAnimInstance()->StopAllMontages(0);

	bPendingMeshUpdateToMainMesh = true;
}





