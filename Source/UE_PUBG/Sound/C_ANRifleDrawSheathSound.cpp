// Fill out your copyright notice in the Description page of Project Settings.


#include "Sound/C_ANRifleDrawSheathSound.h"

#include "Character/C_Enemy.h"
#include "Character/C_Player.h"
#include "Character/Component/C_EquippedComponent.h"
#include "Item/Weapon/Gun/C_AR.h"
#include "Item/Weapon/Gun/C_SR.h"
#include "Kismet/GameplayStatics.h"
#include "Utility/C_Util.h"

const int UC_ANRifleDrawSheathSound::ARIndex = 0;
const int UC_ANRifleDrawSheathSound::SRIndex = 1;


void UC_ANRifleDrawSheathSound::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	AC_BasicCharacter* Character = Cast<AC_BasicCharacter>(MeshComp->GetOwner());
	if (!Character) return;

	UC_EquippedComponent* EquippedComponent = Character->GetEquippedComponent();
	
	if (bIsDrawSoundNotify)
	{
		EWeaponSlot NextWeaponSlot = EquippedComponent->GetNextWeaponType();
		
		if (Cast<AC_AR>(EquippedComponent->GetWeapons()[NextWeaponSlot]))
			PlaySound(DrawSounds[ARIndex], Character);
		
		else if (Cast<AC_SR>(EquippedComponent->GetWeapons()[NextWeaponSlot]))
			PlaySound(DrawSounds[SRIndex], Character);
			
		else UC_Util::Print("From UC_ANRifleDrawSheathSound::Notify : Weapon casting to Gun failed!");
			
		return;
	}

	// SheathSound Notify
	if (Cast<AC_AR>(EquippedComponent->GetCurWeapon()))
		PlaySound(SheathSounds[ARIndex], Character);
		
	else if (Cast<AC_SR>(EquippedComponent->GetCurWeapon()))
		PlaySound(SheathSounds[SRIndex], Character);
			
	else UC_Util::Print("From UC_ANRifleDrawSheathSound::Notify : Weapon casting to Gun failed!");
	

	/*// AR or SR 종류 결정
	if (Cast<AC_AR>(Character->GetEquippedComponent()->GetCurWeapon()))
	{
		// AR일 경우
		
		if (bIsDrawSoundNotify) PlaySound(DrawSounds[ARIndex], Character);
		else PlaySound(SheathSounds[ARIndex], Character);
	}
	else if (Cast<AC_SR>(Character->GetEquippedComponent()->GetCurWeapon()))
	{
		// SR일 경우
		if (bIsDrawSoundNotify) PlaySound(DrawSounds[SRIndex], Character);
		else PlaySound(SheathSounds[SRIndex], Character);
	}
	else UC_Util::Print("From UC_ANRifleDrawSheathSound::Notify : Cur Weapon is not gun type!");*/
	
}

void UC_ANRifleDrawSheathSound::PlaySound(USoundBase* TargetSound, AC_BasicCharacter* TargetCharacter)
{
	if (Cast<AC_Player>(TargetCharacter)) UGameplayStatics::PlaySound2D(TargetCharacter, TargetSound);
	else UGameplayStatics::PlaySoundAtLocation(TargetCharacter, TargetSound, TargetCharacter->GetActorLocation());
}

