// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/ThrowingWeapon/C_FlashBangExplode.h"

#include "AI/C_EnemyAIController.h"
#include "Character/C_Enemy.h"
#include "Item/Weapon/ThrowingWeapon/C_ThrowingWeapon.h"
#include "Kismet/GameplayStatics.h"

#include "Components/ShapeComponent.h"
#include "Components/SphereComponent.h"


#include "Utility/C_Util.h"

#include "Character/C_Player.h"
#include "Character/Component/C_CameraEffectComponent.h"
#include "Engine/OverlapResult.h"
#include "LevelInstance/LevelInstanceTypes.h"

#include "PhysicsEngine/PhysicsAsset.h"
#include "Singleton/C_GameSceneManager.h"

const float AC_FlashBangExplode::EFFECT_DURATION_MAX = 6.f;

bool AC_FlashBangExplode::UseStrategy(AC_ThrowingWeapon* ThrowingWeapon)
{
	if (ThrowingWeapon->GetParticleExplodeEffect())
		UGameplayStatics::SpawnEmitterAtLocation(ThrowingWeapon->GetWorld(), ThrowingWeapon->GetParticleExplodeEffect(), ThrowingWeapon->GetActorLocation());
	
	const FVector ExplosionLocation = ThrowingWeapon->GetActorLocation();
	const float   ExplosionRad      = ThrowingWeapon->GetExplosionSphereRadius();

	FCollisionShape ExplosionSphere = FCollisionShape::MakeSphere(ExplosionRad);
	FCollisionQueryParams ExplosionSphereQueryParams{};
	ExplosionSphereQueryParams.AddIgnoredActor(ThrowingWeapon); // 투척류 자신은 무시
	
	// Overlap Actor 조사 -> Overlap된 Character만 간추려서 피격 판정 처리
	TArray<FOverlapResult> OverlapResults{};
	bool bHit = ThrowingWeapon->GetWorld()->OverlapMultiByObjectType
	(
		OverlapResults,
		ExplosionLocation,
		FQuat::Identity,
		FCollisionObjectQueryParams(ECC_Pawn),
		ExplosionSphere,
		ExplosionSphereQueryParams
	);
	// DrawDebugSphere(ThrowingWeapon->GetWorld(), ExplosionLocation, ExplosionRad, 30, FColor::MakeRandomColor(), true);

	// Overlapped된 Actor가 없음
	if (!bHit) return false;

	TSet<AC_BasicCharacter*> OverlappedCharacters{};


	// 정확한 피격 판정을 위해 모든 캐릭터의 Physics Asset Collider 꺼두고 조사할 피격 부위만 조사할 때 켜두기
	for (const FOverlapResult& OverlapResult : OverlapResults)
	{
		AC_BasicCharacter* Character = Cast<AC_BasicCharacter>(OverlapResult.GetActor());
		if (!IsValid(Character)) continue;

		UPhysicsAsset* PhysicsAsset = Character->GetMesh()->GetPhysicsAsset();
		if (!IsValid(PhysicsAsset)) continue;

		if (Character->GetMainState() == EMainState::DEAD) continue;

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
	
	UC_Util::Print("Overlapped Character Count : " + FString::FromInt(OverlappedCharacters.Num()), FColor::MakeRandomColor(), 10.f);

	for (AC_BasicCharacter* Character : OverlappedCharacters)
	{
		// 머리 쪽에만 LineTrace 시도해서 맞는다면, 섬광탄 효과 발동 시도
		FHitResult				HitResult{};
		FVector					DestLocation = Character->GetMesh()->GetBoneLocation("Head");
		FCollisionQueryParams	CollisionParams{};
		CollisionParams.AddIgnoredActor(ThrowingWeapon);

		// 머리 부위 Collider만 켜기
		SetPhysicsAssetColliderEnabled(Character, "Neck", true);

		bool HasHit = ThrowingWeapon->GetWorld()->LineTraceSingleByChannel(HitResult, ExplosionLocation, DestLocation, ECC_Visibility, CollisionParams);

		if (!HasHit || HitResult.GetActor() != Character || HitResult.BoneName != "Neck")
		{
			SetPhysicsAssetColliderEnabled(Character, "Neck", false); // 머리 부위 Collider 다시 끄기
			continue;
		}

		DrawDebugLine(ThrowingWeapon->GetWorld(), ExplosionLocation, HitResult.ImpactPoint, FColor::Red, true);

		// Apply FlashBang effect to character
		ExecuteExplosionEffectToCharacter(Character, ExplosionLocation, ExplosionRad);

		// 머리 부위 Collider 다시 끄기 (뒤에 있는 캐릭터의 중첩을 피하기 위함)
		SetPhysicsAssetColliderEnabled(Character, "Neck", false);
	}

	// 모든 캐릭터의 Physics Asset 다시 켜두기
	for (AC_BasicCharacter* Character : OverlappedCharacters)
		SetPhysicsAssetCollidersEnabled(Character, true);
	
	ThrowingWeapon->SetActorHiddenInGame(true);
	FTimerHandle& TimerHandle = UC_GameSceneManager::GetInstance(ThrowingWeapon->GetWorld())->GetTimerHandle();
	ThrowingWeapon->GetWorld()->GetTimerManager().SetTimer(TimerHandle, [ThrowingWeapon]()
	{
		ThrowingWeapon->DestroyItem();
	}, 10.f, false);

	return true;
}

void AC_FlashBangExplode::SetPhysicsAssetCollidersEnabled(AC_BasicCharacter* Character, bool Enabled)
{
	ECollisionEnabled::Type CollisionType = Enabled ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision;

	UPhysicsAsset* PhysicsAsset = Character->GetMesh()->GetPhysicsAsset();

	for (USkeletalBodySetup* BodySetup : PhysicsAsset->SkeletalBodySetups)
		BodySetup->AggGeom.SphylElems[0].SetCollisionEnabled(CollisionType);
		//BodySetup->InvalidatePhysicsData(); // Mark the body setup as modified.

	Character->GetMesh()->RecreatePhysicsState();
}

bool AC_FlashBangExplode::SetPhysicsAssetColliderEnabled(AC_BasicCharacter* Character, FName BoneName, bool Enabled)
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

void AC_FlashBangExplode::ExecuteExplosionEffectToCharacter(AC_BasicCharacter* Character, FVector ExplosionLocation, float ExplosionRad)
{
	float Distance					= FVector::Distance(Character->GetActorLocation(), ExplosionLocation);
	float DistanceRateFactor		= (ExplosionRad - Distance) / ExplosionRad;

	FVector CharacterToExplosion	= ExplosionLocation - Character->GetActorLocation();
	CharacterToExplosion.Normalize();
	float Dot						= FVector::DotProduct(Character->GetActorForwardVector(), CharacterToExplosion);
	float Rad						= FMath::Acos(Dot);
	float Degree					= FMath::RadiansToDegrees(Rad); // 캐릭터 forward, 폭발 위치 사이의 각도 (90도 이상이면 뒷편)

	// 캐릭터가 현재 바라보고 있는 방향과 거리 등을 따져서 섬광탄 effect 남기기

	float EffectDuration = EFFECT_DURATION_MAX;

	// 일정거리에서 멀어져야 Duration감소를 시키고, 섬광탄을 바라보는 방향에 따른 감소까지 적용시킴
	if (DistanceRateFactor < 0.7f) EffectDuration *= (Degree <= 110.f) ? DistanceRateFactor : DistanceRateFactor * 0.5f;

	// Player일 경우
	if (AC_Player* Player = Cast<AC_Player>(Character))
		Player->GetCameraEffectComponent()->ExecuteFlashBangEffect(EffectDuration);

	// Enemy일 경우
	if (AC_Enemy* Enemy = Cast<AC_Enemy>(Character))
		Enemy->GetController<AC_EnemyAIController>()->SetFlashBangEffectLeftTime(EffectDuration);

}


