// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_InputComponent.generated.h"


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
private:

	class AC_Player* Player{};
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
	class UInputAction* Num1Action{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UInputAction* Num2Action{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UInputAction* Num4Action{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UInputAction* Num5Action{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UInputAction* XAction{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UInputAction* BAction{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UInputAction* RAction{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UInputAction* MLBAction{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UInputAction* MRBAction{};

	// 가운데 마우스 버튼 처리
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UInputAction* MMBAction{}; 
	
	//상호작용키(F)
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UInputAction* Interaction{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UInputAction* NKeyAction{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UInputAction* MKeyAction{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UInputAction* IKeyAction{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UInputAction* TabKeyAction{};
private:
	class AC_Weapon* ChangeTestWeapon{};
};
