// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/ParkourActionStrategy/C_MantleHighActionStrategy.h"

#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"

#include "Character/Component/C_ParkourComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "MotionWarpingComponent.h"

void UC_MantleHighActionStrategy::UseMotionWarpActionStrategy(AC_BasicCharacter* TargetCharacter, const FParkourDescriptor& CurParkourDesc)
{
	// TODO : Motion Warp -> Root �����̶� manual�ϰ� Z��ġ ������� ��
	FVector WarpStartPos = CurParkourDesc.VerticleHitPositions[0];
	FVector WarpLandPos  = CurParkourDesc.VerticleHitPositions[1];
	
	UMotionWarpingComponent* MotionWarping = TargetCharacter->GetMotionWarpingComponent();

	FMotionWarpingTarget Target{};
	Target.Name		= FName(TEXT("MantleStart"));
	Target.Location = WarpStartPos;
	Target.Rotation = TargetCharacter->GetActorRotation();

	MotionWarping->AddOrUpdateWarpTarget(Target);

	Target = {};
	Target.Name		= FName(TEXT("MantleEnd"));
	Target.Location = WarpLandPos;

	MotionWarping->AddOrUpdateWarpTarget(Target);

	// TODO : ParkourMontageMap �ʱ�ȭ

	UC_ParkourComponent* TargetParkourComponent = TargetCharacter->GetParkourComponent();

	// ���� Parkour Action�� �ش��ϴ� Random�� parkourAction ���
	TArray<FPriorityAnimMontage> HighMantleMontages = TargetParkourComponent->GetParkourMontages(EParkourActionType::MANTLING_HIGH);
	int RandIdx = FMath::RandRange(0, HighMantleMontages.Num() - 1);

	TargetCharacter->PlayAnimMontage(HighMantleMontages[RandIdx]);
}
