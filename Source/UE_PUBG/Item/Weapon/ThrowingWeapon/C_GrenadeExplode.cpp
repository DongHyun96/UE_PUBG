// Fill out your copyright notice in the Description page of Project Settings.



#include "Item/Weapon/ThrowingWeapon/C_GrenadeExplode.h"
#include "Item/Weapon/ThrowingWeapon/C_ThrowingWeapon.h"

#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"
#include "Character/Component/C_StatComponent.h"
#include "Character/Component/C_CameraEffectComponent.h"
#include "Character/Component/C_InvenComponent.h"

#include "Kismet/GameplayStatics.h"

#include "Components/ShapeComponent.h"
#include "Components/SphereComponent.h"
#include "Item/Equipment/C_EquipableItem.h"

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

const float AC_GrenadeExplode::DAMAGE_BASE = 50.f;

AC_GrenadeExplode::AC_GrenadeExplode()
{
}

bool AC_GrenadeExplode::UseStrategy(AC_ThrowingWeapon* ThrowingWeapon)
{
	// TODO : 폭파 sfx / 
	// 폭발 반경 내에 캐릭터가 존재한다면 ray cast -> ray cast hit -> damage 주기
	// 반경 내에 캐릭터가 존재하면 aim punching & 플레이어의 경우 camera shake -> 거리별 Camera Shake 강도 조절

	// 수류탄 터지는 VFX
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

	// 정확한 피격 판정을 위해 모든 캐릭터의 Physics Asset Colliders 꺼두고 조사할 피격 부위만 조사할 때 켜두기
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

	for (AC_BasicCharacter* Character : OverlappedCharacters)
	{
		// 캐릭터에게 Damage 입히기 시도 -> 성공했다면 폭발 effect 캐릭터에 적용시키기(ex 카메라 aim punching)
		if (TryDamagingCharacter(Character, ThrowingWeapon, ExplosionSphere))
			ExecuteExplosionEffectToCharacter(Character, ExplosionLocation, ExplosionRad);
	}

	// 모든 캐릭터의 Physics Asset Colliders 다시 켜두기
	for (AC_BasicCharacter* Character : OverlappedCharacters)
		SetPhysicsAssetCollidersEnabled(Character, true);
	//SetPhysicsAssetCollidersEnabled(Character->GetMesh()->GetPhysicsAsset(), true);

	ThrowingWeapon->SetActorHiddenInGame(true);
	
	ThrowingWeapon->GetWorld()->GetTimerManager().SetTimer(ThrowingWeapon->GetDestroyTimerHandle(), [ThrowingWeapon]()
	{
		ThrowingWeapon->Destroy();
	}, 10.f, false);
	
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

bool AC_GrenadeExplode::TryDamagingCharacter(AC_BasicCharacter* Character, AC_ThrowingWeapon* ThrowingWeapon, USphereComponent* ExplosionSphere)
{
	FHitResult	HitResult{};
	int			HitCount{};
	float		TotalDamage{};
	bool		Hitted{};

	FVector ExplosionLocation = ThrowingWeapon->GetActorLocation();
	float   ExplosionRad = ExplosionSphere->GetScaledSphereRadius();

	// 각 피격 부위에 Ray casting 시도
	for (const FName& DestBoneName : LineTraceDestBoneNames)
	{
		// 해당 부위만 Collider 켜놓기
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

		// Collision 만족 x
		if (!HasHit || HitResult.GetActor() != Character || !BodyPartsDamageRate.Contains(HitResult.BoneName))
		{
			// 해당 부위 Collider 다시 끄기
			SetPhysicsAssetColliderEnabled(Character, ColliderBoneName, false);
			continue;
		}

		DrawDebugLine(ThrowingWeapon->GetWorld(), ExplosionLocation, HitResult.ImpactPoint, FColor::Red, true);

		// Apply Damage to character
		float DamageAmount = DAMAGE_BASE * (ExplosionRad - HitResult.Distance) / ExplosionRad; // 거리 비례 Damage base
		DamageAmount *= BodyPartsDamageRate[HitResult.BoneName]; // 신체부위별 데미지 감소 적용

		float DamageDistance = FVector::Distance(ThrowingWeapon->GetOwnerCharacter()->GetActorLocation(), Character->GetActorLocation());
		DamageDistance *= 0.01f;
		
		// Vest의 Damage는 예외적으로 Damage 총량을 모두 더하여 주기
		FKillFeedDescriptor KillFeedDescriptor =
		{
			EDamageType::Weapon,
			ThrowingWeapon->GetOwnerCharacter(),
			Character,
			ThrowingWeapon,
			false,
			static_cast<int>(DamageDistance)
		};
		
		TotalDamage += Character->GetStatComponent()->TakeDamage(DamageAmount, HitResult.BoneName, KillFeedDescriptor, false);

		//UC_Util::Print("Hitted Bone : " + HitResult.BoneName.ToString(), FColor::Red, 5.f);
		//UC_Util::Print("Bone Damaged : " + FString::SanitizeFloat(DamageAmount), FColor::Red, 5.f);

		HitCount++;
		Hitted = true;

		// 해당 부위 Collider 다시 끄기 (뒤에 있을 캐릭터의 중첩을 피하기 위함)
		SetPhysicsAssetColliderEnabled(Character, ColliderBoneName, false);
	}

	UC_Util::Print("Grenade Total Damage : " + FString::SanitizeFloat(TotalDamage), FColor::Red, 5.f);

	// Vest의 Damage는 따로 주기
	if (AC_EquipableItem* Vest = Character->GetInvenComponent()->GetEquipmentItems()[EEquipSlot::VEST])
		Vest->TakeDamage(TotalDamage);
	
	return Hitted;
}

void AC_GrenadeExplode::ExecuteExplosionEffectToCharacter(AC_BasicCharacter* Character, FVector ExplosionLocation, float ExplosionRad)
{
	if (AC_Player* Player = Cast<AC_Player>(Character))
	{
		// TODO : 현재 ADS 상태이면 실행x

		// 거리 및 방향 계산
		FVector PlayerToExplode = ExplosionLocation - Player->GetActorLocation();
		PlayerToExplode.Normalize();

		FVector Cross = FVector::CrossProduct(Player->GetActorForwardVector(), PlayerToExplode);

		// Z값 왼쪽이 음수, 오른쪽이 양수
		FVector PunchingDirection = (Cross.Z > 0.f) ? FVector(0, -1, 0) : FVector(0, 1, 0);

		float Distance = FVector::Distance(Player->GetActorLocation(), ExplosionLocation);
		float DistanceRateFactor = (ExplosionRad - Distance) / ExplosionRad;

		float PunchingIntensity = 100.f * DistanceRateFactor;
		float XRotDelta = (FMath::RandRange(0, 10) % 2 == 0) ? 35.f * DistanceRateFactor : -35.f * DistanceRateFactor;
		float CamShakeScale = 1.f * DistanceRateFactor;

		//Player->ExecuteCameraAimPunching({ 0.f, 1.f, 0.f }, 100.f, 35.f); // Maximum 값
		//Player->ExecuteCameraShake(1.f);
		Player->GetCameraEffectComponent()->ExecuteCameraAimPunching(PunchingDirection, PunchingIntensity, XRotDelta);
		Player->GetCameraEffectComponent()->ExecuteCameraShake(CamShakeScale);
	}

	// TODO : Enemy AI 또한 방해주기
}


