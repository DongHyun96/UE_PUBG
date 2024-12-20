// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/ParkourActionStrategy/C_VaultLowActionStrategy.h"

#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"

#include "Character/Component/C_ParkourComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "MotionWarpingComponent.h"

void UC_VaultLowActionStrategy::UseMotionWarpActionStrategy(AC_BasicCharacter* TargetCharacter, const FParkourDescriptor& CurParkourDesc)
{
	// TODO : Vault �� LandPos ������� üũ�ؼ� �ٸ��� ó�����־�� ��

	// WarpStartPos, WarpMiddlePos, WarpLandPos ����ֱ�
	FVector WarpStartPos	= CurParkourDesc.VerticalHitPositions[0];
	FVector WarpMiddlePos	= CurParkourDesc.VerticalHitPositions.Last();
	FVector WarpLandPos		= CurParkourDesc.LandPos;

	UMotionWarpingComponent* MotionWarping = TargetCharacter->GetMotionWarpingComponent();

	MotionWarping->RemoveAllWarpTargets();

	FMotionWarpingTarget Target{};
	Target.Name = FName(TEXT("VaultStart"));
	Target.Location = WarpStartPos;
	Target.Rotation = TargetCharacter->GetActorRotation();

	MotionWarping->AddOrUpdateWarpTarget(Target);

	Target = {};
	Target.Name = FName(TEXT("VaultMiddle"));
	Target.Location = WarpMiddlePos;

	MotionWarping->AddOrUpdateWarpTarget(Target);

	Target = {};
	Target.Name = FName(TEXT("VaultLand"));
	Target.Location = WarpLandPos;

	MotionWarping->AddOrUpdateWarpTarget(Target);

	// TODO : ParkourMontageMap �ʱ�ȭ

	UC_ParkourComponent* TargetParkourComponent = TargetCharacter->GetParkourComponent();

	// ���� Parkour Action�� �ش��ϴ� Random�� parkourAction ���
	TArray<FPriorityAnimMontage> LowVaultMontageActions = TargetParkourComponent->GetParkourMontages(EParkourActionType::VAULTING_LOW);
	int RandIdx = FMath::RandRange(0, LowVaultMontageActions.Num() - 1);

	TargetCharacter->PlayAnimMontage(LowVaultMontageActions[RandIdx]);
}
