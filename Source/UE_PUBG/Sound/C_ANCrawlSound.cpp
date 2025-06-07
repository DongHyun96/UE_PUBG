// Fill out your copyright notice in the Description page of Project Settings.


#include "C_ANCrawlSound.h"

#include "Character/C_Enemy.h"
#include "Character/C_Player.h"
#include "Kismet/GameplayStatics.h"
#include "Utility/C_Util.h"

void UC_ANCrawlSound::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	// TODO : 속도에 따른 volumeMultiplier 계산해서 적용

	if (AC_Player* Player = Cast<AC_Player>(MeshComp->GetOwner()))
	{
		UGameplayStatics::PlaySound2D(Player, CrawlSound);	
		return;
	}

	if (AC_Enemy* Enemy = Cast<AC_Enemy>(MeshComp->GetOwner()))
	{
		UGameplayStatics::PlaySoundAtLocation(Enemy, CrawlSound, Enemy->GetActorLocation());
		return;
	}

	UC_Util::Print("From UC_ANCrawlSound::Notify : MeshComp casting failed!", FColor::Red, 10.f);
}
