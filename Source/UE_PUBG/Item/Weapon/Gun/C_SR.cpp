// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/Gun/C_SR.h"

#include "C_Bullet.h"
#include "Character/C_BasicCharacter.h"
#include "Character/C_Enemy.h"
#include "Character/C_Player.h"
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
}

void AC_SR::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetRelativeRotationOnCrawl();
}

bool AC_SR::ExecuteReloadMontage()
{
	int LeftAmmoCount = 0;
	AC_Item_Bullet* CurBullet = Cast<AC_Item_Bullet>( OwnerCharacter->GetInvenComponent()->FindMyItemByName(GetCurrentBulletTypeName()));
	if (IsValid(CurBullet))
	{
		LeftAmmoCount = CurBullet->GetItemCurStack();
	}
	UC_Util::Print(LeftAmmoCount);
	if (!IsValid(OwnerCharacter)) return false;
	if (SniperReloadMontages.IsEmpty()) return false;
	//if (CurBulletCount == MaxBulletCount) return false;

	AC_Player* CurPlayer = Cast<AC_Player>(OwnerCharacter);
	AC_Enemy* CurEnemy = Cast<AC_Enemy>(OwnerCharacter);
	if (!IsValid(CurPlayer) && !IsValid(CurEnemy)) return false;
	if (IsValid(CurPlayer))
	{
		if (CurPlayer->GetMesh()->GetAnimInstance()->Montage_IsPlaying(ReloadMontages[OwnerCharacter->GetPoseState()].Montages[CurState].AnimMontage))	return false;
	}

	if (IsValid(CurEnemy))
	{
		if (CurEnemy->GetMesh()->GetAnimInstance()->Montage_IsPlaying(ReloadMontages[OwnerCharacter->GetPoseState()].Montages[CurState].AnimMontage))	return false;
	}
	UC_Util::Print(CurBulletCount, FColor::Green);
	UC_Util::Print(bIsReloadingSR, FColor::Green);
	//UC_Util::Print(CurBulletCount, FColor::Green);

	if (CurBulletCount >= 1 && CurrentShootingMode == EShootingMode::SINGLE_SHOT &&!bIsReloadingSR)
	{
		UAnimMontage* DrawMontage = SniperReloadMontages[OwnerCharacter->GetPoseState()].AnimMontage;
		OwnerCharacter->PlayAnimMontage(SniperReloadMontages[OwnerCharacter->GetPoseState()]);
		UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
		OwnerCharacter->SetHandState(EHandState::WEAPON_GUN);
		bIsSniperReload = true;
		OwnerCharacter->SetIsReloadingBullet(true);
		BackToMainCamera();
		UC_Util::Print("Nori hutoi",FColor::Cyan);
		//OwnerPlayer->SetRecoilTimelineValues(BulletRPM);
		
		return 	AttachToComponent
		(
			OwnerCharacter->GetMesh(),
			FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
			SR_RELOAD_LEFT_HAND_SOCKET_NAME
		);
	}
	else
	{
		if (LeftAmmoCount == 0) return false;
		UC_Util::Print(CurBulletCount);
		UC_Util::Print(bIsReloadingSR);
		OwnerCharacter->SetIsReloadingBullet(true);
		OwnerCharacter->PlayAnimMontage(ReloadMontages[OwnerCharacter->GetPoseState()].Montages[CurState]);
		BackToMainCamera();
	
	}
	return true;
}	

bool AC_SR::GetIsPlayingMontagesOfAny()
{
	UAnimInstance* CurAnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	UAnimMontage* DrawMontage = DrawMontages[OwnerCharacter->GetPoseState()].Montages[CurState].AnimMontage;
	//UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	UAnimMontage* SheathMontage = SheathMontages[OwnerCharacter->GetPoseState()].Montages[CurState].AnimMontage;
	UAnimMontage* ReloadMontage = ReloadMontages[OwnerCharacter->GetPoseState()].Montages[CurState].AnimMontage;
	UAnimMontage* SniperReloadMontage = SniperReloadMontages[OwnerCharacter->GetPoseState()].AnimMontage;

	bool IsPlayingMontagesOfAny =
		CurAnimInstance->Montage_IsPlaying(DrawMontage) ||
		CurAnimInstance->Montage_IsPlaying(SheathMontage) ||
		CurAnimInstance->Montage_IsPlaying(ReloadMontage) ||
		CurAnimInstance->Montage_IsPlaying(SniperReloadMontage);
	//UC_Util::Print(IsPlayingMontagesOfAny, FColor::Magenta, 10);
	return IsPlayingMontagesOfAny;
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

bool AC_SR::ExecuteAIAttackTickTask(AC_BasicCharacter* InTargetCharacter, const float& DeltaTime)
{
	if (!CanAIAttack(InTargetCharacter))
	{
		return false;
	}
	int BackpackBulletStack = 0;
	if (IsValid(OwnerCharacter->GetInvenComponent()->FindMyItemByName(GetCurrentBulletTypeName())))
		BackpackBulletStack = OwnerCharacter->GetInvenComponent()->FindMyItemByName(GetCurrentBulletTypeName())->GetItemCurStack();
	if (CurBulletCount == 0 &&  BackpackBulletStack== 0)
	{
		UC_Util::Print("Back To Wait Condition");
		return false;
	}
	//ExecuteReloadMontage();
	AC_Enemy* OwnerEnemy = Cast<AC_Enemy>(OwnerCharacter); 
	FVector EnemyLocation = InTargetCharacter->GetActorLocation();
	FVector FireLocation = GunMesh->GetSocketLocation(FName("MuzzleSocket"));

	FVector FireDirection = (EnemyLocation - FireLocation).GetSafeNormal() * 100 * GunDataRef->BulletSpeed;

	//if (!SetBulletDirection(FireLocation, FireDirection, HitLocation, HasHit)) return false;

	//UC_Util::Print(FireLocation);
	//UC_Util::Print(FireDirection);
	FVector Direction = (EnemyLocation - GetActorLocation()).GetSafeNormal();
	FRotator LookRotation = Direction.Rotation();
	//float DeltaTime = GetWorld()->GetDeltaSeconds();
	//UC_Util::Print("Change Rotation");
	float InterpSpeed = 10.0f;
	FRotator CurrentRotation =  OwnerCharacter->GetActorRotation();
	FRotator NewRotation	= FMath::RInterpTo(CurrentRotation, LookRotation, DeltaTime, InterpSpeed);
	NewRotation.Pitch		= 0.f;
	NewRotation.Roll		= 0.f;
	OwnerEnemy->SetActorRotation(NewRotation);
	//UC_Util::Print("Trying To Attack");
	AIFireTimer += DeltaTime;
	//UC_Util::Print(AIFireTimer);
	if (AIFireTimer > 3.0f && abs(NewRotation.Yaw - LookRotation.Yaw) < 10.0f)
	{
		AIFireBullet(InTargetCharacter);
	}
	return true;
}

bool AC_SR::AIFireBullet(class AC_BasicCharacter* InTargetCharacter)
{
	FVector BulletSpreadRadius = FVector(100,100,100);
	FVector EnemyLocation = InTargetCharacter->GetActorLocation();
	FVector SpreadLocation = UKismetMathLibrary::RandomPointInBoundingBox(EnemyLocation,BulletSpreadRadius);
	FVector FireLocation = GunMesh->GetSocketLocation(FName("MuzzleSocket"));
	
	FVector SmokeEnemyLocation;
	if (InTargetCharacter->GetSmokeEnteredChecker()->GetRandomLocationInSmokeArea(SmokeEnemyLocation))
		SpreadLocation = SmokeEnemyLocation;
	
	FVector FireDirection = (SpreadLocation - FireLocation).GetSafeNormal() * 100 * GunDataRef->BulletSpeed;
	AC_Enemy* OwnerEnemy = Cast<AC_Enemy>(OwnerCharacter);
	if (GetIsPlayingMontagesOfAny())
	{
		//UC_Util::Print("AI Cant Fire Gun",FColor::MakeRandomColor(), 1000);
		return false;
	}


	//if (!SetBulletDirection(FireLocation, FireDirection, HitLocation, HasHit)) return false;

	//UC_Util::Print(FireLocation);
	//UC_Util::Print(FireDirection);

	//return true;
	bool ApplyGravity = true;
	for (auto& Bullet : OwnerEnemy->GetBullets())
	{
		if (CurBulletCount == 0)
			break;
		if (Bullet->GetIsActive())
		{
			//UC_Util::Print("Can't fire");
			continue;
		}
		//UC_Util::Print("FIRE!!!!!!!");
		CurBulletCount--;
		bool Succeeded = Bullet->Fire(this, FireLocation, FireDirection, ApplyGravity);
		if (!Succeeded) UC_Util::Print("From AC_Gun::ExecuteAIAttack : Bullet->Fire Failed!", FColor::MakeRandomColor(), 10.f);
		if (GunSoundData->FireSound) UGameplayStatics::PlaySoundAtLocation(this, GunSoundData->FireSound, GetActorLocation());
		//UC_Util::Print("SR Reloading Start!!!!!!!!!!", FColor::MakeRandomColor(), 10.f);
		AIFireTimer = 0.0f;

		ExecuteReloadMontage();

		return Succeeded;
	
		//Bullet->Fire(this, FireLocation, FireDirection);
		//if (BulletCount > 100)
		//	return true;
	}
	ExecuteReloadMontage();

	//UC_Util::Print("No More Bullets in Pool", FColor::MakeRandomColor(), 10.f);
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
	UC_Util::Print("CancleReload : ", FColor::Red, 10.f);
	bIsReloadingSR = false;
	if (CurAnimInstance->Montage_IsPlaying(ReloadMontage) || CurAnimInstance->Montage_IsPlaying(SniperReloadMontage))
	{
		CurAnimInstance->Montage_Stop(0.02);
	}
	OwnerCharacter->SetIsReloadingBullet(false);

	
}

