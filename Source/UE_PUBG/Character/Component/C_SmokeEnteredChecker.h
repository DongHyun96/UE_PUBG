// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_SmokeEnteredChecker.generated.h"

/// <summary>
/// <para> OwnerCharacter가 현재 Smoke Area에 들어와 있는지 체크하는 ActorComponent Class </para>
/// <para> OwnerCharacter가 현재 들어와 있는 Smoke Areas들의 Random한 Location을 구할 때에도 이 Component 사용 </para>
/// </summary>
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UE_PUBG_API UC_SmokeEnteredChecker : public UActorComponent
{
	GENERATED_BODY()

public:
	UC_SmokeEnteredChecker();

protected:
	
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


public:

	void SetOwnerCharacter(class AC_BasicCharacter* InOwnerCharacter) { OwnerCharacter = InOwnerCharacter; }
	
	/// <summary>
	/// 현재 OwnerCharacter가 Smoke Area에 하나라도 들어와 있는지 체크 
	/// </summary>
	bool IsCurrentlyInSmokeArea() const { return !EnteredSmokes.IsEmpty(); }

	/// <summary>
	/// OwnerCharacter가 들어가 있는 Smoke Area들의 내부 위치 중 랜덤한 Location 구하기
	/// </summary>
	/// <param name="OutLocation"> : 구한 Random location </param>
	/// <returns> : 만약 아무 Smoke Area에 들어가 있지 않다면 return false </returns>
	bool GetRandomLocationInSmokeArea(FVector& OutLocation);
	
public:
	
	/// <summary>
	/// EnteredSmokes에 새로운 Smoke 추가 
	/// </summary>
	/// <param name="InSmoke"> : 해당 Smoke Grenade의 SmokeOverlappedHandler Component 객체 </param>
	void AddEnteredSmoke(class UC_SmokeOverlappedHandler* InSmoke);

	/// <summary>
	/// EnteredSmokes에서 Smoke 지우기 
	/// </summary>
	/// <param name="TargetSmoke"> : 해당 Smoke Grenade의 SmokeOverlappedHandler Component 객체 </param>
	void RemoveEnteredSmoke(UC_SmokeOverlappedHandler* TargetSmoke);

private:
	
	/// <summary>
	/// Entered Smoke들 중 Smoke Duration이 끝났을 때 callback 받을 함수
	/// </summary>
	void OnEnteredSmokeDurationFinished(UC_SmokeOverlappedHandler* FinishedSmoke) { EnteredSmokes.Remove(FinishedSmoke); }
	
private:

	AC_BasicCharacter* OwnerCharacter{};

private:

	/// <summary>
	/// 현재 안에 들어가져 있는 연막탄의 OverlappedHandlers
	/// </summary>
	TArray<UC_SmokeOverlappedHandler*> EnteredSmokes{};
	
};
