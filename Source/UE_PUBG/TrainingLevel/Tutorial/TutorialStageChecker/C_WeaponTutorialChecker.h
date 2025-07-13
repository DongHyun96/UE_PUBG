// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "C_TutorialStageChecker.h"
#include "C_WeaponTutorialChecker.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UE_PUBG_API UC_WeaponTutorialChecker : public UC_TutorialStageChecker
{
	GENERATED_BODY()

public:
	
	UC_WeaponTutorialChecker();

protected:
	
	virtual void BeginPlay() override;

public:
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	void InitStage() override;
};
