// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TrainingLevel/Tutorial/TutorialStageChecker/C_TutorialStageChecker.h"
#include "C_InputComponent.generated.h"

// TrainingGround Interactable Table 관련 Delegate 선언
// return 값은 Interaction 처리가 제대로 이루어졌다면 return true
DECLARE_DELEGATE_RetVal(bool, FInteractableTable_Delegate);

enum class EWeaponSlot : uint8;

UENUM(BlueprintType)
enum class EInputAction : uint8
{
	MOVE,
	JUMP,
	LOOK,
	WALK,
	SPRINT,
	CRAWL,
	CROUCH,
	HOLD_DIRECTION,
	DRAW_MAIN_WEAPON,
	DRAW_SUB_WEAPON,
	DRAW_MELEE_WEAPON,
	DRAW_THROWABLE_WEAPON,
	TOGGLE_ARMED,
	CHANGE_SHOOTING_MODE,
	RELOAD,
	SHOT,
	TOGGLE_AIMING,
	PING,
	INTERACTION,
	MINI_MAP,
	INVEN_UI,
	MAIN_MENU,
	THROWABLE_WHEEL,
	CONSUMABLE_WHEEL
};

//UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UE_PUBG_API UC_InputComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UC_InputComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void BindAction(UInputComponent* PlayerInputComponent, class AC_Player* InPlayer);

	TMap<EInputAction, class UInputAction*> GetInputActionMap() const { return InputActionMap; }

private:


	void Move(const struct FInputActionValue& Value);

	/// <summary>
	/// Move가 끝날 때 호출됨, AnimCharacter에서 Speed Lerp할 값 setting 및 Turn in place 동작 초기 setting 
	/// </summary>
	void MoveEnd(const struct FInputActionValue& Value);

	void Look(const struct FInputActionValue& Value);

	void Crouch();
	void OnSwimmingCrouch();

	void Crawl();
	void OnJump();
	void OnSwimmingJump();

	void OnSwimmingJumpCrouchEnd();

	void CancelTurnInPlaceMotion();
	//Alt 키 누를때 이름 추천부탁
	void HoldDirection();
	void ReleaseDirection();
	void SetToNonAimCamera();

	// Number input mappings
	void OnNum1();
	void OnNum2();
	void OnNum4();
	void OnNum5();
	
	/// <summary>
	/// OnNumKey 종합 Boiler plate 코드 정리 
	/// </summary>
	/// <returns> : 무기를 바꾸는 처리가 제대로 시작되었지 않았거나 바꿀 수 없는 상황이라면 return false </returns>
	bool OnNumKey(EWeaponSlot ChangeTo);

	void OnXKey();
	void OnBKey();
	void OnRKey();

	// TODO : 무기에 대한 마우스 버튼 클릭 처리만 해둠, 다른 처리도 필요할 예정
	void OnMLBStarted();
	void OnMLBOnGoing();
	void OnMLBCompleted();

	void OnMRBStarted();
	void OnMRBOnGoing();
	void OnMRBCompleted();

	void OnMMBStarted();

	void OnSprintStarted();
	void OnSprintReleased();

	void OnWalkStarted();
	void OnWalkReleased();

	//상호작용(F)
	//오브젝트의 묶음별로 만들어야 할 수도?
	//아니면 그냥 UObject로 만들기
	void OnFKey();

	void OnNKey();
	void OnMKey();

	void OnIKey();

	void OnTabKey();

	void OnGKey();

	void OnTKey();


private:

	AC_Player* Player{};

	class UCharacterMovementComponent* PlayerMovement{};

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UInputMappingContext* MappingContext{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UInputAction* MoveAction{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UInputAction* JumpAction{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UInputAction* LookAction{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UInputAction* WalkAction{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UInputAction* SprintAction{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UInputAction* CrawlAction{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UInputAction* CrouchAction{};

	/// <summary>
	/// 움직일 때 알트키 누르는 액션 -현호
	/// </summary>
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UInputAction* HoldDirectionAction{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UInputAction* DrawMainWeapon{};           // Draw Main Weapon Num1Action

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UInputAction* DrawSubWeapon{};           // Draw Sub Weapon Num2Action

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UInputAction* DrawMeleeWeapon{};           // Draw Melee Weapon Num4Action

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UInputAction* DrawThrowableWeapon{};           // Draw Throwable Weapon Num5Action

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UInputAction* ToggleArmed{};              // Toggle Armed XAction

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UInputAction* ChangeShootingMode{};              // Change Shooting Mode BAction

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UInputAction* Reload{};              // Reload RAction

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UInputAction* MLBAction{};            // Shot MLBAction

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UInputAction* MRBAction{};            // Toggle Aiming? MRBAction

	// 가운데 마우스 버튼 처리
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UInputAction* MMBAction{};            // Ping System MMBAction
	
	//상호작용키(F)
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UInputAction* Interaction{};          //

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UInputAction* MiniMap{};           // Toggle Big Map NKeyAction

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UInputAction* WorldMap{};           // Toggle Main Map MKeyAction

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UInputAction* IKeyAction{};           // Toggle Inventory  IKeyAction

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UInputAction* TabKeyAction{};         // Toggle Inventory

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UInputAction* MainMenuAction{};             // Toggle Main Menu MainMenuAction

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UInputAction* ToggleThrowableWheelAction{}; // Toggle Throwable Wheel

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UInputAction* ToggleConsumableAction{};		// Toggle Consumable Item

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<EInputAction, UInputAction*> InputActionMap{};

public:

	// Movement Tutorial 진행용 Delegate
	 FTutorialStageGoalCheckerDelegate MovementTutorialDelegate{};


	// Training ground JumpTrainingTable Interaction Delegate
	FInteractableTable_Delegate JumpTrainingTableNum1KeyInteractionDelegate{};
	FInteractableTable_Delegate JumpTrainingTableNum2KeyInteractionDelegate{};

	// Training ground CombatControlTable Interaction Delegate
	FInteractableTable_Delegate CombatControlFKeyInteractionDelegate{};
	
};


