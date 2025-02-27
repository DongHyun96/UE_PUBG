// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "C_DoorNavLinkProxyManager.generated.h"

UCLASS()
class UE_PUBG_API AC_DoorNavLinkProxyManager : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AC_DoorNavLinkProxyManager();

protected:
	
	virtual void BeginPlay() override;

public:	
	
	virtual void Tick(float DeltaTime) override;

private:

	// TArray<class >

};
