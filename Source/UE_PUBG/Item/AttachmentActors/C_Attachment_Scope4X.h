// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/AttachmentActors/AttachmentActor.h"
#include "C_Attachment_Scope4X.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API AC_Attachment_Scope4X : public AAttachmentActor
{
	GENERATED_BODY()
public:
	// Sets default values for this actor's properties
	AC_Attachment_Scope4X();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual bool UseStrategy() override;
};
