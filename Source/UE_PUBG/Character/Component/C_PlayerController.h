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

	// 입력 처리 함수
	virtual void SetupInputComponent() override; 

	// 메뉴 토글 함수
	UFUNCTION(BlueprintCallable)
	void ToggleMainMenu();  

	//UUserWidget* GetMainMenuWidget() const { return MainMenuWidget; }  // 메뉴 위젯 반환 함수

public:

	void SetLobbyWidget(class UC_LobbyWidget* InLobbyWidget) { LobbyWidget = InLobbyWidget; }
	
private:
	
	/// <summary>
	/// IMC를 Subsystem에 추가 시도 
	/// </summary>
	/// <param name="IMC"> : 추가하려는 IMC </param>
	/// <param name="Priority"> : IMC의 Priority </param>
	/// <returns> : 실패했다면 return false </returns>
	bool AddIMCToSubsystem(class UInputMappingContext* IMC, int Priority);

private: // LobbyMap 관련 Input 처리

	void OnLobbySpaceBarDown();
	void OnLobbyFKeyReleased();
	
protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> MainMenuClass;  // WBP_MainMenu 클래스 연결

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* OpenMenuAction{};
	
private:
	
	class UUserWidget* MainMenuWidget = nullptr;  // 실제 위젯 객체

	class UInputMappingContext* InputMappingContext = nullptr;

private: // LobbyMap에서만 쓰일 Input Action들 
	
	UInputAction* IALobbyFKeyAction{};
	UInputAction* IALobbySpaceBarAction{};

private:

	UC_LobbyWidget* LobbyWidget{};
	
};


