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
	UStaticMeshComponent* BulletMesh;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UProjectileMovementComponent* BulletProjectileMovement;
};