// Fill out your copyright notice in the Description page of Project Settings.


#include "C_ANCharacterSound.h"

#include "Character/C_Enemy.h"
#include "Character/C_Player.h"
#include "CompGeom/FitOrientedBox3.h"
#include "Kismet/GameplayStatics.h"
#include "Utility/C_Util.h"

void UC_ANCharacterSound::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!IsValid(Sound)) return;

	if (AC_Player* Player = Cast<AC_Player>(MeshComp->GetOwner()))
	{
		UGameplayStatics::PlaySound2D(Player, Sound, VolumeMultiplier, PitchMultiplier);
		return;
	}

	if (AC_Enemy* Enemy = Cast<AC_Enemy>(MeshComp->GetOwner()))
	{
		if (bOnlyPlayPlayerSound) return;
		UGameplayStatics::PlaySoundAtLocation(Enemy, Sound, Enemy->GetActorLocation(), VolumeMultiplier, PitchMultiplier);
	}
}
