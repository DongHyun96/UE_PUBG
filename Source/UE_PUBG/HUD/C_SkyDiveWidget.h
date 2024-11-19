// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "C_SkyDiveWidget.generated.h"

enum class ESkyDivingState : uint8;

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_SkyDiveWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void NativeConstruct() override;

protected:

	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:

	void SetJumpedAltitude(float Altitude) { JumpedAltitude = Altitude; }
	void SetParachuteLimitAltitude(float Altitude) { ParachuteLimitAltitude = Altitude; }
	
	void SetOwnerPlayer(class AC_Player* InOwnerPlayer) { OwnerPlayer = InOwnerPlayer; }
	
	/// <summary>
	/// Altitude Text setting & Altitude Box position setting
	/// </summary>
	/// <param name="Altitude"> : ��(cm) </param>
	void SetAltitude(const float& Altitude);

private:

	/// <summary>
	/// Speed HUD ������Ʈ
	/// </summary>
	void UpdateSpeed();

private:

	class AC_Player* OwnerPlayer{};

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UTextBlock* AltitudeTextBlock{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UCanvasPanel* AltitudeBoxPanel{};

private:
	// AltitudeBox �������� Slot obj
	class UCanvasPanelSlot* AltitudeBoxPanelSlot{};

private: // ���� �� ���� ����  
	
	float JumpedAltitude{}; // �ִ� ����
	float ParachuteLimitAltitude{}; // Parachute Limit Altitude

private: // ���� ���� Box Widget ��ġ ���� ���
	
	const FVector2D ALTITUDE_BOX_MAX_TO_PARACHUTE = { -252.f, 99.f };
	const FVector2D ALTITUDE_BOX_PARACHUTE_TO_MIN = { 99.f, 190.f };

protected: // Speed ���� fields

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UMaterialInstanceDynamic* SpeedScaleBarMatInstDynamic{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UTextBlock* CurrentSpeedTextBlock{};

private:

	TArray<class UTextBlock*> SpeedTextBlocks{};
	TArray<class UCanvasPanelSlot*> SpeedTextBlockSlots{};

	const FVector2D SPEEDTEXTBLOCKS_Y_RANGE = FVector2D(278, -268);
	const float SPEEDTEXT_MID_Y_POS = 5.f;
	const float SPEEDTEXTBLOCKS_STEP = -91.f;
	const float TEXTBLOCKSCALING_FACTOR = 1.82f;
	
};
