// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "I_ExplodeStrategy.h"
#include "C_FlashBangExplode.generated.h"

UCLASS(Blueprintable)
class UE_PUBG_API AC_FlashBangExplode : public AActor, public II_ExplodeStrategy
{
	GENERATED_BODY()

public:

	//AC_FlashBangExplode();

protected:

	//void BeginPlay() override;

private:

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
	/// 피격 판정이 난 Character에게 줄 피격 Effect 실행
	/// </summary>
	/// <param name="Character"> : 피격 당한 Character </param>
	/// <param name="ExplosionLocation"> : 섬광탄 폭발 위치 </param>
	/// <param name="ExplosionRad"> : 폭발 반경 </param>
	void ExecuteExplosionEffectToCharacter(class AC_BasicCharacter* Character, FVector ExplosionLocation, float ExplosionRad);

private:

	static const float EFFECT_DURATION_MAX;

};


