// Fill out your copyright notice in the Description page of Project Settings.


#include "C_WeaponTutorialChecker.h"

UC_WeaponTutorialChecker::UC_WeaponTutorialChecker()
{
	PrimaryComponentTick.bCanEverTick = true;

	// 무기와 탄환 획득하기
	FGoalData GatherWeaponsAndAmmo{};
	GatherWeaponsAndAmmo.SubGoalsAchieved.Init(false, 2);
	GoalData.Add(GatherWeaponsAndAmmo);

	// R 재장전하기
	GoalData.Add(FGoalData());

	// 사격장으로 이동하기
	GoalData.Add(FGoalData());

	// 스코프 모드에서 목표물 명중하기
	FGoalData HitTargetWhileAimDownSight{};
	HitTargetWhileAimDownSight.SubGoalsAchieved.Init(false, 10);
	GoalData.Add(HitTargetWhileAimDownSight);
}

void UC_WeaponTutorialChecker::BeginPlay()
{
	Super::BeginPlay();
}

void UC_WeaponTutorialChecker::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
