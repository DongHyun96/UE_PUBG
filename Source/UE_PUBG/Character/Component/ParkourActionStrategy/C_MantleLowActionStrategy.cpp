// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/ParkourActionStrategy/C_MantleLowActionStrategy.h"

#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"

#include "Character/Component/C_ParkourComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "MotionWarpingComponent.h"

void UC_MantleLowActionStrategy::UseMotionWarpActionStrategy(AC_BasicCharacter* TargetCharacter, const FParkourDescriptor& CurParkourDesc)
{
	FVector WarpStartPos = CurParkourDesc.VerticalHitPositions[0];
	FVector WarpLandPos = CurParkourDesc.VerticalHitPositions[1];
	
	UMotionWarpingComponent* MotionWarping = TargetCharacter->GetMotionWarpingComponent();

	MotionWarping->RemoveAllWarpTargets();

	FMotionWarpingTarget Target{};
	Target.Name		= FName(TEXT("MantleStart"));
	Target.Location = WarpStartPos;
	Target.Rotation = TargetCharacter->GetActorRotation();

	MotionWarping->AddOrUpdateWarpTarget(Target);

	Target = {};
	Target.Name		= FName(TEXT("MantleEnd"));
	Target.Location = WarpLandPos;

	MotionWarping->AddOrUpdateWarpTarget(Target);

	// TODO : ParkourMontageMap 초기화

	UC_ParkourComponent* TargetParkourComponent = TargetCharacter->GetParkourComponent();

	// 현재 Parkour Action에 해당하는 Random한 parkourAction 재생
	TArray<FPriorityAnimMontage> LowMantleMontages = TargetParkourComponent->GetParkourMontages(EParkourActionType::MANTLING_LOW);
	int RandIdx = FMath::RandRange(0, LowMantleMontages.Num() - 1);

	TargetCharacter->PlayAnimMontage(LowMantleMontages[RandIdx]);
	
}
