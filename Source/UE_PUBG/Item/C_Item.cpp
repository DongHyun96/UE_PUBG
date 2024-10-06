// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/C_Item.h"
#include "Utility/C_Util.h"
#include "Character/C_BasicCharacter.h"

#include "Weapon/WeaponStrategy/I_WeaponButtonStrategy.h"
#include "Weapon/WeaponStrategy/C_GunStrategy.h"

// Sets default values
AC_Item::AC_Item()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    //WeaponButtonStrategy = CreateDefaultSubobject<AC_GunStrategy>("GunStrategy");

	//OwnerCharacter = nullptr;
}

// Called when the game starts or when spawned
void AC_Item::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AC_Item::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AC_Item::DetachmentItem()
{
	
	if (!OwnerCharacter) return;
	
	DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);

	SetActorLocation(OwnerCharacter->GetActorLocation() + FVector(0.f, 0.f, -75.f));
	SetActorRotation(FRotator(0.f, 0.f, -90.f));

	SetOwnerCharacter(nullptr);

	//«—π¯ ≤Ø¥Ÿ ≤®¡‡æﬂ OverlapBegin¿Ã ¿€µø
	SetActorEnableCollision(false);
	SetActorEnableCollision(true);

	UC_Util::Print("Check");
}

void AttachToSocket(USceneComponent* InParent)
{

}
