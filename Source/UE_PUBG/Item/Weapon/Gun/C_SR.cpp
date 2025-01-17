// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/Gun/C_SR.h"
#include "Character/C_Player.h"
#include "Utility/C_Util.h"
AC_SR::AC_SR()
{
	PrimaryActorTick.bCanEverTick = true;
	//ItemType 설정.
	ItemDatas.ItemType = EItemTypes::MAINGUN;
	CurGunType = EGunType::SR;
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
	if (!IsValid(OwnerCharacter)) return false;
	if (SniperReloadMontages.IsEmpty()) return false;
	

	AC_Player* CurPlayer = Cast<AC_Player>(OwnerCharacter);

	if (CurPlayer->GetMesh()->GetAnimInstance()->Montage_IsPlaying(ReloadMontages[OwnerCharacter->GetPoseState()].Montages[CurState].AnimMontage))	return false;

	if (CurBulletCount > 0)
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
		if (OwnerCharacter->GetCurrentSevenmmBulletCount() == 0) return false;
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
