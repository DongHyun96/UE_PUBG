// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item/C_Item.h"
#include "AttachmentActor.generated.h"

UCLASS(abstract)
class UE_PUBG_API AAttachmentActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAttachmentActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
protected:
	class AC_BasicCharacter* OwnerCharacter{};
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UMeshComponent* AttachmentMesh{};
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	EPartsName PartName{};
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	EAttachmentNames AttachmentName{};
public: 
	void SetOwnerCharacter(class AC_BasicCharacter* InOwnerCharacter) { OwnerCharacter = InOwnerCharacter; }
	EPartsName GetPartName() { return PartName; }
	EAttachmentNames GetAttachmentName() { return AttachmentName; }
	UMeshComponent* GetAttachmentMesh() { return AttachmentMesh; }

	virtual bool UseAttachStrategy() PURE_VIRTUAL(AAttachmentActor::UseAttachStrategy, return false;);
	virtual bool UseDetachStrategy() PURE_VIRTUAL(AAttachmentActor::UseDetachStrategy, return false;);
	virtual bool UseMrbStrategy() { return false; };
};


