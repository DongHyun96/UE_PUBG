// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/C_AnimPlayer.h"

#include "Character/C_Player.h"
#include "Character/Component/C_EquippedComponent.h"
#include "Utility/C_Util.h"

void UC_AnimPlayer::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	if (!IsValid(OwnerCharacter))
	{
		UC_Util::Print("From AnimPlayer NativeBeginPlay : Initing OwnerPlayer failed!", FColor::Red, 10.f);
		return;
	}

	OwnerPlayer = Cast<AC_Player>(OwnerCharacter);
}

void UC_AnimPlayer::SetAimOffsetRotation()
{
	Super::SetAimOffsetRotation();

	if (!IsValid(OwnerPlayer)) return;
	
	AC_Gun* CurGun = Cast<AC_Gun>(OwnerPlayer->GetEquippedComponent()->GetCurWeapon());

	if (OwnerPlayer->GetIsWatchingSight() && OwnerPlayer->GetPoseState() == EPoseState::CRAWL)
	{
		if (CCurrentAimOffsetRotation.Pitch < 0)
		{

			OwnerCharacter->GetMesh()->HideBoneByName(FName("LeftArm"), EPhysBodyOp::PBO_None);
			OwnerCharacter->GetMesh()->HideBoneByName(FName("RightArm"), EPhysBodyOp::PBO_None);
			CurGun->GetGunMesh()->SetVisibility(false,true);
		}
		else
		{

			OwnerCharacter->GetMesh()->UnHideBoneByName(FName("LeftArm"));
			OwnerCharacter->GetMesh()->UnHideBoneByName(FName("RightArm"));
			CurGun->GetGunMesh()->SetVisibility(true,true);

		}
	}
	else
	{
		OwnerCharacter->GetMesh()->UnHideBoneByName(FName("LeftArm"));
		OwnerCharacter->GetMesh()->UnHideBoneByName(FName("RightArm"));
		if(IsValid(CurGun))
			CurGun->GetGunMesh()->SetVisibility(true,true);

	}

}
