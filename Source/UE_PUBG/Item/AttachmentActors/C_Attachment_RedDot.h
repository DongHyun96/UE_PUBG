// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/AttachmentActors/AttachmentActor.h"
#include "C_Attachment_RedDot.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API AC_Attachment_RedDot : public AAttachmentActor
{
	GENERATED_BODY()
public:
	// Sets default values for this actor's properties
	AC_Attachment_RedDot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual bool UseAttachStrategy() override;
	virtual bool UseDetachStrategy() override;
};
