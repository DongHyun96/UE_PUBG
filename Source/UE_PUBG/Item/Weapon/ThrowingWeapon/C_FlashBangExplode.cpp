// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/ThrowingWeapon/C_FlashBangExplode.h"

#include "Item/Weapon/ThrowingWeapon/C_ThrowingWeapon.h"
#include "Kismet/GameplayStatics.h"

#include "Components/ShapeComponent.h"
#include "Components/SphereComponent.h"

#include "Utility/C_Util.h"

#include "Character/C_Player.h"

#include "PhysicsEngine/PhysicsAsset.h"


void AC_FlashBangExplode::BeginPlay()
{
	Super::BeginPlay();
}


bool AC_FlashBangExplode::UseStrategy(AC_ThrowingWeapon* ThrowingWeapon)
{
	// TODO : 반경에 있는 캐릭터 시야에 따른 시각 장애 주기
	// 청각장애는 모두 동일 

	if (ThrowingWeapon->GetParticleExplodeEffect())
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ThrowingWeapon->GetParticleExplodeEffect(), ThrowingWeapon->GetActorLocation());
	
	FVector ExplosionLocation = ThrowingWeapon->GetActorLocation();

	USphereComponent* ExplosionSphere = Cast<USphereComponent>(ThrowingWeapon->GetExplosionSphere());
	if (!IsValid(ExplosionSphere))
	{
		UC_Util::Print("From AC_FlashBangExplode::UseStrategy : Explosion Sphere casting failed!", FColor::Red, 5.f);
		return false;
	}

	ExplosionSphere->SetWorldLocation(ExplosionLocation);
	float ExplosionRad = ExplosionSphere->GetScaledSphereRadius();

	TArray<AActor*> OverlappingActors{};
	ExplosionSphere->GetOverlappingActors(OverlappingActors, TSubclassOf<AC_BasicCharacter>());

	if (OverlappingActors.IsEmpty()) return true;

	for (AActor* Actor : OverlappingActors)
	{
		AC_BasicCharacter* Character = Cast<AC_BasicCharacter>(Actor);
		if (!IsValid(Character)) continue;

		UPhysicsAsset* PhysicsAsset = Character->GetMesh()->GetPhysicsAsset();
		if (!IsValid(PhysicsAsset)) continue;

		// 머리 쪽에만 LineTrace 시도해서 맞는다면, 섬광탄 효과 발동 시도
		FHitResult				HitResult{};
		FVector					DestLocation = Character->GetMesh()->GetBoneLocation("Head");
		FCollisionQueryParams	CollisionParams{};
		CollisionParams.AddIgnoredActor(ThrowingWeapon);

		bool HasHit = GetWorld()->LineTraceSingleByChannel(HitResult, ExplosionLocation, DestLocation, ECC_Visibility, CollisionParams);

		if (!HasHit || HitResult.GetActor() != Character)	continue;
		if (HitResult.BoneName != "Neck")					continue;

		DrawDebugLine(GetWorld(), ExplosionLocation, HitResult.ImpactPoint, FColor::Red, true);

		// Apply FlashBang effect to character

		float Distance					= FVector::Distance(Character->GetActorLocation(), ExplosionLocation);
		float DistanceRateFactor		= (ExplosionRad - Distance) / ExplosionRad;

		FVector CharacterToExplosion	= ExplosionLocation - Character->GetActorLocation();
		CharacterToExplosion.Normalize();
		float Dot						= FVector::DotProduct(Character->GetActorForwardVector(), CharacterToExplosion);
		float Rad						= FMath::Acos(Dot);
		float Degree					= FMath::RadiansToDegrees(Rad); // 캐릭터 forward, 폭발 위치 사이의 각도 (90도 이상이면 뒷편)

		UC_Util::Print(Degree, FColor::Green, 5.f);

		// 캐릭터가 현재 바라보고 있는 방향과 거리 등을 따져서 섬광탄 effect 남기기
		AC_Player* Player = Cast<AC_Player>(Character);

		if (IsValid(Player))
		{
			//PostProcessVolume->Settings.BloomIntensity = 250.f;
			Player->ExecuteFlashBangEffect(5.f);
		}


		// TODO : Enemy의 경우 적절한 방해 주기

	}

	UC_Util::Print("FlashBang Explode");
	return true;
}
