// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/Gun/C_Bullet.h"
#include "Item/Weapon/Gun/C_Gun.h"
#include "Components/Image.h"
#include "Components/Widget.h"
#include "Blueprint/UserWidget.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "UMG.h"


#include "Components/PanelWidget.h"
#include "Components/NamedSlotInterface.h"
#include "Utility/C_Util.h"

#include "Components/CanvasPanelSlot.h"
#include "Character/C_BasicCharacter.h"
#include "Character/Component/C_EquippedComponent.h"
#include "GameFramework/Actor.h"
#include "Components/ShapeComponent.h"
#include "Components/SceneComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Character/C_Player.h"
#include "GameFramework/SpringArmComponent.h"
#include "Item/Weapon/WeaponStrategy/C_GunStrategy.h"


AC_Bullet::AC_Bullet()
{
	PrimaryActorTick.bCanEverTick = true;
	//WeaponButtonStrategy = CreateDefaultSubobject<AC_GunStrategy>("GunStrategy");
	if (!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	}
	BulletProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("BulletProjectileMovement");
	BulletProjectileMovement->MaxSpeed = 620 * 100.0f;
	BulletProjectileMovement->ProjectileGravityScale = 0;
	BulletProjectileMovement->SetUpdatedComponent(RootComponent);
	
}

void AC_Bullet::BeginPlay()
{
	Super::BeginPlay();
	BulletProjectileMovement->SetActive(false);
	//BulletMesh = FindComponentByClass<UStaticMeshComponent>();
	//BulletMesh->SetupAttachment(RootComponent);
}

void AC_Bullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UC_Util::Print(float(GetActorLocation().X));
	//UC_Util::Print(float(BulletProjectileMovement->Velocity.X));
	//UC_Util::Print(float(BulletProjectileMovement->Velocity.Y));
	//UC_Util::Print(float(BulletProjectileMovement->Velocity.Z));

}
