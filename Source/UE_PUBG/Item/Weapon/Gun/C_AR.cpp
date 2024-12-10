// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/Gun/C_AR.h"
#include "Item/Attachment/C_AttachableItem.h"
#include "Character/C_Player.h"


AC_AR::AC_AR()
{
	PrimaryActorTick.bCanEverTick = true;
	//ItemType 설정.
	ItemDatas.ItemType = EItemTypes::MAINGUN;
	CurGunType = EGunType::AR;
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
	if (CurBulletCount > 0)
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
