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

	UFUNCTION(BlueprintCallable)
	void SetOwnerCharacter(class AC_BasicCharacter* InOwnerCharacter) { OwnerCharacter = InOwnerCharacter; }

	/// <summary>
	/// InvenUI->Init�Լ� ȣ��, InvenUI�� �ʱ�ȭ
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void InitializeList();

	/// <summary>
	/// InvenUI�� ���ְ� ���ִ� �Լ�.
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void OpenInvenUI();

	/// <summary>
	/// InvenUI�� ���ִ� �Լ�.
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void ShowInvenUI();

	/// <summary>
	/// InvenUi�� ���ִ� �Լ�.
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void CloseInvenUI();

public:
	UFUNCTION(BlueprintCallable)
	UC_InvenUiWidget* GetInvenUI() { return InvenUI; }
protected:
	UPROPERTY()
	class UC_InvenUiWidget* InvenUI = nullptr;

	UPROPERTY()
	APlayerController* PlayerController = nullptr;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> InvenUiClass;

	bool isPanelOpened = false;

	//���⿡ UPROERTY()�� �ɸ� �κ��� �Ⱥ���. InvenUIWidget�ȿ��� OwnerCharacter->GetInvenComponent()->GetNearItems() -> Empty�� ���°� �Ǿ� ����.
	//UPROPERTY(BlueprintReadWrite, EditAnywhere)
	AC_BasicCharacter* OwnerCharacter = nullptr;
		
};
