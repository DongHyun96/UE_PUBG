// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "C_InstructionWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_InstructionWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:

	void HandleConsumableInstructionFlicker(const float& DeltaTime);

public:

	UFUNCTION(BlueprintImplementableEvent)
	void ToggleEjectInstructionVisibility(bool Visible);

	UFUNCTION(BlueprintImplementableEvent)
	void ToggleDeployParachuteInstructionVisibility(bool Visible);

	bool ActivateConsumableInstruction(FString UsingTextContent);

	void DeActivateConsumableInstruction() { ConsumableInstruction->SetVisibility(ESlateVisibility::Hidden); }

protected: // Consumable Item Activating cancel instruction ฐüทร

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UCanvasPanel* ConsumableInstruction{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UTextBlock* ConsumableCurrentUsingTextBlock{};
};
