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
		"Head",				// 머리

		"Spine1",			// 몸통

		"LeftForeArm",		// 팔 쪽
		"LeftHand",
		"RightForeArm",
		"RightHand",

		"Hips",				// 사타구니

		"LeftLeg",			// 다리 쪽
		"LeftFoot",
		"RightLeg",
		"RightFoot"
	};
}

bool AC_GrenadeExplode::UseStrategy(AC_ThrowingWeapon* ThrowingWeapon)
{
	// TODO : 폭파 vfx, sfx / 
	// 폭발 반경 내에 캐릭터가 존재한다면 ray cast -> ray cast hit -> damage 주기
	// 반경 내에 캐릭터가 존재하면 aim punching & 플레이어의 경우 camera shake -> 거리별 Camera Shake 강도 조절

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

		// 각 피격 부위에 Ray casting 시도
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
			float DamageAmount = DAMAGE_BASE * (ExplosionRad - HitResult.Distance) / ExplosionRad; // 거리 비례 Damage base
			DamageAmount *= BodyPartsDamageRate[HitResult.BoneName]; // 신체부위별 데미지 감소 적용
			
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
			// Aim Punching 주기

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