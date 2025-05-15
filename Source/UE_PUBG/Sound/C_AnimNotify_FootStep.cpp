// Fill out your copyright notice in the Description page of Project Settings.


#include "Sound/C_AnimNotify_FootStep.h"
#include "C_AnimNotify_FootStep.h"
#include "Character/C_BasicCharacter.h"
#include "Character/Component/C_FeetComponent.h"

void UC_AnimNotify_FootStep::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    AC_BasicCharacter* Character = Cast<AC_BasicCharacter>(MeshComp->GetOwner());
    if (!Character) return;

    EPhysicalSurface SurfaceType = Character->GetFeetComponent()->GetLeftSurfaceType();

    //USoundBase* FootstepSound = Character->GetFootstepSoundForSurface(SurfaceType, bIsLeftFoot);
    //if (FootstepSound)
    //{
    //    UGameplayStatics::PlaySoundAtLocation(Character, FootstepSound, Character->GetActorLocation());
    //}
}
