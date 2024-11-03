// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item/C_Item.h"
#include "InvenUserInterface/C_InvenUiWidget.h"
#include "C_InvenSystem.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE_PUBG_API UC_InvenSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UC_InvenSystem();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetOwnerCharacter(class AC_BasicCharacter* InOwnerCharacter) { OwnerCharacter = InOwnerCharacter; }

	void InitializeList();

	void OpenInvenUI();
protected:
	class UC_InvenUiWidget* InvenUI;

	APlayerController* PlayerController;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> InvenUiClass;

	bool isPanelOpened = false;

	AC_BasicCharacter* OwnerCharacter = nullptr;
		
};
