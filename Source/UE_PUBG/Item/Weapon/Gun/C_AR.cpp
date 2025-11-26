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
#include "HUD/C_InformWidget.h"
#include "Singleton/C_GameSceneManager.h"

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

	{"LeftArm",		0.9f},
	{"LeftHand",	0.9f},

	{"RightArm",	0.9f},
	{"RightHand",	0.9f}
};

AC_AR::AC_AR()
{
	PrimaryActorTick.bCanEverTick = true;
	//PrimaryActorTick.bCanEverTick = false;
	//ItemType 설정.
	//ItemDataRef.ItemType = EItemTypes::MAINGUN; ItemType
	//CurGunType = EGunType::AR;
	EquippedSocketName = FName("Rifle_Equip"); // 무기가 손에 부착될 socket 이름
	IronSightWindowLocation = FVector2D(0.5f, 0.4f);

}

void AC_AR::BeginPlay()
{
	Super::BeginPlay();
	AIAttackIntervalTime = GetBulletRPM();
}

void AC_AR::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
	{
		OwnerPlayer->GetHUDWidget()->GetAmmoWidget()->SetShootingMode(CurrentShootingMode);
		
		FString PlayerLog = "CHANGED FIRING MODE : ";
		
		switch (CurrentShootingMode)
		{
		case EShootingMode::SEMI_AUTO: 	PlayerLog += "SEMI AUTO"; break;
		case EShootingMode::FULL_AUTO: 	PlayerLog += "FULL AUTO"; break;
		case EShootingMode::MAX: break;
		}

		OwnerPlayer->GetHUDWidget()->GetInformWidget()->AddPlayerWarningLog(PlayerLog);
		UGameplayStatics::PlaySound2D(this, GunSoundData->ChangeShootingModeSound, 1.f, FMath::RandRange(1.f, 1.25f));
	}
}

void AC_AR::CancelReload()
{
	Super::CancelReload();
	UAnimInstance* CurAnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();

	UAnimMontage* ReloadMontage = ReloadMontages[OwnerCharacter->GetPoseState()].Montages[CurGunSlotState].AnimMontage;


	if (CurAnimInstance->Montage_IsPlaying(ReloadMontage))
	{
		CurAnimInstance->Montage_Stop(0.02);
	}
	OwnerCharacter->SetIsReloadingBullet(false);

}

bool AC_AR::DestroyItem(bool bNetForce, bool bShouldModifyLevel)
{
	if (IsValid(Magazine)) Magazine->Destroy();
	return Super::DestroyItem(bNetForce, bShouldModifyLevel);
}


