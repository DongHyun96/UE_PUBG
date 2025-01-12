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

	// II_ExplodeStrategy��(��) ���� ��ӵ�
	bool UseStrategy(AC_ThrowingWeapon* ThrowingWeapon) override;

private:

	/// <summary>
	/// PhysicsAsset�� Collider�� ���� / �ѱ�
	/// </summary>
	/// <param name="Character"> : Target Character </param>
	/// <param name="CollisionEnabled"> : Collision Enabled if true </param>
	void SetPhysicsAssetCollidersEnabled(class AC_BasicCharacter* Character, bool Enabled);

	
	/// <summary>
	/// Ư�� bone�� ���� Collider ���� / �ѱ�
	/// </summary>
	/// <param name="Character"> : Target Character </param>
	/// <param name="BoneName"> : Collider ���� ų Bone�� BoneName </param>
	/// <param name="CollisionEnabled"> : Collision Enabled if true </param>
	/// <returns> : BoneName�� �ִٸ� Setting�ϰ� return true </returns>
	bool SetPhysicsAssetColliderEnabled(class AC_BasicCharacter* Character, FName BoneName, bool Enabled);



private:

	/// <summary>
	/// ĳ���Ϳ��� Damage �ֱ� �õ�
	/// </summary>
	/// <param name="Character"> : Target Character </param>
	/// <param name="ThrowingWeapon"> : Damage�� ������ �ϴ� ThrowingWeapon </param>
	/// <param name="ExplosionSphere"> : ThrowingWeapon�� Explosion Sphere </param>
	/// <returns> : Damage�� ��������(LineTrace�� �����ߴٸ�) return true </returns>
	bool TryDamagingCharacter(class AC_BasicCharacter* Character, class AC_ThrowingWeapon* ThrowingWeapon, class USphereComponent* ExplosionSphere);

	/// <summary>
	/// �ǰ� ������ �� Character���� �� �ǰ� Effect ����
	/// </summary>
	/// <param name="Character"> : �ǰ� ���� Character </param>
	/// <param name="ExplosionLocation"> : ����ź ���� ��ġ </param>
	/// <param name="ExplosionRad"> : ���� �ݰ� </param>
	void ExecuteExplosionEffectToCharacter(class AC_BasicCharacter* Character, FVector ExplosionLocation, float ExplosionRad);

private:

	// �� �ǰ� ������ Damage rate
	static const TMap<FName, float> BodyPartsDamageRate;

	// Ray�� �� Destination bone names
	static const TArray<FName> LineTraceDestBoneNames;

	static const float DAMAGE_BASE;

};
