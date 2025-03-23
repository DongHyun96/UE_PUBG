// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "C_TickRandomColorGenerator.generated.h"

UCLASS()
class UE_PUBG_API AC_TickRandomColorGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AC_TickRandomColorGenerator();

protected:
	
	virtual void BeginPlay() override;

public:	
	
	virtual void Tick(float DeltaTime) override;

	FColor GetTickRandomColor() const { return TickRandomColor; }

private:
	
	FColor TickRandomColor{};

};
