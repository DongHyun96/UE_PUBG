// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "C_PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API AC_PlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AC_PlayerController();

	void BeginPlay() override;

	virtual void SetupInputComponent() override; // 입력 처리 함수

	void ToggleMainMenu();  // 메뉴 토글 함수

	//UUserWidget* GetMainMenuWidget() const { return MainMenuWidget; }  // 메뉴 위젯 반환 함수
protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> MainMenuClass;  // WBP_MainMenu 클래스 연결

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* OpenMenuAction{};
private:
	class UUserWidget* MainMenuWidget = nullptr;  // 실제 위젯 객체

	class UInputMappingContext* InputMappingContext = nullptr;
};


