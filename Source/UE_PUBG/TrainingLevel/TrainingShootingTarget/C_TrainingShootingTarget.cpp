// Fill out your copyright notice in the Description page of Project Settings.


#include "C_TrainingShootingTarget.h"

#include "C_ShootingTargetWidgetsHolder.h"
#include "Character/C_Player.h"
#include "Components/ShapeComponent.h"
#include "Item/Weapon/Gun/C_Bullet.h"
#include "Item/Weapon/Gun/C_Gun.h"
#include "Kismet/GameplayStatics.h"
#include "Utility/C_Util.h"

AC_ShootingTargetWidgetsHolder* AC_TrainingShootingTarget::ShootingTargetWidgetsHolder{};

AC_TrainingShootingTarget::AC_TrainingShootingTarget()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AC_TrainingShootingTarget::BeginPlay()
{
	Super::BeginPlay();

	/* Init each body parts collider */
	
	if (UShapeComponent* HeadCollider = Cast<UShapeComponent>(GetDefaultSubobjectByName("HeadCollider")))
	{
		CorrespondingBodyPartNames.Add(HeadCollider, "Neck");
		HeadShapeComponent = HeadCollider;
	}
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

	/* Init Shooting Target Widgets Holder  */
	if (!IsValid(ShootingTargetWidgetsHolder))
	{
		AActor* HolderActor = UGameplayStatics::GetActorOfClass(GetWorld(), AC_ShootingTargetWidgetsHolder::StaticClass());
		if (!HolderActor)
		{
			UC_Util::Print
			(
				"From AC_TrainingShootingTarget::BeginPlay : TrainingShootingTarget on current level but no ShootingTargetWidgetsHolder found!",
				FColor::Red, 10.f
			);
			return;
		}
		ShootingTargetWidgetsHolder = Cast<AC_ShootingTargetWidgetsHolder>(HolderActor);		
	}
}

void AC_TrainingShootingTarget::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (ShootingTargetWidgetsHolder) ShootingTargetWidgetsHolder = nullptr;
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

	// Other Actor가 Bullet인지 조사
	AC_Bullet* Bullet = Cast<AC_Bullet>(OtherActor);
	if (!Bullet) return;

	// Bullet을 쏜 주체가 Player인지 조사
	if (!Cast<AC_Player>(Bullet->GetOwnerCharacter())) return;

	const float DamageBase = Bullet->GetFiredGun()->GetDamageBase();
	const float DamageRate = Bullet->GetFiredGun()->GetDamageRateByBodyPart(CorrespondingBodyPartNames[PartCollider]);
	const float TotalDamage = DamageBase * DamageRate;

	// Spawn TotalDamage Info Widget
	if (!IsValid(ShootingTargetWidgetsHolder))
	{
		UC_Util::Print("From AC_TrainingShootingTarget::OnCollisionPartHit : Target hitted but invalid ShootingTargetWidgetsHolder", FColor::Red, 10.f);
		return;
	}

	ShootingTargetWidgetsHolder->SpawnDamageInfoWidget(PartCollider == HeadShapeComponent, TotalDamage, Hit.ImpactPoint + FVector::UnitZ() * 20.f);
}