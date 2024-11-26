// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/Gun/C_AR.h"
#include "Item/Attachment/C_AttachableItem.h"
#include "Character/C_Player.h"


AC_AR::AC_AR()
{
	PrimaryActorTick.bCanEverTick = true;
	//ItemType ¼³Á¤.
	ItemDatas.ItemType = EItemTypes::MAINGUN;
}

void AC_AR::BeginPlay()
{
	Super::BeginPlay();

}

void AC_AR::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AC_AR::ExecuteReloadMontage()
{
	if (CurBulletCount > 0)
	{
		return;
	}
	AC_Player* CurPlayer = Cast<AC_Player>(OwnerCharacter);
	if (CurBulletCount == MaxBulletCount) return;
	if (!CurPlayer->GetCanMove()) return;
	if (CurPlayer->GetMesh()->GetAnimInstance()->Montage_IsPlaying(ReloadMontages[OwnerCharacter->GetPoseState()].Montages[CurState].AnimMontage))	return;
	SetMagazineVisibility(false);
	OwnerCharacter->SetIsReloadingBullet(true);
	OwnerCharacter->PlayAnimMontage(ReloadMontages[OwnerCharacter->GetPoseState()].Montages[CurState]);
	BackToMainCamera();
}
