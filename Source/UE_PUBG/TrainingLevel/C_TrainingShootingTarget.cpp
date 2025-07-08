// Fill out your copyright notice in the Description page of Project Settings.


#include "C_TrainingShootingTarget.h"
#include "Components/ShapeComponent.h"
#include "Item/Weapon/Gun/C_Bullet.h"
#include "Item/Weapon/Gun/C_Gun.h"
#include "Utility/C_Util.h"

AC_TrainingShootingTarget::AC_TrainingShootingTarget()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AC_TrainingShootingTarget::BeginPlay()
{
	Super::BeginPlay();

	if (UShapeComponent* HeadCollider = Cast<UShapeComponent>(GetDefaultSubobjectByName("HeadCollider")))
		CorrespondingBodyPartNames.Add(HeadCollider, "Neck");
	else UC_Util::Print("From AC_TrainingShootingTarget::BeginPlay : Cannot find Head Collider", FColor::Red, 10.f);

	if (UShapeComponent* HipCollider = Cast<UShapeComponent>(GetDefaultSubobjectByName("HipCollider")))
		CorrespondingBodyPartNames.Add(HipCollider, "Hips");
	else UC_Util::Print("From AC_TrainingShootingTarget::BeginPlay : Cannot find Hip Collider", FColor::Red, 10.f);
	
	if (UShapeComponent* TorsoCollider = Cast<UShapeComponent>(GetDefaultSubobjectByName("TorsoCollider")))
		CorrespondingBodyPartNames.Add(TorsoCollider, "Spine");
	else UC_Util::Print("From AC_TrainingShootingTarget::BeginPlay : Cannot find TorsoCollider", FColor::Red, 10.f);

	for (int i = 0; i < 2; ++i)
	{
		FString ColliderStr = "ArmCollider" + FString::FromInt(i);
		FName ArmColliderName(*ColliderStr);
		if (UShapeComponent* ArmCollider = Cast<UShapeComponent>(GetDefaultSubobjectByName(ArmColliderName)))
			CorrespondingBodyPartNames.Add(ArmCollider, "LeftArm");
		else UC_Util::Print("From AC_TrainingShootingTarget::BeginPlay : Cannot find ArmCollider", FColor::Red, 10.f);
	}

	if (UShapeComponent* LegCollider = Cast<UShapeComponent>(GetDefaultSubobjectByName("LegCollider")))
		CorrespondingBodyPartNames.Add(LegCollider, "LeftFoot");
	else UC_Util::Print("From AC_TrainingShootingTarget::BeginPlay : Cannot find LegCollider", FColor::Red, 10.f);

	for (const TPair<UShapeComponent*, FName>& Pair : CorrespondingBodyPartNames)
		Pair.Key->OnComponentHit.AddDynamic(this, &AC_TrainingShootingTarget::OnCollisionPartHit);
	
}

void AC_TrainingShootingTarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AC_TrainingShootingTarget::OnCollisionPartHit
(
	UPrimitiveComponent*	HitComponent,
	AActor*					OtherActor,
	UPrimitiveComponent*	OtherComp,
	FVector					NormalImpulse,
	const FHitResult&		Hit
)
{
	UShapeComponent* PartCollider = Cast<UShapeComponent>(HitComponent);
	if (!CorrespondingBodyPartNames.Contains(PartCollider))
	{
		UC_Util::Print("From AC_TrainingShootingTarget::OnCollisionPartHit : Unsaved Part collider hit!", FColor::Red, 10.f);
		return;
	}

	UC_Util::Print("OnCollisionPartHit", FColor::MakeRandomColor(), 10.f);

	// Other Actor가 Bullet인지 조사
	AC_Bullet* Bullet = Cast<AC_Bullet>(OtherActor);
	if (!Bullet) return;

	const float DamageBase = Bullet->GetFiredGun()->GetDamageBase();
	const float DamageRate = Bullet->GetFiredGun()->GetDamageRateByBodyPart(CorrespondingBodyPartNames[PartCollider]);
	const float TotalDamage = DamageBase * DamageRate;

	// TODO : TotalDamage 띄우기 (HeadShot의 경우, 빨간색 & 이미지)	
}