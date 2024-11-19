// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item/C_Item.h"
#include "C_AttachableItemMeshComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE_PUBG_API UC_AttachableItemMeshComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UC_AttachableItemMeshComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	TMap<EPartsName, TMap<EAttachmentNames, TArray<class AAttachmentActor*>>> GetAttachableItemMeshs() { return AttachableItemsMesh; }
	AAttachmentActor* GetAttachablePartMesh(EPartsName InPartsName, EAttachmentNames InAttachmentName);
	AAttachmentActor* GetCurrentAttachment(class USceneComponent* InParent, EPartsName InPartsName, EAttachmentNames InAttachmentName);
	void SetOwnerCharacter(class AC_BasicCharacter* InOwnerCharacter) { OwnerCharacter = InOwnerCharacter; }
	bool AttachToGun(class USceneComponent* InParent, EPartsName InPartsName, EAttachmentNames InAttachmentName);
	void DetachFromGun(class USceneComponent* InParent, EPartsName InPartsName, EAttachmentNames InAttachmentName);

	void UseAttachmentStrategy(USceneComponent* InParent);
protected:
	class AC_BasicCharacter* OwnerCharacter;
	TMap<EPartsName, TMap<EAttachmentNames,TArray<class AAttachmentActor*>>> AttachableItemsMesh;
		
};
