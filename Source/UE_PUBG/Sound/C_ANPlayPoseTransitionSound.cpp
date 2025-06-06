// Fill out your copyright notice in the Description page of Project Settings.


#include "C_ANPlayPoseTransitionSound.h"

#include "Character/C_BasicCharacter.h"
#include "Character/C_Enemy.h"
#include "Character/C_Player.h"
#include "Kismet/GameplayStatics.h"
#include "Utility/C_Util.h"

void UC_ANPlayPoseTransitionSound::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (!PoseTransitionSound)
	{
		UC_Util::Print("From UC_ANPlayPoseTransitionSound::Notify : PoseTransitionSound not initialized!", FColor::Red, 10.f);
		return;
	}

	if (AC_Player* Player = Cast<AC_Player>(MeshComp->GetOwner()))
	{
		UGameplayStatics::PlaySound2D(Player, PoseTransitionSound);
		return;
	}

	if (AC_Enemy* Enemy = Cast<AC_Enemy>(MeshComp->GetOwner()))
	{
		UGameplayStatics::PlaySoundAtLocation(Enemy, PoseTransitionSound, Enemy->GetActorLocation());
		return;
	}

	UC_Util::Print("From UC_ANPlayPoseTransitionSound::Notify : Casting to Player or Enemy all failed!", FColor::Red, 10.f);
	
	
}
