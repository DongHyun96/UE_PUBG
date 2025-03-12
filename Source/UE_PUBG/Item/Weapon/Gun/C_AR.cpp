// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/Gun/C_AR.h"
#include "Item/Attachment/C_AttachableItem.h"
#include "Character/C_Player.h"
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
	if (CurBulletCount == MaxBulletCount) return false;
	if (!CurPlayer->GetCanMove()) return false;
	if (CurPlayer->GetMesh()->GetAnimInstance()->Montage_IsPlaying(ReloadMontages[OwnerCharacter->GetPoseState()].Montages[CurState].AnimMontage))	return false;
	SetMagazineVisibility(false);
	OwnerCharacter->SetIsReloadingBullet(true);
	OwnerCharacter->PlayAnimMontage(ReloadMontages[OwnerCharacter->GetPoseState()].Montages[CurState]);
	BackToMainCamera();
	return true;
}

bool AC_AR::ExecuteAIAttack(AC_BasicCharacter* InTargetCharacter)
{
	return Super::ExecuteAIAttack(InTargetCharacter);
}

float AC_AR::GetDamageRateByBodyPart(const FName& BodyPart)
{
	if (!BODYPARTS_DAMAGERATE.Contains(BodyPart))
	{
		UC_Util::Print("From AC_SR::GetDamageRateByBodyPart : No Body part found!", FColor::Red, 10.f);
		return 0.f;
	}
	
	return BODYPARTS_DAMAGERATE[BodyPart];
}


