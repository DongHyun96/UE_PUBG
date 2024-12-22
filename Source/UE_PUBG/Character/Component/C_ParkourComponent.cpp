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
		VaultingLowStrategy->_getUObject()->AddToRoot(); // GC 방지 -> 주의 : 반드시 RemoveFromRoot 호출할 것

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

	// 아직 Game이 진행중이고 남아있는 캐릭터들은 전략들을 아직 필요로 함
	if (EndPlayReason == EEndPlayReason::Type::Destroyed) return;

	// static 멤버변수 메모리 해제
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

		// Set CanMove 시간차 관련 설정
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

		if (OwnerCharacter->GetEquippedComponent()->GetCurWeapon()) // 착용 중인 무기가 있었을 때 해당 무기 다시 장착
		{
			UC_EquippedComponent* OwnerEquippedComponent = OwnerCharacter->GetEquippedComponent();

			OwnerCharacter->GetEquippedComponent()->SetNextWeaponType(OwnerEquippedComponent->GetCurWeaponType());
			FPriorityAnimMontage DrawMontage = OwnerEquippedComponent->GetCurWeapon()->GetCurDrawMontage();
			OwnerCharacter->PlayAnimMontage(DrawMontage);
		}

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

	// Init CanMoveTimer
	CanMoveTimerAfterWarpActionFin = 0.f;

	// Check front obstacle and obstacle's hit location
	if (!CheckParkourTarget(CurParkourDesc)) return false;

	// 어느 거리까지 장애물이 전방으로 뻗어있는지 조사
	if (!InitVerticalHitPositionsAndLandPos(CurParkourDesc)) return false;

	if (!InitCurParkourActionStrategy(CurParkourDesc)) return false;

	OwnerCharacter->SetCanMove(false);
	OwnerCharacter->SetNextSpeed(0.f);
	if (OwnerPlayer) OwnerPlayer->SetStrafeRotationToIdleStop();

	// 현재 들고 있는 무기가 존재한다면 무기 잠깐 몸 쪽에 붙이기
	if (AC_Weapon* OwnerWeapon = OwnerCharacter->GetEquippedComponent()->GetCurWeapon())
	{
		OwnerWeapon->AttachToHolster(OwnerCharacter->GetMesh());
		OwnerCharacter->SetHandState(EHandState::UNARMED);

		// 총기류 예외처리
		if (AC_Gun* Gun = Cast<AC_Gun>(OwnerWeapon)) Gun->BackToMainCamera();
	}

	SwapMesh(true);

	OwnerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
	OwnerCharacter->SetActorEnableCollision(false);

	// ParkourActionStrategy 실행
	CurParkourActionStrategy->UseMotionWarpActionStrategy(OwnerCharacter, CurParkourDesc);
	return true;
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

bool UC_ParkourComponent::CheckParkourTarget(FParkourDescriptor& CurParkourDesc)
{
	static const float FORWARD_CHECK_LENGTH = 100.f;

	FHitResult				HitResult{};
	FCollisionQueryParams	CollisionParams{};
	CollisionParams.AddIgnoredActor(OwnerCharacter);

	TArray<AActor*> AttachedActors{};
	OwnerCharacter->GetAttachedActors(AttachedActors);
	CollisionParams.AddIgnoredActors(AttachedActors);

	// 파쿠르할 수 있는 높이인지 체크
	FVector Start	= OwnerCharacter->GetActorLocation() + FVector::UnitZ() * 205.f;
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

	// 파쿠르 할 수 있는 높이가 아님
	if (NotPossibleHeight) return false;

	for (int i = 0; i < 7; i++)
	{
		FVector StartLocation	= OwnerCharacter->GetActorLocation() + FVector::UnitZ() * (180.f - i * 30.f); // 위에서 아래로 검사 진행
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

bool UC_ParkourComponent::InitVerticalHitPositionsAndLandPos(FParkourDescriptor& CurParkourDesc)
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
			CurParkourDesc.VerticalHitPositions.Add(HitResult.Location);
			continue;
		}

		// 첫 Start pos가 제대로 안잡혔을 때
		if (distanceLevel == 0) return false;

		// Set Landing Position
		FVector Start = HitResult.TraceStart + OwnerCharacter->GetActorForwardVector() * 80.f;
		FVector End   = Start - FVector::UnitZ() * 1000.f; // TODO - 너무 낮은 높이로 LandPos 잡힐수도 있음 -> Test / 이럴 때에는 Vaulting에서 잘 처리
		HitResult = {};

		HasHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, CollisionParams);

		if (HasHit)
		{
			CurParkourDesc.LandPos = HitResult.Location;
			CurParkourDesc.bLandPosInited = true;
			return true;
		}
	}

	return true;
}

bool UC_ParkourComponent::InitCurParkourActionStrategy(const FParkourDescriptor& CurParkourDesc)
{
	CurParkourActionStrategy = nullptr;

	const int DIST_LEVEL = CurParkourDesc.VerticalHitPositions.Num();

	if (DIST_LEVEL <= 0) return false;

	// 무조건 Vaulting 처리 해야할 때 - Mantling할 공간이 충분하지 않을 때
	if (DIST_LEVEL == 1)
	{
		// Vault Action strategy에서 handling할 것
		//if (!CurParkourDesc.bLandPosInited) return false;

		CurParkourActionStrategy = ParkourActionStrategies[CurParkourDesc.bIsLowAction ?
			EParkourActionType::VAULTING_LOW : EParkourActionType::VAULTING_HIGH];
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
		CurParkourActionStrategy = ParkourActionStrategies[CurParkourDesc.bIsLowAction ?
			EParkourActionType::MANTLING_LOW : EParkourActionType::MANTLING_HIGH];
		return true;
	}

	// 높이가 높을 때에 DistanceLevel 1 초과이면 무조건 Mantling
	if (!CurParkourDesc.bIsLowAction && DIST_LEVEL > 1)
	{
		CurParkourActionStrategy = ParkourActionStrategies[CurParkourDesc.bIsLowAction ?
			EParkourActionType::MANTLING_LOW : EParkourActionType::MANTLING_HIGH];
		return true;
	}

	if (bHasTryVaulting)
	{
		//if (!CurParkourDesc.bLandPosInited) return false;

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

	bPendingMeshUpdateToMainMesh = true; // 다음 Update Tick에서 Main skeletal mesh로 돌아감
}

