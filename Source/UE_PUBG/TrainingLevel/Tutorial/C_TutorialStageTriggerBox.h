// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "C_TutorialStageTriggerBox.generated.h"

UCLASS()
class UE_PUBG_API AC_TutorialStageTriggerBox : public ATriggerBox
{
	GENERATED_BODY()

public:
	
	AC_TutorialStageTriggerBox();

protected:
	
	virtual void BeginPlay() override;

public:
	
	virtual void Tick(float DeltaTime) override;

	/// <summary>
	/// Start trigger box 활성화 / 비활성화
	/// </summary>
	/// <param name="InIsEnabled"></param>
	void ToggleTriggerBox(bool InIsEnabled);

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UUserWidget* TriangleWidget{};
	
};
