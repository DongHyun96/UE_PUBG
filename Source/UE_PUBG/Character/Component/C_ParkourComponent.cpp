// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/C_ParkourComponent.h"
#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "MotionWarpingComponent.h"

#include "Character/Component/ParkourActionStrategy/I_ParkourActionStrategy.h"
#include "Character/Component/ParkourActionStrategy/C_VaultLowActionStrategy.h"

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

		ParkourActionStrategies.Add(EParkourActionType::VAULTING_LOW, VaultingLowStrategy);
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

	// 어느 거리까지 장애물이 전방으로 뻗어있는지 조사
	// Check Vaulting or Mantling -> Vault Start Pos / Middle pos / Land pos check
	// CurParkourActionType 정하기
	if (!CheckParkourActionAndDistance(CurParkourDesc)) return false;

	DrawDebugSphere(GetWorld(), CurParkourDesc.WarpStartPos, 5.f, 4, FColor::Cyan,	true);
	DrawDebugSphere(GetWorld(), CurParkourDesc.WarpMiddlePos, 5.f, 4, FColor::Cyan, true);
	DrawDebugSphere(GetWorld(), CurParkourDesc.WarpLandPos, 5.f, 4, FColor::Cyan,	true);

	// Handle Motion warping
	switch (CurParkourDesc.CurParkourActionType)
	{
	case EParkourActionType::MANTLING_LOW: case EParkourActionType::MANTLING_HIGH: MantleMotionWarp(CurParkourDesc);
		return true;
	case EParkourActionType::VAULTING_LOW: case EParkourActionType::VAULTING_HIGH: VaultMotionWarp(CurParkourDesc);
		return true;
	default: return false;
	}
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
		// 이미 해당 mesh일 때
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
		// 이미 해당 mesh일 때
		if (OwnerCharacter->GetMesh()->GetSkeletalMeshAsset() == MainSkeletalMesh) return;

		OwnerCharacter->GetMesh()->SetSkeletalMesh(MainSkeletalMesh);
		OwnerCharacter->GetMesh()->SetAnimInstanceClass(MainAnimInstanceClass);
		OwnerCharacter->GetMesh()->SetRelativeTransform(MainRelativeTransform);
	}
}

void UC_ParkourComponent::VaultMotionWarp(const FParkourDescriptor& CurParkourDesc)
{
	if (CurParkourDesc.bIsLowAction)
	{
		ParkourActionStrategies[EParkourActionType::VAULTING_LOW]->UseMotionWarpActionStrategy(OwnerCharacter, CurParkourDesc);
		return;
	}
	// 기존의 제약 처리
	//float LocationZ = OwnerCharacter->GetMesh()->GetComponentLocation().Z;
	//if (VaultLandPos.Z < LocationZ - 50.f || VaultLandPos.Z > LocationZ + 50.f) return;
	//if (!CanWarp) return;
	
	//DrawDebugSphere(GetWorld(), VaultStartPos, 10.f, 12, FColor::Yellow, true);
	//DrawDebugSphere(GetWorld(), VaultMiddlePos, 10.f, 12, FColor::Yellow, true);
	//DrawDebugSphere(GetWorld(), VaultLandPos, 10.f, 12, FColor::Yellow, true);

	//bIsCurrentlyWarping = true;
	OwnerCharacter->SetCanMove(false);
	OwnerCharacter->SetNextSpeed(0.f);
	if (OwnerPlayer) OwnerPlayer->SetStrafeRotationToIdleStop();

	SwapMesh(true);

	OwnerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);

	OwnerCharacter->SetActorEnableCollision(false);
	UMotionWarpingComponent* MotionWarping = OwnerCharacter->GetMotionWarpingComponent();

	FMotionWarpingTarget Target{};
	Target.Name		= FName(TEXT("VaultStart"));
	Target.Location = CurParkourDesc.WarpStartPos;
	Target.Rotation = OwnerCharacter->GetActorRotation();

	MotionWarping->AddOrUpdateWarpTarget(Target);

	Target = {};
	Target.Name		= FName(TEXT("VaultMiddle"));
	Target.Location = CurParkourDesc.WarpMiddlePos;

	MotionWarping->AddOrUpdateWarpTarget(Target);

	Target = {};
	Target.Name		= FName(TEXT("VaultLand"));
	Target.Location = CurParkourDesc.WarpLandPos;

	MotionWarping->AddOrUpdateWarpTarget(Target);

	// TODO : ParkourMontageMap 초기화

	// 현재 Parkour Action에 해당하는 Random한 parkourAction 재생
	int RandIdx = FMath::RandRange(0, ParkourMontageMap[CurParkourDesc.CurParkourActionType].Num() - 1);
	OwnerCharacter->PlayAnimMontage(ParkourMontageMap[CurParkourDesc.CurParkourActionType][RandIdx]);
}

void UC_ParkourComponent::MantleMotionWarp(const FParkourDescriptor& CurParkourDesc)
{
	// TODO : Motion Warp -> Root 기준이라 manual하게 Z위치 맞춰줘야 함 (아닐지도?)

	// 기존의 제약 처리
	//float LocationZ = OwnerCharacter->GetMesh()->GetComponentLocation().Z;
	//if (VaultLandPos.Z < LocationZ - 50.f || VaultLandPos.Z > LocationZ + 50.f) return;
	//if (!CanWarp) return;
	
	//DrawDebugSphere(GetWorld(), VaultStartPos, 10.f, 12, FColor::Yellow, true);
	//DrawDebugSphere(GetWorld(), VaultMiddlePos, 10.f, 12, FColor::Yellow, true);
	//DrawDebugSphere(GetWorld(), VaultLandPos, 10.f, 12, FColor::Yellow, true);

	OwnerCharacter->SetCanMove(false);
	OwnerCharacter->SetNextSpeed(0.f);
	if (OwnerPlayer) OwnerPlayer->SetStrafeRotationToIdleStop();
	//bIsCurrentlyWarping = true;

	SwapMesh(true);

	OwnerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);

	OwnerCharacter->SetActorEnableCollision(false);
	UMotionWarpingComponent* MotionWarping = OwnerCharacter->GetMotionWarpingComponent();

	FMotionWarpingTarget Target{};
	Target.Name		= FName(TEXT("MantleStart"));
	Target.Location = CurParkourDesc.WarpStartPos;
	Target.Rotation = OwnerCharacter->GetActorRotation();

	MotionWarping->AddOrUpdateWarpTarget(Target);

	Target = {};
	Target.Name		= FName(TEXT("MantleEnd"));
	Target.Location = CurParkourDesc.WarpMiddlePos;

	MotionWarping->AddOrUpdateWarpTarget(Target);

	// TODO : ParkourMontageMap 초기화

	// 현재 Parkour Action에 해당하는 Random한 parkourAction 재생
	int RandIdx = FMath::RandRange(0, ParkourMontageMap[CurParkourDesc.CurParkourActionType].Num() - 1);
	OwnerCharacter->PlayAnimMontage(ParkourMontageMap[CurParkourDesc.CurParkourActionType][RandIdx]);
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

	// 파쿠르할 수 있는 높이인지 체크
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

	// 파쿠르 할 수 있는 높이가 아님
	if (NotPossibleHeight) return false;

	for (int i = 0; i < 6; i++)
	{
		FVector StartLocation	= OwnerCharacter->GetActorLocation() + FVector::UnitZ() * (150.f - i * 30.f); // 위에서 아래로 검사 진행
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

bool UC_ParkourComponent::CheckVerticleHitPoints(FParkourDescriptor& CurParkourDesc)
{
	return false;
}

bool UC_ParkourComponent::CheckParkourActionAndDistance(FParkourDescriptor& CurParkourDesc)
{
	// Check Distance

	FCollisionQueryParams CollisionParams{};
	CollisionParams.AddIgnoredActor(OwnerCharacter);

	TArray<AActor*> AttachedActors{};
	OwnerCharacter->GetAttachedActors(AttachedActors);
	CollisionParams.AddIgnoredActors(AttachedActors);
	
	bool HasHit{};
	int  DistanceLevel{};
	bool PossibleToVault{};
	bool LandInited{};

	FVector MantlingMiddlePos{};

	for (int i = 0; i < 4; i++)
	{
		FVector StartLocation = CurParkourDesc.FirstObstacleHitLocation + 
								FVector::UnitZ() * 200.f +
								OwnerCharacter->GetActorForwardVector() * (50.f * i);
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
			DrawDebugCylinder(GetWorld(), StartLocation, DestLocation, 10.f, 4, FColor::Red, true);

			DistanceLevel = i;
			if (DistanceLevel == 3)
			{
				PossibleToVault = false;
				break;
			}

			if (DistanceLevel == 0) CurParkourDesc.WarpStartPos = HitResult.Location; // Init Start Location
			//if (DistanceLevel == 1) MantlingMiddlePos = HitResult.Location + OwnerCharacter->GetActorForwardVector() * 50.f;
			if (DistanceLevel == 1) MantlingMiddlePos = HitResult.Location;


			// (Init PossibleToVault) 50cm 이상으로 높이가 높아지는 Obstacle의 경우 Vault 불가 판정
			PossibleToVault = (HitResult.Location.Z - CurParkourDesc.WarpStartPos.Z < 50.f);

			// Init Middle position
			CurParkourDesc.WarpMiddlePos = HitResult.Location;
			DrawDebugSphere(GetWorld(), CurParkourDesc.WarpMiddlePos, 10.f, 12, FColor::Yellow, true);

			continue;
		}

		// 첫 Start pos가 제대로 안잡혔을 때
		if (i == 0) return false;

		// Set Landing Position
		FVector Start	= HitResult.TraceStart + OwnerCharacter->GetActorForwardVector() * 80.f;
		FVector End		= Start - FVector::UnitZ() * 1000.f; // TODO - 너무 낮은 높이로 LandPos 잡힐수도 있음 -> Test
		HitResult		= {};

		HasHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, CollisionParams);

		if (HasHit)
		{
			CurParkourDesc.WarpLandPos = HitResult.Location;
			LandInited = true;
			break;
		}
	}

	// Init ParkourActionType

	// 무조건 Mantling으로 처리해야할 때
	if (!PossibleToVault)
	{
		CurParkourDesc.CurParkourActionType = (CurParkourDesc.bIsLowAction) ?
											   EParkourActionType::MANTLING_LOW : EParkourActionType::MANTLING_HIGH;
		CurParkourDesc.WarpMiddlePos = MantlingMiddlePos;
		return true;
	}

	// 무조건 Vaulting 처리 해야할 때 - Mantling할 공간이 충분하지 않을 때
	if (DistanceLevel == 0)
	{
		if (!LandInited) return false;

		CurParkourDesc.CurParkourActionType = (CurParkourDesc.bIsLowAction) ?
											   EParkourActionType::VAULTING_LOW : EParkourActionType::VAULTING_HIGH;
		return true;
	}

	// 높이가 높을 때에 DistanceLevel 0 초과이면 무조건 Mantling
	if (!CurParkourDesc.bIsLowAction && DistanceLevel > 0)
	{
		CurParkourDesc.CurParkourActionType = EParkourActionType::MANTLING_HIGH;
		CurParkourDesc.WarpMiddlePos = MantlingMiddlePos;
		return true;
	}

	if (bHasTryVaulting)
	{
		if (!LandInited) return false;
		CurParkourDesc.CurParkourActionType = (CurParkourDesc.bIsLowAction) ?
											   EParkourActionType::VAULTING_LOW : EParkourActionType::VAULTING_HIGH;
	}
	else
	{
		CurParkourDesc.CurParkourActionType = (CurParkourDesc.bIsLowAction) ?
											   EParkourActionType::MANTLING_LOW : EParkourActionType::MANTLING_HIGH;
		CurParkourDesc.WarpMiddlePos = MantlingMiddlePos;
	}

	return true;
}

void UC_ParkourComponent::InitCurParkourActionStrategy(const FParkourDescriptor& CurParkourDesc)
{
}

void UC_ParkourComponent::OnParkourAnimMontageEnd()
{
	UC_Util::Print("OnParkourEnd", FColor::Cyan, 10.f);

	OwnerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	OwnerCharacter->SetActorEnableCollision(true);

	OwnerCharacter->GetMesh()->GetAnimInstance()->StopAllMontages(0);

	bPendingMeshUpdateToMainMesh = true; // 다음 Update Tick에서 Main skeletal mesh로 돌아감
}

