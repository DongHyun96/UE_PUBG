// Fill out your copyright notice in the Description page of Project Settings.


#include "Sound/C_AnimNotify_FootStep.h"

#include "AssetUserData/C_FootStepFallingData.h"
#include "Character/C_AnimBasicCharacter.h"
#include "Character/C_BasicCharacter.h"

void UC_AnimNotify_FootStep::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	
    AC_BasicCharacter* Character = Cast<AC_BasicCharacter>(MeshComp->GetOwner());
    if (!Character)
    {
		bIsValidToPlaySound = false;
    	return;
    }

	UC_FootStepFallingData* bHasFallingData = Animation->GetAssetUserData<UC_FootStepFallingData>();
	
	// Set SoundLocation
	SoundLocation = MeshComp->GetSocketLocation(FootSocketName);

	// Set FootSoundVolume
    UC_AnimBasicCharacter* AnimBasicCharacter = Cast<UC_AnimBasicCharacter>(MeshComp->GetAnimInstance());
    if (!AnimBasicCharacter)
    {
		bIsValidToPlaySound = false;
    	return;
    }

    const FVector2D SpeedRange = {0.f, 630.f};
    const FVector2D VolumeRange = {0.5f, 1.f};
    FootSoundVolume = bHasFallingData ? 1.f : FMath::GetMappedRangeValueClamped(SpeedRange, VolumeRange, AnimBasicCharacter->GetSpeed());

	bIsValidToPlaySound = true;
}
