// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/Gun/C_SR.h"

#include "C_Bullet.h"
#include "Character/C_BasicCharacter.h"
#include "Character/C_Enemy.h"
#include "Character/C_Player.h"
#include "Character/C_PreviewCharacter.h"
#include "Character/Component/C_SmokeEnteredChecker.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Utility/C_Util.h"

const FName AC_SR::SR_RELOAD_LEFT_HAND_SOCKET_NAME = "Kar98ReloadSocket";

const TMap<FName, float> AC_SR::BODYPARTS_DAMAGERATE =
{
	{"Neck",		2.5f},

	{"Hips",		1.3f},

	{"LeftUpLeg",	0.9f},
	{"LeftFoot",	0.9f},
	{"RightUpLeg",	0.9f},
	{"RightFoot",	0.9f},

	{"Spine",		1.3f},	// 몸통
	{"Spine1",		1.3f},
	{"Spine2",		1.3f},

	{"LeftArm",		0.9f},
	{"LeftHand",	0.9f},

	{"RightArm",	0.9f},
	{"RightHand",	0.9f}
};


AC_SR::AC_SR()
{
	PrimaryActorTick.bCanEverTick = true;
	//PrimaryActorTick.bCanEverTick = false;
	//ItemType 설정.
	//ItemDataRef.ItemType = EItemTypes::MAINGUN;
	//CurGunType = EGunType::SR;
	EQUIPPED_SOCKET_NAME = FName("Sniper_Equip"); // 무기가 손에 부착될 socket 이름
	IronSightWindowLocation = FVector2D(0.5f, 0.5f);
	CurrentShootingMode = EShootingMode::SEMI_AUTO;
}

void AC_SR::BeginPlay()
{
	Super::BeginPlay();
	AIAttackIntervalTime = 2.5f;
}

void AC_SR::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetRelativeRotationOnCrawl();
}

bool AC_SR::ExecuteBoltActionReloadMontage()
{
	if (!IsValid(OwnerCharacter)) return false;

	// 이미 탄창 장전 모션이 실행중일 때
	if (OwnerCharacter->GetMesh()->GetAnimInstance()->Montage_IsPlaying(ReloadMontages[OwnerCharacter->GetPoseState()].Montages[CurState].AnimMontage))
		return false;

	// Bolt Action 장전할 수 없는 상황일 때
	if (CurMagazineBulletCount == 0 || bIsCurrentlyReloadingSRMagazine) return false;	

	// 볼트액션 장전 Montage가 없을 때
	if (SniperReloadMontages.IsEmpty()) return false;
		
	OwnerCharacter->PlayAnimMontage(SniperReloadMontages[OwnerCharacter->GetPoseState()]);
		
	OwnerCharacter->SetHandState(EHandState::WEAPON_GUN);

	
	OwnerCharacter->SetIsReloadingBullet(true);
	BackToMainCamera();
	//OwnerPlayer->SetRecoilTimelineValues(BulletRPM);
		
	return AttachToComponent
	(
		OwnerCharacter->GetMesh(),
		FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
		SR_RELOAD_LEFT_HAND_SOCKET_NAME
	);
}

bool AC_SR::GetIsPlayingMontagesOfAny()
{
	UAnimInstance* OwnerAnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	UAnimMontage* SniperReloadMontage = SniperReloadMontages[OwnerCharacter->GetPoseState()].AnimMontage;
	return Super::GetIsPlayingMontagesOfAny() || OwnerAnimInstance->Montage_IsPlaying(SniperReloadMontage);
}

void AC_SR::SetRelativeRotationOnCrawl()
{
	if (!IsValid(OwnerCharacter)) return;
	EPoseState OwnerCurPoseState = OwnerCharacter->GetPoseState();
	switch (OwnerCurPoseState)
	{
	case EPoseState::STAND:
	case EPoseState::POSE_MAX:
	case EPoseState::CROUCH:
		SetActorRelativeRotation(FRotator(0));
		break;
	case EPoseState::CRAWL:
		SetActorRelativeRotation(RelativeRotationOnCrawl);

		break;
	default:
		break;
	}
}

bool AC_SR::AIFireBullet(AC_BasicCharacter* InTargetCharacter)
{
	if (GetIsPlayingMontagesOfAny()) return false;

	const FVector BulletSpreadRadius	= FVector(100,100,100);
	const FVector EnemyLocation			= InTargetCharacter->GetActorLocation();
	      FVector SpreadLocation		= UKismetMathLibrary::RandomPointInBoundingBox(EnemyLocation,BulletSpreadRadius);
	const FVector FireLocation			= GunMesh->GetSocketLocation(FName("MuzzleSocket"));
	
	FVector SmokeEnemyLocation{};
	if (InTargetCharacter->GetSmokeEnteredChecker()->GetRandomLocationInSmokeArea(SmokeEnemyLocation))
		SpreadLocation = SmokeEnemyLocation;
	
	const FVector BulletVelocity = (SpreadLocation - FireLocation).GetSafeNormal() * 100 * GunDataRef->BulletSpeed;
	
	for (auto& Bullet : OwnerCharacter->GetBullets())
	{
		if (Bullet->GetIsActive()) continue;
		
		
		bool Succeeded = Bullet->Fire(this, FireLocation, BulletVelocity);
		if (!Succeeded)
		{
			UC_Util::Print("From AC_Gun::ExecuteAIAttack : Bullet->Fire Failed!", FColor::MakeRandomColor(), 10.f);
			return false;
		}

		// Bullet Fire Succeeded
		CurMagazineBulletCount--;
		if (GunSoundData->FireSound) UGameplayStatics::PlaySoundAtLocation(this, GunSoundData->FireSound, GetActorLocation());
		
		ExecuteBoltActionReloadMontage();

		return Succeeded;
	}
	
	UC_Util::Print("From AC_SR::AIFireBullet : No More Bullets in Pool", FColor::Red, 10.f);
	return false;
}

float AC_SR::GetDamageRateByBodyPart(const FName& BodyPart)
{
	if (!BODYPARTS_DAMAGERATE.Contains(BodyPart))
	{
		UC_Util::Print("From AC_SR::GetDamageRateByBodyPart : No Body part found!", FColor::Red, 10.f);
		return 0.f;
	}
	
	return BODYPARTS_DAMAGERATE[BodyPart];
}

void AC_SR::ChangeCurShootingMode()
{
	// Blank (SR의 경우 Shooting Mode를 바꿀 수 없게끔 처리)
}

void AC_SR::CancelReload()
{
	Super::CancelReload();
	UAnimInstance* CurAnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();

	UAnimMontage* ReloadMontage = ReloadMontages[OwnerCharacter->GetPoseState()].Montages[CurState].AnimMontage;

	UAnimMontage* SniperReloadMontage = SniperReloadMontages[OwnerCharacter->GetPoseState()].AnimMontage;
	UC_Util::Print("CancelReload : ", FColor::Red, 10.f);
	bIsCurrentlyReloadingSRMagazine = false;
	if (CurAnimInstance->Montage_IsPlaying(ReloadMontage) || CurAnimInstance->Montage_IsPlaying(SniperReloadMontage))
	{
		CurAnimInstance->Montage_Stop(0.02f);
	}
	OwnerCharacter->SetIsReloadingBullet(false);

	
}

void AC_SR::OnSniperReloadEnd()
{
	this->AttachToHand(OwnerCharacter->GetMesh());
	OwnerCharacter->SetIsReloadingBullet(false);
}

