// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/ParkourActionStrategy/C_VaultLowActionStrategy.h"

#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"

#include "Character/Component/C_ParkourComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "MotionWarpingComponent.h"

void UC_VaultLowActionStrategy::UseMotionWarpActionStrategy(AC_BasicCharacter* TargetCharacter, const FParkourDescriptor& CurParkourDesc)
{
	TargetCharacter->SetCanMove(false);
	TargetCharacter->SetNextSpeed(0.f);
	if (AC_Player* Player = Cast<AC_Player>(TargetCharacter)) Player->SetStrafeRotationToIdleStop();

	UC_ParkourComponent* TargetParkourComponent = TargetCharacter->GetParkourComponent();

	TargetParkourComponent->SwapMesh(true);

	TargetCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);

	TargetCharacter->SetActorEnableCollision(false);
	UMotionWarpingComponent* MotionWarping = TargetCharacter->GetMotionWarpingComponent();

	// TODO : WarpStartPos Middle LandPos 여기서 지정해서 해주기
	FMotionWarpingTarget Target{};
	Target.Name = FName(TEXT("VaultStart"));
	Target.Location = CurParkourDesc.WarpStartPos;
	Target.Rotation = TargetCharacter->GetActorRotation();

	MotionWarping->AddOrUpdateWarpTarget(Target);

	Target = {};
	Target.Name = FName(TEXT("VaultMiddle"));
	Target.Location = CurParkourDesc.WarpMiddlePos;

	MotionWarping->AddOrUpdateWarpTarget(Target);

	Target = {};
	Target.Name = FName(TEXT("VaultLand"));
	Target.Location = CurParkourDesc.WarpLandPos;

	MotionWarping->AddOrUpdateWarpTarget(Target);

	// TODO : ParkourMontageMap 초기화

	// 현재 Parkour Action에 해당하는 Random한 parkourAction 재생
	TArray<FPriorityAnimMontage> LowVaultMontageActions = TargetParkourComponent->GetParkourMontages(EParkourActionType::VAULTING_LOW);
	int RandIdx = FMath::RandRange(0, LowVaultMontageActions.Num() - 1);

	TargetCharacter->PlayAnimMontage(LowVaultMontageActions[RandIdx]);
}
