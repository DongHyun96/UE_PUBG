// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "C_ShootingTargetWidgetsHolder.generated.h"

/// <summary>
/// Shooting Target Damage 표기 Widgets Holder 클래스 (ShootingTarget들이 모두 돌아가며 이 객체를 통해 Damage를 표기 처리
/// </summary>
UCLASS()
class UE_PUBG_API AC_ShootingTargetWidgetsHolder : public AActor
{
	GENERATED_BODY()

public:
	AC_ShootingTargetWidgetsHolder();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	/// <summary>
	/// Damage 정보 Widget 스폰시키기
	/// </summary>
	/// <param name="bIsHeadShot"> : HeadShot인지 여부 </param>
	/// <param name="DamageAmount"> : Damage 총량 </param>
	/// <param name="WorldLocation"> : Spawn시킬 World 위치 </param>
	void SpawnDamageInfoWidget(bool bIsHeadShot, float DamageAmount, const FVector& WorldLocation);

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TArray<class UWidgetComponent*> DamageWidgetComponents{};

	
private:
	const uint8 WidgetPoolCount = 10;

	// Pool이 모두 사용중이더라도, 가장 Spawn Animation이 거의 재생된 Widget을 재사용 처리할 예정
	TQueue<UWidgetComponent*> DamageWidgetQueue{};
	
};
