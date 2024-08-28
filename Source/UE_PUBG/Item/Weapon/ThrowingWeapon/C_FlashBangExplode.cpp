// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/ThrowingWeapon/C_FlashBangExplode.h"

#include "Item/Weapon/ThrowingWeapon/C_ThrowingWeapon.h"
#include "Kismet/GameplayStatics.h"

#include "Components/ShapeComponent.h"
#include "Components/SphereComponent.h"


#include "Utility/C_Util.h"

#include "Character/C_Player.h"

#include "PhysicsEngine/PhysicsAsset.h"


bool AC_FlashBangExplode::UseStrategy(AC_ThrowingWeapon* ThrowingWeapon)
{
	// TODO : �ݰ濡 �ִ� ĳ���� �þ߿� ���� �ð� ��� �ֱ�
	// û����ִ� ��� ���� 

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

	TArray<AActor*>				OverlappingActors{};
	TArray<AC_BasicCharacter*>	OverlappedCharacters{};

	ExplosionSphere->GetOverlappingActors(OverlappingActors, TSubclassOf<AC_BasicCharacter>());

	// ��Ȯ�� �ǰ� ������ ���� ��� ĳ������ Physics Asset Collider ���ΰ� ������ �ǰ� ������ ������ �� �ѵα�
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
		// �Ӹ� �ʿ��� LineTrace �õ��ؼ� �´´ٸ�, ����ź ȿ�� �ߵ� �õ�
		FHitResult				HitResult{};
		FVector					DestLocation = Character->GetMesh()->GetBoneLocation("Head");
		FCollisionQueryParams	CollisionParams{};
		CollisionParams.AddIgnoredActor(ThrowingWeapon);

		// �Ӹ� ���� Collider�� �ѱ�
		SetPhysicsAssetColliderEnabled(Character, "Neck", true);

		bool HasHit = GetWorld()->LineTraceSingleByChannel(HitResult, ExplosionLocation, DestLocation, ECC_Visibility, CollisionParams);

		if (!HasHit || HitResult.GetActor() != Character || HitResult.BoneName != "Neck")
		{
			SetPhysicsAssetColliderEnabled(Character, "Neck", false); // �Ӹ� ���� Collider �ٽ� ����
			continue;
		}

		DrawDebugLine(GetWorld(), ExplosionLocation, HitResult.ImpactPoint, FColor::Red, true);

		// Apply FlashBang effect to character
		ExecuteExplosionEffectToCharacter(Character, ExplosionLocation, ExplosionRad);

		// �Ӹ� ���� Collider �ٽ� ���� (�ڿ� �ִ� ĳ������ ��ø�� ���ϱ� ����)
		SetPhysicsAssetColliderEnabled(Character, "Neck", false);
	}

	// ��� ĳ������ Physics Asset �ٽ� �ѵα�
	for (AC_BasicCharacter* Character : OverlappedCharacters)
		SetPhysicsAssetCollidersEnabled(Character, true);

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
	float Degree					= FMath::RadiansToDegrees(Rad); // ĳ���� forward, ���� ��ġ ������ ���� (90�� �̻��̸� ����)

	// ĳ���Ͱ� ���� �ٶ󺸰� �ִ� ����� �Ÿ� ���� ������ ����ź effect �����
	AC_Player* Player = Cast<AC_Player>(Character);

	if (IsValid(Player))
	{
		//PostProcessVolume->Settings.BloomIntensity = 250.f;

		float EffectDuration = EFFECT_DURATION_MAX;

		// �����Ÿ����� �־����� Duration���Ҹ� ��Ű��, ����ź�� �ٶ󺸴� ���⿡ ���� ���ұ��� �����Ŵ
		if (DistanceRateFactor < 0.7f) EffectDuration *= (Degree <= 110.f) ? DistanceRateFactor : DistanceRateFactor * 0.5f;
		
		UC_Util::Print("EffectDuration : " + FString::SanitizeFloat(EffectDuration), FColor::Green, 5.f);
		UC_Util::Print("DistanceRateFactor : " + FString::SanitizeFloat(DistanceRateFactor), FColor::Green, 5.f);
		UC_Util::Print("Degree : " + FString::SanitizeFloat(Degree), FColor::Green, 5.f);

		Player->ExecuteFlashBangEffect(EffectDuration);
	}

	// TODO : Enemy�� ��� ������ ���� �ֱ�

}
