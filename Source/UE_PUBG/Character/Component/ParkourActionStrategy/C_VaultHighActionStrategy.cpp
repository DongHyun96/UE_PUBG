// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/ParkourActionStrategy/C_VaultHighActionStrategy.h"

#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"

#include "Character/Component/C_ParkourComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "MotionWarpingComponent.h"

#include "Utility/C_Util.h"

void UC_VaultHighActionStrategy::UseMotionWarpActionStrategy(AC_BasicCharacter* TargetCharacter, const FParkourDescriptor& CurParkourDesc)
{
	// TODO : Vault �� LandPos ������� üũ�ؼ� �ٸ��� ó�����־�� ��

	UC_Util::Print("VaultHigh Strategy", FColor::Red, 10.f);

	static const float CHARACTER_CLIMB_ACTION_HEIGHT = 200.f; // ĳ���Ͱ� �� ��� ������ �� ���� ���� ����
	static const float WARP_START_FORWARD_OFFSET	 = 25.f; // ���鿡 �ʹ� �޶�پ ������ �̻��ؼ� offset ����

	// ĳ������ ���� �� ����
	static const FName RIGHT_TOE_NAME = "RightToeBase";
	float FootWorldHeight			  = TargetCharacter->GetMesh()->GetBoneTransform(RIGHT_TOE_NAME).GetTranslation().Z;

	FVector WarpMiddlePos = CurParkourDesc.VerticalHitPositions[0] - TargetCharacter->GetActorForwardVector() * WARP_START_FORWARD_OFFSET;

	// Warp Start Pos ���ϱ�
	FVector WarpStartPos = WarpMiddlePos - FVector::UnitZ() * CHARACTER_CLIMB_ACTION_HEIGHT;
	if (WarpStartPos.Z < FootWorldHeight) WarpStartPos.Z = FootWorldHeight; // ������ �׼� ���� ���� ����ó��

	FVector WarpLandPos = WarpMiddlePos + TargetCharacter->GetActorForwardVector() * 120.f;

	UWorld* TargetWorld = TargetCharacter->GetWorld();

	DrawDebugSphere(TargetWorld, WarpStartPos, 5.f, 4, FColor::Red, true);
	DrawDebugSphere(TargetWorld, WarpMiddlePos, 5.f, 4, FColor::Yellow, true);
	DrawDebugSphere(TargetWorld, WarpLandPos, 5.f, 4, FColor::Blue, true);

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

	// TODO : ParkourMontageMap �ʱ�ȭ

	UC_ParkourComponent* TargetParkourComponent = TargetCharacter->GetParkourComponent();

	// ���� Parkour Action�� �ش��ϴ� Random�� parkourAction ���
	TArray<FPriorityAnimMontage> HighMantleMontages = TargetParkourComponent->GetParkourMontages(EParkourActionType::VAULTING_HIGH);
	int RandIdx = FMath::RandRange(0, HighMantleMontages.Num() - 1);

	TargetCharacter->PlayAnimMontage(HighMantleMontages[RandIdx]);

	// CanMove�� �� ���� �ð��븦 �ø� ����
	TargetParkourComponent->SetCanMoveTimerAfterWarpActionFin(0.5f);
}