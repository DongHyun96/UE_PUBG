// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/Gun/C_AR.h"
#include "Item/Weapon/Gun/C_Bullet.h"

#include "Character/C_Enemy.h"
#include "Item/ItemBullet/C_Item_Bullet.h"
#include "Item/Attachment/C_AttachableItem.h"
#include "Character/C_Player.h"
#include "Character/C_BasicCharacter.h"
#include "Character/Component/C_SmokeEnteredChecker.h"
#include "HUD/C_AmmoWidget.h"
#include "HUD/C_HUDWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Utility/C_Util.h"

const TMap<FName, float> AC_AR::BODYPARTS_DAMAGERATE =
{
	{"Neck",		2.35f},

	{"Hips",		0.9f},

	{"LeftUpLeg",	0.9f},
	{"LeftFoot",	0.9f},
	{"RightUpLeg",	0.9f},
	{"RightFoot",	0.9f},

	{"Spine",		1.f},	// 몸통
	{"Spine1",		1.f},
	{"Spine2",		1.f},

	{"LeftArm",	0.9f},
	{"LeftHand",	0.9f},

	{"RightArm",	0.9f},
	{"RightHand",	0.9f}
};

AC_AR::AC_AR()
{
	PrimaryActorTick.bCanEverTick = true;
	//ItemType 설정.
	//ItemDataRef.ItemType = EItemTypes::MAINGUN; ItemType
	//CurGunType = EGunType::AR;
	EQUIPPED_SOCKET_NAME = FName("Rifle_Equip"); // 무기가 손에 부착될 socket 이름
	IronSightWindowLocation = FVector2D(0.5f, 0.4f);

}

void AC_AR::BeginPlay()
{
	Super::BeginPlay();
}

void AC_AR::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AC_AR::ExecuteReloadMontage()
{
	int LeftAmmoCount = 0;
	AC_Item_Bullet* CurBullet = Cast<AC_Item_Bullet>( OwnerCharacter->GetInvenComponent()->FindMyItemByName(GetCurrentBulletTypeName()));
	if (IsValid(CurBullet))
	{
		LeftAmmoCount = CurBullet->GetItemCurStack();
	}
	
	if (!OwnerCharacter) return false;
	if (LeftAmmoCount == 0) return false;
	if (CurBulletCount == MaxBulletCount)
	{
		return false;
	}
	AC_Player* CurPlayer = Cast<AC_Player>(OwnerCharacter);
	AC_Enemy* CurEnemy = Cast<AC_Enemy>(OwnerCharacter);
	if (!IsValid(CurPlayer) && !IsValid(CurEnemy)) return false;
	
	if (CurBulletCount == MaxBulletCount) return false;
	if (IsValid(CurPlayer))
	{
		if (CurPlayer->GetMesh()->GetAnimInstance()->Montage_IsPlaying(ReloadMontages[OwnerCharacter->GetPoseState()].Montages[CurState].AnimMontage))	return false;
		if (!CurPlayer->GetCanMove()) return false;
	}

	if (IsValid(CurEnemy))
	{
		if (CurEnemy->GetMesh()->GetAnimInstance()->Montage_IsPlaying(ReloadMontages[OwnerCharacter->GetPoseState()].Montages[CurState].AnimMontage))	return false;
		if (!CurEnemy->GetCanMove()) return false;
	}
	SetMagazineVisibility(false);
	OwnerCharacter->SetIsReloadingBullet(true);
	OwnerCharacter->PlayAnimMontage(ReloadMontages[OwnerCharacter->GetPoseState()].Montages[CurState]);
	BackToMainCamera();
	return true;
}

bool AC_AR::ExecuteAIAttack(AC_BasicCharacter* InTargetCharacter)
{
	// int LeftAmmoCount = 0;
	// AC_Item_Bullet* CurBullet = Cast<AC_Item_Bullet>( OwnerCharacter->GetInvenComponent()->FindMyItemByName(GetCurrentBulletTypeName()));
	// if (IsValid(CurBullet))
	// {
	// 	LeftAmmoCount = CurBullet->GetItemCurStack();
	// }
	// if (LeftAmmoCount == 0 && CurBulletCount == 0)
	// {
	// 	UC_Util::Print("Back To Wait Condition");
	// 	return false;
	// }
	return Super::ExecuteAIAttack(InTargetCharacter);
}

bool AC_AR::ExecuteAIAttackTickTask(class AC_BasicCharacter* InTargetCharacter, const float& DeltaTime)
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
	UC_Util::Print("Trying To Attack");
	AIFireTimer += DeltaTime;
	//UC_Util::Print(AIFireTimer);
	if (AIFireTimer > GetBulletRPM() && abs(NewRotation.Yaw - LookRotation.Yaw) < 10.0f)
	{
		return AIFireBullet(InTargetCharacter);
	}
	return true;
}

bool AC_AR::AIFireBullet(class AC_BasicCharacter* InTargetCharacter)
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
		if (GunSoundData->ShoottingSound) UGameplayStatics::PlaySoundAtLocation(this, GunSoundData->ShoottingSound, GetActorLocation());


		AIFireTimer = 0.0f;
		return Succeeded;
	
		//Bullet->Fire(this, FireLocation, FireDirection);
		//if (BulletCount > 100)
		//	return true;
	}
	ExecuteReloadMontage();

	//UC_Util::Print("No More Bullets in Pool", FColor::MakeRandomColor(), 10.f);
	return false;
}

float AC_AR::GetDamageRateByBodyPart(const FName& BodyPart)
{
	if (!BODYPARTS_DAMAGERATE.Contains(BodyPart))
	{
		// UC_Util::Print("From AC_AR::GetDamageRateByBodyPart : No Body part found!", FColor::Red, 10.f);
		return 0.f;
	}
	
	return BODYPARTS_DAMAGERATE[BodyPart];
}

void AC_AR::ChangeCurShootingMode()
{
	// 단발, 연사만을 포함한 ShootingMode Switching
	// 추후, AR 종류에 따른 점사까지 포함한 처리를 한다하면, 전략 객체로 블루프린트에서 넣는 구조로 가져갈 것
	int CurMode = static_cast<int>(CurrentShootingMode);
	++CurMode %= 2;
	CurrentShootingMode = static_cast<EShootingMode>(CurMode);

	if (AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharacter))
		OwnerPlayer->GetHUDWidget()->GetAmmoWidget()->SetShootingMode(CurrentShootingMode);
}


