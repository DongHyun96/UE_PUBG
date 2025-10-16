// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/ParkourActionStrategy/C_VaultHighActionStrategy.h"
#include "Character/C_BasicCharacter.h"
#include "Character/Component/C_ParkourComponent.h"
#include "MotionWarpingComponent.h"
#include "Utility/C_Util.h"

void UC_VaultHighActionStrategy::UseMotionWarpActionStrategy(AC_BasicCharacter* TargetCharacter, const FParkourDescriptor& CurParkourDesc)
{
	UC_Util::Print("VaultHigh Strategy", FColor::Red, 10.f);

	static const float CHARACTER_CLIMB_ACTION_HEIGHT = 200.f; // 캐릭터가 팔 뻗어서 오르려 할 때의 총장 길이
	static const float WARP_START_FORWARD_OFFSET	 = 25.f; // 벽면에 너무 달라붙어서 오르면 이상해서 offset 지정

	// 캐릭터의 현재 발 높이
	static const FName RIGHT_TOE_NAME = "RightToeBase";
	float FootWorldHeight			  = TargetCharacter->GetMesh()->GetBoneTransform(RIGHT_TOE_NAME).GetTranslation().Z;

	FVector WarpMiddlePos = CurParkourDesc.VerticalHitPositions[0] - TargetCharacter->GetActorForwardVector() * WARP_START_FORWARD_OFFSET;

	// Warp Start Pos 구하기
	FVector WarpStartPos = WarpMiddlePos - FVector::UnitZ() * CHARACTER_CLIMB_ACTION_HEIGHT;
	if (WarpStartPos.Z < FootWorldHeight) WarpStartPos.Z = FootWorldHeight; // 파쿠르 액션 동작 높이 예외처리

	FVector WarpLandPos = WarpMiddlePos + TargetCharacter->GetActorForwardVector() * 120.f;

	UWorld* TargetWorld = TargetCharacter->GetWorld();

	//DrawDebugSphere(TargetWorld, WarpStartPos, 5.f, 4, FColor::Red, true);
	//DrawDebugSphere(TargetWorld, WarpMiddlePos, 5.f, 4, FColor::Yellow, true);
	//DrawDebugSphere(TargetWorld, WarpLandPos, 5.f, 4, FColor::Blue, true);

	UMotionWarpingComponent* MotionWarping = TargetCharacter->GetMotionWarpingComponent();

	MotionWarping->RemoveAllWarpTargets();

	FMotionWarpingTarget Target{};
	Target.Name		= FName(TEXT("VaultStart"));
	Target.Location = WarpStartPos;
	Target.Rotation = TargetCharacter->GetActorRotation();

	MotionWarping->AddOrUpdateWarpTarget(Target);

	Target = {};
	Target.Name = FName(TEXT("VaultMiddle"));
	Target.Location = WarpMiddlePos;

	MotionWarping->AddOrUpdateWarpTarget(Target);

	Target = {};
	Target.Name		= FName(TEXT("VaultEnd"));
	Target.Location = WarpLandPos;

	MotionWarping->AddOrUpdateWarpTarget(Target);

	UC_ParkourComponent* TargetParkourComponent = TargetCharacter->GetParkourComponent();

	// 현재 Parkour Action에 해당하는 Random한 parkourAction 재생
	TArray<FPriorityAnimMontage> HighMantleMontages = TargetParkourComponent->GetParkourMontages(EParkourActionType::VAULTING_HIGH);
	int RandIdx = FMath::RandRange(0, HighMantleMontages.Num() - 1);

	TargetCharacter->PlayAnimMontage(HighMantleMontages[RandIdx]);

	// CanMove할 수 없는 시간대를 늘릴 예정
	TargetParkourComponent->SetCanMoveTimerAfterWarpActionFin(0.5f);
}


