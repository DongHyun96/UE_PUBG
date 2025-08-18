// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "C_InteractableTable.generated.h"

UCLASS()
class UE_PUBG_API AC_InteractableTable : public AActor
{
	GENERATED_BODY()

public:
	
	AC_InteractableTable();

protected:
	
	virtual void BeginPlay() override;

public:
	
	virtual void Tick(float DeltaTime) override;

protected:

	UFUNCTION()
	virtual void OnBoxColliderBeginOverlap
	(
		UPrimitiveComponent*	OverlappedComp,
		AActor*					OtherActor,
		UPrimitiveComponent*	OtherComp,
		int32					OtherBodyIndex,
		bool					bFromSweep,
		const FHitResult&		SweepResult
	);

	UFUNCTION()
	virtual void OnBoxColliderEndOverlap
	(
		UPrimitiveComponent*	OverlappedComp,
		AActor*					OtherActor,
		UPrimitiveComponent*	OtherComp,
		int32					OtherBodyIndex
	);


private:

	UShapeComponent* BoxColliderComponent{};
	UStaticMeshComponent* StaticMeshComponent{};

	TArray<UPrimitiveComponent*> PrimitiveComponents{};

protected:

	// 현재 이 테이블이 focused되어있는지
	bool bIsFocused{};

private:
	
	class UWidgetComponent* TriangleWidgetComponent{};
	float TriangleWidgetOpacityDestination = 1.f;

protected:
	
	UUserWidget* TriangleWidget{};
};
