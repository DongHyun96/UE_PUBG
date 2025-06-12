// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "C_ANCrawlSound.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_ANCrawlSound : public UAnimNotify
{
	GENERATED_BODY()

public:
	
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	
protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	USoundBase* CrawlSound{};
	
};
