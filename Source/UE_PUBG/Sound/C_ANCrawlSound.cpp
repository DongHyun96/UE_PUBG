// Fill out your copyright notice in the Description page of Project Settings.


#include "C_ANCrawlSound.h"

#include "Character/C_AnimBasicCharacter.h"
#include "Character/C_Enemy.h"
#include "Character/C_Player.h"
#include "Kismet/GameplayStatics.h"
#include "Utility/C_Util.h"

void UC_ANCrawlSound::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (AC_Player* Player = Cast<AC_Player>(MeshComp->GetOwner()))
	{
		// Set foot sound volume (속도에 따른 volumeMultiplier 계산해서 적용)
		UC_AnimBasicCharacter* AnimBasicCharacter = Cast<UC_AnimBasicCharacter>(MeshComp->GetAnimInstance());
		const FVector2D SpeedRange  = {60.f, 120.f};
		const FVector2D VolumeRange = {0.65f, 1.f};
		float VolumeMultiplier = FMath::GetMappedRangeValueClamped(SpeedRange, VolumeRange, AnimBasicCharacter->GetSpeed());
		
		UGameplayStatics::PlaySound2D(Player, CrawlSound, VolumeMultiplier);
		return;
	}

	if (AC_Enemy* Enemy = Cast<AC_Enemy>(MeshComp->GetOwner()))
	{
		// Set foot sound volume (속도에 따른 volumeMultiplier 계산해서 적용)
		UC_AnimBasicCharacter* AnimBasicCharacter = Cast<UC_AnimBasicCharacter>(MeshComp->GetAnimInstance());
		const FVector2D SpeedRange  = {60.f, 120.f};
		const FVector2D VolumeRange = {0.65f, 1.f};
		float VolumeMultiplier = FMath::GetMappedRangeValueClamped(SpeedRange, VolumeRange, AnimBasicCharacter->GetSpeed());
		
		UGameplayStatics::PlaySoundAtLocation(Enemy, CrawlSound, Enemy->GetActorLocation(), VolumeMultiplier);
		return;
	}

	UC_Util::Print("From UC_ANCrawlSound::Notify : MeshComp casting failed!", FColor::Red, 10.f);
}
