// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/ParkourActionStrategy/C_VaultLowActionStrategy.h"

#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"

#include "Character/Component/C_ParkourComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "MotionWarpingComponent.h"

#include "Utility/C_Util.h"

void UC_VaultLowActionStrategy::UseMotionWarpActionStrategy(AC_BasicCharacter* TargetCharacter, const FParkourDescriptor& CurParkourDesc)
{
	
	// WarpStartPos, WarpMiddlePos, WarpLandPos 잡아주기
	FVector WarpStartPos	= CurParkourDesc.VerticalHitPositions[0];
	FVector WarpMiddlePos	= CurParkourDesc.VerticalHitPositions.Last();
	FVector WarpLandPos		= CurParkourDesc.LandPos;
	
	// VaultLow 높이 조절용 값
	static const float START_Z_OFFSET  = 50.f; 
	static const float MIDDLE_Z_OFFSET = 120.f;

	WarpStartPos.Z  -= START_Z_OFFSET;
	WarpMiddlePos.Z -= MIDDLE_Z_OFFSET;

	// LandPos 예외처리
	if (!CurParkourDesc.bLandPosInited || FMath::Abs(CurParkourDesc.LandPos.Z - TargetCharacter->GetActorLocation().Z) > 50.f)
	{
		// 캐릭터의 현재 발 높이
		static const FName RIGHT_TOE_NAME = "RightToeBase";
		float FootZ = TargetCharacter->GetMesh()->GetBoneTransform(RIGHT_TOE_NAME).GetTranslation().Z;

		WarpLandPos   = WarpMiddlePos + TargetCharacter->GetActorForwardVector() * 80.f;
		WarpLandPos.Z = FootZ;
	}

	//UC_Util::Print(WarpStartPos, FColor::Red, 10.f);
	//UC_Util::Print(WarpMiddlePos, FColor::Red, 10.f);
	//UC_Util::Print(WarpLandPos, FColor::Red, 10.f);


	DrawDebugSphere(TargetCharacter->GetWorld(), WarpStartPos, 5.f, 4, FColor::Yellow, true);
	DrawDebugSphere(TargetCharacter->GetWorld(), WarpMiddlePos, 5.f, 4, FColor::Yellow, true);
	DrawDebugSphere(TargetCharacter->GetWorld(), WarpLandPos, 5.f, 4, FColor::Yellow, true);

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

	// TODO : ParkourMontageMap 초기화

	UC_ParkourComponent* TargetParkourComponent = TargetCharacter->GetParkourComponent();

	// 현재 Parkour Action에 해당하는 Random한 parkourAction 재생
	TArray<FPriorityAnimMontage> LowVaultMontageActions = TargetParkourComponent->GetParkourMontages(EParkourActionType::VAULTING_LOW);
	int RandIdx = FMath::RandRange(0, LowVaultMontageActions.Num() - 1);

	TargetCharacter->PlayAnimMontage(LowVaultMontageActions[RandIdx]);
}


