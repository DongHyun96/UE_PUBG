// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "C_CrosshairWidgetComponent.generated.h"
UENUM(BlueprintType)
enum class ECrosshairState : uint8
{
	NORIFLE,
	RIFLE,
	RIFLEAIMDOWNSIGHT,
	RIFLECRAWLMOVING
};
UENUM(BlueprintType)
enum class EWeaponSightState : uint8
{
	NOSIGHT,
	REDDOT
};
USTRUCT(BlueprintType)
struct FAimDownSightImages
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EWeaponSightState, class UImage*> Images{};
};
UCLASS()
class UE_PUBG_API UC_CrosshairWidgetComponent : public UUserWidget
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties

protected:
	// Called when the game starts
	//virtual void BeginPlay() override;
public:	
	virtual void NativeConstruct() override;
	// Called every frame
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	void SetOwnerCharacter(class AC_BasicCharacter* InOwnerCharacter) { OwnerCharacter = InOwnerCharacter; }

protected:
	//크로스헤어 위젯
	AC_BasicCharacter* OwnerCharacter;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UUserWidget* AimSightWidget{};
	void InitializeAimSightWidget();

	//위젯에 띄울 이미지 변수
	FAimDownSightImages AimDownSightImages{};

	class UImage* BaseCrosshairImage{};
	UImage* RedDotImage{};
	UImage* GrayDotImage{};
	UImage* UnarmedRifleDotImage{};
	FVector2D InitialBaseCrosshairImageSlotSize{};
	ECrosshairState CurState{};

	class UBorder* CrosshairTop{};
	UBorder* CrosshairBottom{};
	UBorder* CrosshairLeft{};
	UBorder* CrosshairRight{};

	void SetVisibileOfCrosshairBorder(bool InIsVisible);
	float FullAutoFiringTime = 0;
	float FullAutoSpreadRate = 0;
	void CountFullAutoFiringTime(float InDeltaTime);
public:
	void SetCrosshairState(ECrosshairState InState);
	UUserWidget* GetAimWidget() { return AimSightWidget; }
	void ToggleWidgetVisibility(bool InOnOff);
	void ManageAimWidgetImages();
	void UpdateImageSize();
	void UpdateCrosshairBorderLocation(class UCanvasPanelSlot* InImageSlot);
};
