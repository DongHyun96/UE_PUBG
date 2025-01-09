// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "I_ExplodeStrategy.h"
#include "C_GrenadeExplode.generated.h"

UCLASS(Blueprintable)
class UE_PUBG_API AC_GrenadeExplode : public AActor, public II_ExplodeStrategy
{
	GENERATED_BODY()

public:
	AC_GrenadeExplode();
	
private:

	// II_ExplodeStrategy을(를) 통해 상속됨
	bool UseStrategy(AC_ThrowingWeapon* ThrowingWeapon) override;

private:

	/// <summary>
	/// PhysicsAsset의 Collider들 끄기 / 켜기
	/// </summary>
	/// <param name="Character"> : Target Character </param>
	/// <param name="CollisionEnabled"> : Collision Enabled if true </param>
	void SetPhysicsAssetCollidersEnabled(class AC_BasicCharacter* Character, bool Enabled);

	
	/// <summary>
	/// 특정 bone에 대한 Collider 끄기 / 켜기
	/// </summary>
	/// <param name="Character"> : Target Character </param>
	/// <param name="BoneName"> : Collider 끄고 킬 Bone의 BoneName </param>
	/// <param name="CollisionEnabled"> : Collision Enabled if true </param>
	/// <returns> : BoneName이 있다면 Setting하고 return true </returns>
	bool SetPhysicsAssetColliderEnabled(class AC_BasicCharacter* Character, FName BoneName, bool Enabled);



private:

	/// <summary>
	/// 캐릭터에게 Damage 주기 시도
	/// </summary>
	/// <param name="Character"> : Target Character </param>
	/// <param name="ThrowingWeapon"> : Damage를 입히려 하는 ThrowingWeapon </param>
	/// <param name="ExplosionSphere"> : ThrowingWeapon의 Explosion Sphere </param>
	/// <returns> : Damage를 입혔으면(LineTrace를 성공했다면) return true </returns>
	bool TryDamagingCharacter(class AC_BasicCharacter* Character, class AC_ThrowingWeapon* ThrowingWeapon, class USphereComponent* ExplosionSphere);

	/// <summary>
	/// 피격 판정이 난 Character에게 줄 피격 Effect 실행
	/// </summary>
	/// <param name="Character"> : 피격 당한 Character </param>
	/// <param name="ExplosionLocation"> : 수류탄 폭발 위치 </param>
	/// <param name="ExplosionRad"> : 폭발 반경 </param>
	void ExecuteExplosionEffectToCharacter(class AC_BasicCharacter* Character, FVector ExplosionLocation, float ExplosionRad);

private:

	// 각 피격 부위별 Damage rate
	static const TMap<FName, float> BodyPartsDamageRate;

	// Ray를 쏠 Destination bone names
	static const TArray<FName> LineTraceDestBoneNames;

	static const float DAMAGE_BASE;

};
