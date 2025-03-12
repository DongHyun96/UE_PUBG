// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/Gun/C_SR.h"
#include "Character/C_Player.h"
#include "Utility/C_Util.h"

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

	{"LeftArm",	0.9f},
	{"LeftHand",	0.9f},

	{"RightArm",	0.9f},
	{"RightHand",	0.9f}
};


AC_SR::AC_SR()
{
	PrimaryActorTick.bCanEverTick = true;
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
	if (!IsValid(OwnerCharacter)) return false;
	if (SniperReloadMontages.IsEmpty()) return false;
	

	AC_Player* CurPlayer = Cast<AC_Player>(OwnerCharacter);

	if (CurPlayer->GetMesh()->GetAnimInstance()->Montage_IsPlaying(ReloadMontages[OwnerCharacter->GetPoseState()].Montages[CurState].AnimMontage))	return false;

	if (CurBulletCount > 0 && CurrentShootingMode == EShootingMode::SINGLE_SHOT &&!IsReloadingSR)
	{
		UAnimMontage* DrawMontage = SniperReloadMontages[OwnerCharacter->GetPoseState()].AnimMontage;
		OwnerCharacter->PlayAnimMontage(SniperReloadMontages[OwnerCharacter->GetPoseState()]);
		UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
		OwnerCharacter->SetHandState(EHandState::WEAPON_GUN);
		AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharacter);
		//OwnerPlayer->GetCrosshairWidgetComponent()->SetCrosshairState(ECrosshairState::RIFLE);
		bIsSniperReload = true;
		OwnerCharacter->SetIsReloadingBullet(true);
		BackToMainCamera();

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

bool AC_SR::ExecuteAIAttack(AC_BasicCharacter* InTargetCharacter)
{
	return Super::ExecuteAIAttack(InTargetCharacter);
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

//bool AC_SR::ReloadBullet()
//{
//	Super::ReloadBullet();
//	IsReloadingSR = false;
//	return false;
//}


