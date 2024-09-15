// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/C_Item.h"
#include "C_Bullet.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API AC_Bullet : public AC_Item
{
	GENERATED_BODY()
public:
	AC_Bullet();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* BulletMesh;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UProjectileMovementComponent* BulletProjectileMovement;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class USphereComponent* SphereCollider;
	void SetMeshVisiblility(bool InBool) { BulletMesh->SetVisibility(InBool);}

	bool Fire(class AC_Gun* OwnerGun, FVector InLocation, FVector InDirection);

	//UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	//bool bBulletIsActive() { return BulletProjectileMovement->IsActive(); }
};
