// Fill out your copyright notice in the Description page of Project Settings.



#include "Item/Weapon/ThrowingWeapon/C_GrenadeExplode.h"

#include "Kismet/GameplayStatics.h"
#include "Item/Weapon/ThrowingWeapon/C_ThrowingWeapon.h"
#include "Character/C_BasicCharacter.h"
#include "Components/ShapeComponent.h"
#include "Utility/C_Util.h"

#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "PhysicsEngine/BodyInstance.h"
#include "PhysicsEngine/BodySetup.h"
#include "Components/PrimitiveComponent.h"

#include "Chaos/PhysicsObject.h"

#include "Components/SphereComponent.h"

AC_GrenadeExplode::AC_GrenadeExplode()
{
	BodyPartsDamageRate =
	{
		{"Neck",		0.9f},

		{"Hips",		0.5f},

		{"LeftUpLeg",	0.2f},
		{"LeftFoot",	0.1f},
		{"RightUpLeg",	0.2f},
		{"RightFoot",	0.1f},

		{"Spine",		0.7f},
		{"Spine1",		0.7f},
		{"Spine2",		0.7f},

		{"LeftArm",		0.2f},
		{"LeftHand",	0.1f},

		{"RightArm",	0.2f},
		{"RightHand",	0.1f}
	};

	LineTraceDestBoneNames =
	{
		"Head",				// �Ӹ�

		"Spine1",			// ����

		"LeftForeArm",		// �� ��
		"LeftHand",
		"RightForeArm",
		"RightHand",

		"Hips",				// ��Ÿ����

		"LeftLeg",			// �ٸ� ��
		"LeftFoot",
		"RightLeg",
		"RightFoot"
	};
}

bool AC_GrenadeExplode::UseStrategy(AC_ThrowingWeapon* ThrowingWeapon)
{
	// TODO : ���� vfx, sfx / 
	// ���� �ݰ� ���� ĳ���Ͱ� �����Ѵٸ� ray cast -> ray cast hit -> damage �ֱ�
	// �ݰ� ���� ĳ���Ͱ� �����ϸ� aim punching & �÷��̾��� ��� camera shake -> �Ÿ��� Camera Shake ���� ����

	if (ThrowingWeapon->GetParticleExplodeEffect())
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ThrowingWeapon->GetParticleExplodeEffect(), ThrowingWeapon->GetActorLocation());

	FVector ExplosionLocation = ThrowingWeapon->GetActorLocation();

	USphereComponent* ExplosionSphere = Cast<USphereComponent>(ThrowingWeapon->GetExplosionSphere());
	if (!IsValid(ExplosionSphere)) return false;

	ExplosionSphere->SetWorldLocation(ExplosionLocation);

	TArray<AActor*> OverlappingActors{};
	ExplosionSphere->GetOverlappingActors(OverlappingActors, TSubclassOf<AC_BasicCharacter>());

	if (OverlappingActors.IsEmpty()) return false;

	for (AActor* Actor : OverlappingActors)
	{
		AC_BasicCharacter* Character = Cast<AC_BasicCharacter>(Actor);
		if (!IsValid(Character)) continue;
		
		UPhysicsAsset* PhysicsAsset = Character->GetMesh()->GetPhysicsAsset();
		if (!IsValid(PhysicsAsset)) continue;

		const TArray<USkeletalBodySetup*>& BodySetups = PhysicsAsset->SkeletalBodySetups;

		FHitResult HitResult{};
		TArray<AActor*> IgnoreActors{};
		IgnoreActors.Add(ThrowingWeapon);

		int count{};
		float TotalDamage{};
		bool Hitted{};

		// �� �ǰ� ������ Ray casting �õ�
		for (FName& DestBoneName : LineTraceDestBoneNames)
		{
			FVector DestLocation	= Character->GetMesh()->GetBoneLocation(DestBoneName);

			FCollisionQueryParams CollisionParams{};
			CollisionParams.AddIgnoredActor(ThrowingWeapon);

			HitResult = {};

			bool HasHit = GetWorld()->LineTraceSingleByChannel(HitResult, ExplosionLocation, DestLocation, ECC_Visibility, CollisionParams);

			if (!HasHit || HitResult.GetActor() != Character)		continue;
			if (!BodyPartsDamageRate.Contains(HitResult.BoneName))	continue;

			DrawDebugLine(GetWorld(), ExplosionLocation, HitResult.ImpactPoint, FColor::Red, true);

			// Apply Damage to character

			float ExplosionRad = ExplosionSphere->GetScaledSphereRadius();
			float DamageAmount = DAMAGE_BASE * (ExplosionRad - HitResult.Distance) / ExplosionRad; // �Ÿ� ��� Damage base
			DamageAmount *= BodyPartsDamageRate[HitResult.BoneName]; // ��ü������ ������ ���� ����
			
			TotalDamage += Character->TakeDamage(DamageAmount, HitResult.BoneName, ThrowingWeapon);

			UC_Util::Print("Hitted Bone : " + HitResult.BoneName.ToString(), FColor::Red, 5.f);
			UC_Util::Print("Bone Damaged : " + FString::SanitizeFloat(DamageAmount), FColor::Red, 5.f);

			count++;
			Hitted = true;
		}

		FString Str = "Total Damage : " + FString::SanitizeFloat(TotalDamage);
		UC_Util::Print(Str, FColor::Red, 5.f);

		if (Hitted)
		{
			// Aim Punching �ֱ�

		}
		
		//FString Str2 = "Total Damage parts : " + FString::FromInt(count);
		//UC_Util::Print(Str2, FColor::Red, 5.f);
	}
	return true;
}

//bool HasHit = UKismetSystemLibrary::LineTraceSingle
//(
//	GetWorld(),
//	Start, End,
//	ETraceTypeQuery::TraceTypeQuery4,
//	true,
//	IgnoreActors,
//	//EDrawDebugTrace::Type::None,
//	EDrawDebugTrace::Type::ForDuration,
//	HitResult,
//	true
//);

/*
ECC_WorldStatic UMETA(DisplayName="WorldStatic"),
ECC_WorldDynamic UMETA(DisplayName="WorldDynamic"),
ECC_Pawn UMETA(DisplayName="Pawn"),
ECC_Visibility UMETA(DisplayName="Visibility" , TraceQuery="1"),
ECC_Camera UMETA(DisplayName="Camera" , TraceQuery="1"),
ECC_PhysicsBody UMETA(DisplayName="PhysicsBody"),
ECC_Vehicle UMETA(DisplayName="Vehicle"),
ECC_Destructible UMETA(DisplayName="Destructible"),
*/