// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item/C_Item.h"
//#include "InvenUserInterface/C_InvenUiWidget.h"
#include "InvenUI/C_InventoryUIWidget.h"
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

	UFUNCTION(BlueprintCallable)
	void SetOwnerCharacter(class AC_BasicCharacter* InOwnerCharacter) { OwnerCharacter = InOwnerCharacter; }

	/// <summary>
	/// InvenUI->Init함수 호출, InvenUI를 초기화
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void InitializeList();

	/// <summary>
	/// InvenUI를 켜주고 꺼주는 함수.
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void OpenInvenUI();

	/// <summary>
	/// InvenUI를 켜주는 함수.
	/// </summary>
	//UFUNCTION(BlueprintCallable)
	void ShowInvenUI();

	/// <summary>
	/// InvenUi를 꺼주는 함수.
	/// </summary>
	//UFUNCTION(BlueprintCallable)
	void CloseInvenUI();

public:
	UFUNCTION(BlueprintCallable)
	UC_InventoryUIWidget* GetInvenUI() { return InvenUI; }

	class AC_PlayerController* GetPlayerController() { return PlayerController; }
protected:

	UPROPERTY()
	UC_InventoryUIWidget* InvenUI = nullptr;
	UPROPERTY()
	AC_PlayerController* PlayerController = nullptr;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> InvenUiClass;

	//여기에 UPROERTY()를 걸면 인벤이 안보임. InvenUIWidget안에서 OwnerCharacter->GetInvenComponent()->GetNearItems() -> Empty인 상태가 되어 버림.
	//UPROPERTY(BlueprintReadWrite, EditAnywhere)
	AC_BasicCharacter* OwnerCharacter = nullptr;
		

};


