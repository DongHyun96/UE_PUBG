// Fill out your copyright notice in the Description page of Project Settings.



#include "Item/Weapon/ThrowingWeapon/C_GrenadeExplode.h"
#include "Item/Weapon/ThrowingWeapon/C_ThrowingWeapon.h"

#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"
#include "Character/Component/C_StatComponent.h"
#include "Character/Component/C_CameraEffectComponent.h"

#include "Kismet/GameplayStatics.h"

#include "Components/ShapeComponent.h"
#include "Components/SphereComponent.h"

#include "Utility/C_Util.h"

#include "PhysicsEngine/PhysicsAsset.h"

const TMap<FName, float> AC_GrenadeExplode::BodyPartsDamageRate =
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

const TArray<FName> AC_GrenadeExplode::LineTraceDestBoneNames =
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

const float AC_GrenadeExplode::DAMAGE_BASE = 50.f;

AC_GrenadeExplode::AC_GrenadeExplode()
{
}

bool AC_GrenadeExplode::UseStrategy(AC_ThrowingWeapon* ThrowingWeapon)
{
	// TODO : ���� sfx / 
	// ���� �ݰ� ���� ĳ���Ͱ� �����Ѵٸ� ray cast -> ray cast hit -> damage �ֱ�
	// �ݰ� ���� ĳ���Ͱ� �����ϸ� aim punching & �÷��̾��� ��� camera shake -> �Ÿ��� Camera Shake ���� ����

	// ����ź ������ VFX
	if (ThrowingWeapon->GetParticleExplodeEffect())
		UGameplayStatics::SpawnEmitterAtLocation(ThrowingWeapon->GetWorld(), ThrowingWeapon->GetParticleExplodeEffect(), ThrowingWeapon->GetActorLocation());

	FVector ExplosionLocation = ThrowingWeapon->GetActorLocation();

	USphereComponent* ExplosionSphere = Cast<USphereComponent>(ThrowingWeapon->GetExplosionSphere());

	if (!IsValid(ExplosionSphere))
	{
		UC_Util::Print("From AC_GrenadeExplode::UseStrategy : Explosion Sphere casting failed!", FColor::Red, 5.f);
		return false;
	}

	ExplosionSphere->SetWorldLocation(ExplosionLocation);
	float ExplosionRad = ExplosionSphere->GetScaledSphereRadius();

	TArray<AActor*>				OverlappingActors{};
	TArray<AC_BasicCharacter*>	OverlappedCharacters{};

	ExplosionSphere->GetOverlappingActors(OverlappingActors, TSubclassOf<AC_BasicCharacter>());

	// ��Ȯ�� �ǰ� ������ ���� ��� ĳ������ Physics Asset Colliders ���ΰ� ������ �ǰ� ������ ������ �� �ѵα�
	for (AActor* Actor : OverlappingActors)
	{
		AC_BasicCharacter* Character = Cast<AC_BasicCharacter>(Actor);
		if (!IsValid(Character)) continue;

		UPhysicsAsset* PhysicsAsset = Character->GetMesh()->GetPhysicsAsset();
		if (!IsValid(PhysicsAsset)) continue;

		OverlappedCharacters.Add(Character);

		//SetPhysicsAssetCollidersEnabled(Character->GetMesh()->GetPhysicsAsset(), false);
		SetPhysicsAssetCollidersEnabled(Character, false);

		// Testing Debug Message
		/*for (USkeletalBodySetup* BodySetup : Character->GetMesh()->GetPhysicsAsset()->SkeletalBodySetups)
		{
			if (BodySetup->AggGeom.SphylElems[0].GetCollisionEnabled() == ECollisionEnabled::NoCollision)
				UC_Util::Print("NoCollision!", FColor::Cyan, 10.f);
		}*/
	}

	int DamagedCharacterCnt{};

	for (AC_BasicCharacter* Character : OverlappedCharacters)
	{
		// ĳ���Ϳ��� Damage ������ �õ� -> �����ߴٸ� ���� effect ĳ���Ϳ� �����Ű��(ex ī�޶� aim punching)
		if (TryDamagingCharacter(Character, ThrowingWeapon, ExplosionSphere))
		{
			ExecuteExplosionEffectToCharacter(Character, ExplosionLocation, ExplosionRad);
			DamagedCharacterCnt++;
		}
	}

	//UC_Util::Print("DamagedCharacter Cnt : " + FString::FromInt(DamagedCharacterCnt), FColor::Cyan, 10.f);

	// ��� ĳ������ Physics Asset Colliders �ٽ� �ѵα�
	for (AC_BasicCharacter* Character : OverlappedCharacters)
		SetPhysicsAssetCollidersEnabled(Character, true);
	//SetPhysicsAssetCollidersEnabled(Character->GetMesh()->GetPhysicsAsset(), true);

	return true;
}

void AC_GrenadeExplode::SetPhysicsAssetCollidersEnabled(AC_BasicCharacter* Character, bool Enabled)
{
	ECollisionEnabled::Type CollisionType = Enabled ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision;

	UPhysicsAsset* PhysicsAsset = Character->GetMesh()->GetPhysicsAsset();

	for (USkeletalBodySetup* BodySetup : PhysicsAsset->SkeletalBodySetups)
		BodySetup->AggGeom.SphylElems[0].SetCollisionEnabled(CollisionType);
	//BodySetup->InvalidatePhysicsData(); // Mark the body setup as modified.

	Character->GetMesh()->RecreatePhysicsState();
}

bool AC_GrenadeExplode::SetPhysicsAssetColliderEnabled(AC_BasicCharacter* Character, FName BoneName, bool Enabled)
{
	ECollisionEnabled::Type CollisionType = Enabled ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision;

	UPhysicsAsset* PhysicsAsset = Character->GetMesh()->GetPhysicsAsset();

	for (USkeletalBodySetup* BodySetup : PhysicsAsset->SkeletalBodySetups)
	{
		if (BodySetup->BoneName == BoneName)
		{
			BodySetup->AggGeom.SphylElems[0].SetCollisionEnabled(CollisionType);
			//BodySetup->InvalidatePhysicsData(); // Mark the body setup as modified.
			Character->GetMesh()->RecreatePhysicsState();
			return true;
		}
	}
	return false;
}

//void AC_GrenadeExplode::SetPhysicsAssetCollidersEnabled(UPhysicsAsset* PhysicsAsset, bool Enabled)
//{
//	ECollisionEnabled::Type CollisionType = Enabled ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision;
//
//	for (USkeletalBodySetup* BodySetup : PhysicsAsset->SkeletalBodySetups)
//	{
//		//FKAggregateGeom&	AggGeom  = BodySetup->AggGeom;
//		//FKSphylElem&		Collider = AggGeom.SphylElems[0];
//		//
//		//Collider.SetCollisionEnabled(CollisionType);
//
//		BodySetup->AggGeom.SphylElems[0].SetCollisionEnabled(CollisionType);
//		//BodySetup->InvalidatePhysicsData(); // Mark the body setup as modified.
//	}
//}
//
//bool AC_GrenadeExplode::SetPhysicsAssetColliderEnabled(UPhysicsAsset* PhysicsAsset, FName BoneName, bool Enabled)
//{
//	ECollisionEnabled::Type CollisionType = Enabled ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision;
//
//	for (USkeletalBodySetup* BodySetup : PhysicsAsset->SkeletalBodySetups)
//	{
//		if (BodySetup->BoneName == BoneName)
//		{
//			//FKAggregateGeom&	AggGeom = BodySetup->AggGeom;
//			//FKSphylElem&		Collider = AggGeom.SphylElems[0];
//			//
//			//Collider.SetCollisionEnabled(CollisionType);
//
//			BodySetup->AggGeom.SphylElems[0].SetCollisionEnabled(CollisionType);
//			//BodySetup->InvalidatePhysicsData(); // Mark the body setup as modified.
//			return true;
//		}
//	}
//	return false;
//}

bool AC_GrenadeExplode::TryDamagingCharacter(AC_BasicCharacter* Character, AC_ThrowingWeapon* ThrowingWeapon, USphereComponent* ExplosionSphere)
{
	FHitResult	HitResult{};
	int			HitCount{};
	float		TotalDamage{};
	bool		Hitted{};

	FVector ExplosionLocation = ThrowingWeapon->GetActorLocation();
	float   ExplosionRad = ExplosionSphere->GetScaledSphereRadius();

	// �� �ǰ� ������ Ray casting �õ�
	for (const FName& DestBoneName : LineTraceDestBoneNames)
	{
		// �ش� ������ Collider �ѳ���
		FName ColliderBoneName = (DestBoneName == "Head")			? "Neck" :
								 (DestBoneName == "LeftForeArm")	? "LeftArm" :
								 (DestBoneName == "RightForeArm")	? "RightArm" :
								 (DestBoneName == "LeftLeg")		? "LeftUpLeg" :
								 (DestBoneName == "RightLeg")		? "RightUpLeg" : DestBoneName;

		//SetPhysicsAssetColliderEnabled(Character->GetMesh()->GetPhysicsAsset(), ColliderBoneName, true);
		SetPhysicsAssetColliderEnabled(Character, ColliderBoneName, true);


		FVector DestLocation = Character->GetMesh()->GetBoneLocation(DestBoneName);

		FCollisionQueryParams CollisionParams{};
		CollisionParams.AddIgnoredActor(ThrowingWeapon);

		HitResult = {};

		bool HasHit = ThrowingWeapon->GetWorld()->LineTraceSingleByChannel(HitResult, ExplosionLocation, DestLocation, ECC_Visibility, CollisionParams);

		// Collision ���� x
		if (!HasHit || HitResult.GetActor() != Character || !BodyPartsDamageRate.Contains(HitResult.BoneName))
		{
			// �ش� ���� Collider �ٽ� ����
			SetPhysicsAssetColliderEnabled(Character, ColliderBoneName, false);
			continue;
		}

		DrawDebugLine(ThrowingWeapon->GetWorld(), ExplosionLocation, HitResult.ImpactPoint, FColor::Red, true);

		// Apply Damage to character
		float DamageAmount = DAMAGE_BASE * (ExplosionRad - HitResult.Distance) / ExplosionRad; // �Ÿ� ��� Damage base
		DamageAmount *= BodyPartsDamageRate[HitResult.BoneName]; // ��ü������ ������ ���� ����

		TotalDamage += Character->GetStatComponent()->TakeDamage(DamageAmount, HitResult.BoneName, ThrowingWeapon);

		//UC_Util::Print("Hitted Bone : " + HitResult.BoneName.ToString(), FColor::Red, 5.f);
		//UC_Util::Print("Bone Damaged : " + FString::SanitizeFloat(DamageAmount), FColor::Red, 5.f);

		HitCount++;
		Hitted = true;

		// �ش� ���� Collider �ٽ� ���� (�ڿ� ���� ĳ������ ��ø�� ���ϱ� ����)
		SetPhysicsAssetColliderEnabled(Character, ColliderBoneName, false);
	}

	UC_Util::Print("Total Damage : " + FString::SanitizeFloat(TotalDamage), FColor::Red, 5.f);
	//UC_Util::Print("Total Damage part count : " + FString::FromInt(HitCount), FColor::Red, 5.f);

	return Hitted;
}

void AC_GrenadeExplode::ExecuteExplosionEffectToCharacter(AC_BasicCharacter* Character, FVector ExplosionLocation, float ExplosionRad)
{
	AC_Player* Player = Cast<AC_Player>(Character);

	if (IsValid(Player))
	{
		// TODO : ���� ADS �����̸� ����x

		// �Ÿ� �� ���� ���
		FVector PlayerToExplode = ExplosionLocation - Player->GetActorLocation();
		PlayerToExplode.Normalize();

		FVector Cross = FVector::CrossProduct(Player->GetActorForwardVector(), PlayerToExplode);

		// Z�� ������ ����, �������� ���
		FVector PunchingDirection = (Cross.Z > 0.f) ? FVector(0, -1, 0) : FVector(0, 1, 0);

		float Distance = FVector::Distance(Player->GetActorLocation(), ExplosionLocation);
		float DistanceRateFactor = (ExplosionRad - Distance) / ExplosionRad;

		float PunchingIntensity = 100.f * DistanceRateFactor;
		float XRotDelta = (FMath::RandRange(0, 10) % 2 == 0) ? 35.f * DistanceRateFactor : -35.f * DistanceRateFactor;
		float CamShakeScale = 1.f * DistanceRateFactor;

		//Player->ExecuteCameraAimPunching({ 0.f, 1.f, 0.f }, 100.f, 35.f); // Maximum ��
		//Player->ExecuteCameraShake(1.f);
		Player->GetCameraEffectComponent()->ExecuteCameraAimPunching(PunchingDirection, PunchingIntensity, XRotDelta);
		Player->GetCameraEffectComponent()->ExecuteCameraShake(CamShakeScale);
	}
	// TODO : Enemy AI ���� �����ֱ�
}
