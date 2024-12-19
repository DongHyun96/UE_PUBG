// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/ParkourActionStrategy/C_VaultHighActionStrategy.h"

#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"

#include "Character/Component/C_ParkourComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "MotionWarpingComponent.h"

void UC_VaultHighActionStrategy::UseMotionWarpActionStrategy(AC_BasicCharacter* TargetCharacter, const FParkourDescriptor& CurParkourDesc)
{
	// TODO : Vault 시 LandPos 허공인지 체크해서 다르게 처리해주어야 함

	// WarpStartPos, WarpMiddlePos, WarpLandPos 잡아주기
	FVector WarpStartPos	= CurParkourDesc.VerticleHitPositions[0];
	FVector WarpMiddlePos	= CurParkourDesc.VerticleHitPositions.Last();
	FVector WarpLandPos		= CurParkourDesc.LandPos;

	UMotionWarpingComponent* MotionWarping = TargetCharacter->GetMotionWarpingComponent();

	// TODO : WarpStartPos Middle LandPos 여기서 지정해서 해주기
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

	// TODO : ParkourMontageMap 초기화

	UC_ParkourComponent* TargetParkourComponent = TargetCharacter->GetParkourComponent();

	// 현재 Parkour Action에 해당하는 Random한 parkourAction 재생
	// TODO : Vaulting High로 수정할 것
	TArray<FPriorityAnimMontage> HighVaultMontageActions = TargetParkourComponent->GetParkourMontages(EParkourActionType::VAULTING_LOW);
	int RandIdx = FMath::RandRange(0, HighVaultMontageActions.Num() - 1);

	TargetCharacter->PlayAnimMontage(HighVaultMontageActions[RandIdx]);
}
