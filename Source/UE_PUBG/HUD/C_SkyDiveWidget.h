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
	virtual void NativeConstruct() override;

protected:

	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:

	void SetJumpedAltitude(float Altitude) { JumpedAltitude = Altitude; }
	void SetParachuteLimitAltitude(float Altitude) { ParachuteLimitAltitude = Altitude; }
	
	void SetOwnerPlayer(class AC_Player* InOwnerPlayer) { OwnerPlayer = InOwnerPlayer; }
	
	/// <summary>
	/// Altitude Text setting & Altitude Box position setting
	/// </summary>
	/// <param name="Altitude"> : 고도(cm) </param>
	void SetAltitude(const float& Altitude);

private:

	/// <summary>
	/// Speed HUD 업데이트
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
	// AltitudeBox 실질적인 Slot obj
	class UCanvasPanelSlot* AltitudeBoxPanelSlot{};

private: // 낙하 고도 관련 값들  
	
	float JumpedAltitude{}; // 최대 높이
	float ParachuteLimitAltitude{}; // Parachute Limit Altitude

private: // 낙하 높이 Box Widget 위치 관련 상수
	
	static const FVector2D ALTITUDE_BOX_MAX_TO_PARACHUTE;
	static const FVector2D ALTITUDE_BOX_PARACHUTE_TO_MIN;

protected: // Speed 관련 fields

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UMaterialInstanceDynamic* SpeedScaleBarMatInstDynamic{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UTextBlock* CurrentSpeedTextBlock{};

private:

	TArray<class UTextBlock*> SpeedTextBlocks{};
	TArray<class UCanvasPanelSlot*> SpeedTextBlockSlots{};

	static const FVector2D SPEEDTEXTBLOCKS_Y_RANGE;
	static const float SPEEDTEXT_MID_Y_POS;
	static const float SPEEDTEXTBLOCKS_STEP;
	static const float TEXTBLOCKSCALING_FACTOR;
	
};


