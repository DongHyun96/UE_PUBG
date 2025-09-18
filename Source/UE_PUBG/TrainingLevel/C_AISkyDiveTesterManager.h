// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "C_AISkyDiveTesterManager.generated.h"

/// <summary>
/// Training Ground 내의 AI SkyDive용 SkyDive Start Location, Landing Location 구하기 처리 및 etc. 
/// </summary>
UCLASS()
class UE_PUBG_API AC_AISkyDiveTesterManager : public AActor
{
	GENERATED_BODY()

public:
	
	AC_AISkyDiveTesterManager();

protected:
	
	virtual void BeginPlay() override;

public:
	
	virtual void Tick(float DeltaTime) override;

public:

	FVector GetRandomSkyDiveStartLocation() const {return GetRandomLocationInBox(SkyDiveStartAreaBoxComponent); }
	FVector GetRandomSkyDiveLandingLocation() const {return GetRandomLocationInBox(SkyDiveLandingAreaBoxComponent); }

	class AC_Enemy* GetSkyDiveTester() const { return SkyDiveTesterEnemy; }

	class UC_JumpTrainingWidget* GetJumpTrainingWidget() const { return JumpTrainingWidget; }

	/// <summary>
	/// SkyDiveTester 관전 카메라 전환 켜기/끄기
	/// </summary>
	void ToggleSpectatingSkyDiveTester();

	bool GetIsSpectatingSkyDiveTester() const { return bIsSpectatingSkyDiveTester; }

private:

	FVector GetRandomLocationInBox(class UBoxComponent* Box) const;

private:

	UBoxComponent* SkyDiveStartAreaBoxComponent{};
	UBoxComponent* SkyDiveLandingAreaBoxComponent{};

protected:

	UPROPERTY(BlueprintReadWrite, EditInstanceOnly)
	AC_Enemy* SkyDiveTesterEnemy{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UC_JumpTrainingWidget* JumpTrainingWidget{};
	
private:

	// 현재 SkyDiveTester를 관전 중인지
	bool bIsSpectatingSkyDiveTester{};
};
