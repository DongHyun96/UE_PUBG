// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/C_ParkourComponent.h"
#include "Character/C_BasicCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "MotionWarpingComponent.h"

#include "Utility/C_Util.h"

UC_ParkourComponent::UC_ParkourComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	//OwnerCharacter->GetMesh()->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &UC_ParkourComponent::OnParkourAnimMontageEnd);
}


void UC_ParkourComponent::BeginPlay()
{
	Super::BeginPlay();

	//OwnerCharacter->GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(
	//	FOnMontageEnded::CreateUObject(this, &UC_ParkourComponent::OnParkourAnimMontageEnd), VaultingMontage.AnimMontage);

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

void UC_ParkourComponent::Vault()
{
	FHitResult HitResult{};
	FCollisionQueryParams CollisionParams{};
	CollisionParams.AddIgnoredActor(OwnerCharacter);
	//TArray<AActor*> AttachedActors{};
	//OwnerCharacter->GetAttachedActors(AttachedActors);
	//CollisionParams.AddIgnoredActors(AttachedActors);

	for (int i = 0; i < 3; i++)
	{
		//GetWorld()->SphereTraceByChannel
		//SphereTraceByChanne

		FVector StartLocation = OwnerCharacter->GetActorLocation();
		StartLocation.Z += i * 30;
		FVector DestLocation = StartLocation + OwnerCharacter->GetActorForwardVector() * 180.f;

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

		//DrawDebugCylinder(GetWorld(), StartLocation, DestLocation, 5.f, 4, FColor::Red, true);
		//DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 5.f, 4, FColor::Yellow, true);

		if (HasHit) break;
	}

	FVector FirstHitResult = HitResult.Location;

	for (int i = 0; i < 6; i++)
	{
		FVector StartLocation = FirstHitResult + FVector::UnitZ() * 100.f + OwnerCharacter->GetActorForwardVector() * (50.f * i);
		FVector DestLocation = StartLocation  - FVector::UnitZ() * 100.f;

		HitResult = {};

		bool HasHit = GetWorld()->SweepSingleByChannel
		(
			HitResult,
			StartLocation,
			DestLocation,
			FQuat::Identity,
			ECC_Visibility,
			FCollisionShape::MakeSphere(10.f),
			CollisionParams
		);

		//DrawDebugCylinder(GetWorld(), StartLocation, DestLocation, 10.f, 4, FColor::Red, true);
		//DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10.f, 4, FColor::Yellow, true);

		if (HasHit)
		{
			if (i == 0) // Start Position
			{
				VaultStartPos = HitResult.Location;
				//DrawDebugSphere(GetWorld(), VaultStartPos, 10.f, 12, FColor::Cyan, false, 10.f);
			}

			// Mid Position
			VaultMiddlePos = HitResult.Location;
			//DrawDebugSphere(GetWorld(), VaultMiddlePos, 10.f, 12, FColor::Yellow, false, 10.f);
			CanWarp = true;

		}
		else // Landing position
		{
			FVector Start = HitResult.TraceStart + OwnerCharacter->GetActorForwardVector() * 80.f;
			FVector End   = Start - FVector::UnitZ() * 1000.f;
			HitResult = {};

			bool Hitted = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, CollisionParams);
			if (Hitted)
			{
				//DrawDebugLine(GetWorld(), Start, HitResult.Location, FColor::Blue, true);
				VaultLandPos = HitResult.Location;
				//DrawDebugSphere(GetWorld(), VaultLandPos, 10.f, 12, FColor::Yellow, true);
				break;
			}
		}
	}

	VaultMotionWarp();
}

void UC_ParkourComponent::VaultMotionWarp()
{
	float LocationZ = OwnerCharacter->GetMesh()->GetComponentLocation().Z;
	if (VaultLandPos.Z < LocationZ - 50.f || VaultLandPos.Z > LocationZ + 50.f) return;
	if (!CanWarp) return;

	UC_Util::Print("MotionWarping");

	DrawDebugSphere(GetWorld(), VaultStartPos, 10.f, 12, FColor::Yellow, true);
	DrawDebugSphere(GetWorld(), VaultMiddlePos, 10.f, 12, FColor::Yellow, true);
	DrawDebugSphere(GetWorld(), VaultLandPos, 10.f, 12, FColor::Yellow, true);


	SwapMesh(true);

	OwnerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);

	OwnerCharacter->SetActorEnableCollision(false);
	UMotionWarpingComponent* MotionWarping = OwnerCharacter->GetMotionWarpingComponent();

	FMotionWarpingTarget Target{};
	Target.Name		= FName(TEXT("VaultStart"));
	Target.Location = VaultStartPos;
	Target.Rotation = OwnerCharacter->GetActorRotation();

	MotionWarping->AddOrUpdateWarpTarget(Target);

	Target = {};
	Target.Name		= FName(TEXT("VaultMiddle"));
	Target.Location = VaultMiddlePos;

	MotionWarping->AddOrUpdateWarpTarget(Target);

	Target = {};
	Target.Name		= FName(TEXT("VaultLand"));
	Target.Location = VaultLandPos;

	MotionWarping->AddOrUpdateWarpTarget(Target);

	OwnerCharacter->PlayAnimMontage(VaultingMontage);

	// AddDynamic 내부적으로 중복 Binding 방지
	//OwnerCharacter->GetMesh()->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &UC_ParkourComponent::OnParkourAnimMontageEnd);
}

void UC_ParkourComponent::OnParkourAnimMontageEnd()
{
	UC_Util::Print("OnParkourEnd", FColor::Cyan, 10.f);

	OwnerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	OwnerCharacter->SetActorEnableCollision(true);
	CanWarp = false;

	OwnerCharacter->GetMesh()->GetAnimInstance()->StopAllMontages(0);

	VaultLandPos.Z = 20000.f;

	bPendingMeshUpdateToMainMesh = true; // 다음 Update Tick에서 Main skeletal mesh로 돌아감
}

