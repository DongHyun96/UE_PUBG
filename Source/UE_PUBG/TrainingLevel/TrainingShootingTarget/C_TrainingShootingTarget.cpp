// Fill out your copyright notice in the Description page of Project Settings.


#include "C_TrainingShootingTarget.h"

#include "C_ShootingTargetDamageInfoWidget.h"
#include "C_ShootingTargetWidgetsHolder.h"
#include "Character/C_Player.h"
#include "Components/ShapeComponent.h"
#include "Item/Equipment/C_Helmet.h"
#include "Item/Equipment/C_Vest.h"
#include "Item/Weapon/Gun/C_Bullet.h"
#include "Item/Weapon/Gun/C_Gun.h"
#include "Kismet/GameplayStatics.h"
#include "Utility/C_Util.h"

AC_ShootingTargetWidgetsHolder* AC_TrainingShootingTarget::ShootingTargetWidgetsHolder{};

FTutorialStageGoalCheckerDelegate AC_TrainingShootingTarget::WeaponTutorialDelegate{};
FTutorialStageUpdateWidgetNumberDelegate AC_TrainingShootingTarget::TutorialUpdateWidgetNumberDelegate{};
uint8 AC_TrainingShootingTarget::WeaponTutorialHitCount{};

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

	ToggleStandState(true);
}

void AC_TrainingShootingTarget::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (ShootingTargetWidgetsHolder) ShootingTargetWidgetsHolder = nullptr;
}

void AC_TrainingShootingTarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GrenadeDamagedLayDownTimer <= 0.f) return;
	GrenadeDamagedLayDownTimer -= DeltaTime;

	if (GrenadeDamagedLayDownTimer <= 0.f)
	{
		GrenadeDamagedLayDownTimer = 0.f;
		ToggleStandState(true);
	}
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
	AC_Player* Player = Cast<AC_Player>(Bullet->GetOwnerCharacter());
	if (!Player) return;

	// WeaponTutorialDelegate가 Bound되어있고, ADS 모드일 때의 처리
	if (WeaponTutorialDelegate.IsBound() && Bullet->GetFiredGun()->GetIsAimPress())
	{
		if (WeaponTutorialDelegate.Execute(3, WeaponTutorialHitCount))
		{
			++WeaponTutorialHitCount;

			// Widget Number update Delegate 호출 처리
			if (TutorialUpdateWidgetNumberDelegate.IsBound())
				TutorialUpdateWidgetNumberDelegate.Broadcast(WeaponTutorialHitCount);
		}
	}

	const float DamageBase = Bullet->GetFiredGun()->GetDamageBase();
	const float DamageRate = Bullet->GetFiredGun()->GetDamageRateByBodyPart(CorrespondingBodyPartNames[PartCollider]);
	float TotalDamage = DamageBase * DamageRate; // 기본 Total Damage

	// Spawn TotalDamage Info Widget
	if (!IsValid(ShootingTargetWidgetsHolder))
	{
		UC_Util::Print("From AC_TrainingShootingTarget::OnCollisionPartHit : Target hitted but invalid ShootingTargetWidgetsHolder", FColor::Red, 10.f);
		return;
	}

	// HeadShot 여부 결정
	EShootingTargetDamageInfoType DamageInfoType =
		(PartCollider == HeadShapeComponent) ? EShootingTargetDamageInfoType::HeadShot : EShootingTargetDamageInfoType::Normal;
	
	// ArmorLevel이 0이 아닌 경우, TotalDamage ArmorLevel에 따라 줄이기
	if (ArmorLevel != ETargetArmorLevel::Lv0)
	{
		EEquipableItemLevel EquipableItemLevel = static_cast<EEquipableItemLevel>(static_cast<uint8>(ArmorLevel) - 1);
		TotalDamage *= (DamageInfoType == EShootingTargetDamageInfoType::HeadShot) ?
			AC_Helmet::GetDamageReduceFactor(EquipableItemLevel) : AC_Vest::GetDamageReduceFactor(EquipableItemLevel); 
	}
	
	ShootingTargetWidgetsHolder->SpawnDamageInfoWidget(DamageInfoType, TotalDamage, Hit.ImpactPoint + FVector::UnitZ() * 20.f);
}

void AC_TrainingShootingTarget::ExecuteGrenadeDamaged()
{
	GrenadeDamagedLayDownTimer = 3.f;
	ToggleStandState(false);
}
