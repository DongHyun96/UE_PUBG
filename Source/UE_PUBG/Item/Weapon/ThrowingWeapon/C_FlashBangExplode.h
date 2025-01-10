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
	/// �ǰ� ������ �� Character���� �� �ǰ� Effect ����
	/// </summary>
	/// <param name="Character"> : �ǰ� ���� Character </param>
	/// <param name="ExplosionLocation"> : ����ź ���� ��ġ </param>
	/// <param name="ExplosionRad"> : ���� �ݰ� </param>
	void ExecuteExplosionEffectToCharacter(class AC_BasicCharacter* Character, FVector ExplosionLocation, float ExplosionRad);

private:

	static const float EFFECT_DURATION_MAX;

};
