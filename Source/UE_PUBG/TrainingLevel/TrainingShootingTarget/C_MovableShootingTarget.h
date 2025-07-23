// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "C_TrainingShootingTarget.h"
#include "C_MovableShootingTarget.generated.h"

UCLASS()
class UE_PUBG_API AC_MovableShootingTarget : public AC_TrainingShootingTarget
{
	GENERATED_BODY()

public:
	AC_MovableShootingTarget();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

protected: // 좌, 우 끝점 Anchor 역할 (좌, 우를 맞출 필요 X)

	// 표적지 이동 양 끝점 중 하나
	UPROPERTY(EditInstanceOnly)
	USceneComponent* Anchor0{};

	// 표적지 이동 양 끝점 중 하나
	UPROPERTY(EditInstanceOnly)
	USceneComponent* Anchor1{};

	// 표적지 이동 속도
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MoveSpeed = 1.f;

private:

	FVector LeftLocation{};
	FVector RightLocation{};

	float ElapsedTime{};
};
