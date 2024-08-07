// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "GameFramework/Character.h"
#include "Character/C_BasicCharacter.h"
#include "C_Player.generated.h"


USTRUCT(BlueprintType)
struct FPoseAnimMontage
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TMap<EPoseState, FPriorityAnimMontage> LeftMontages{};
		
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TMap<EPoseState, FPriorityAnimMontage> RightMontages{};
};

/**
 * 
 */
UCLASS()
class UE_PUBG_API AC_Player : public AC_BasicCharacter
{
	GENERATED_BODY()

public:
	AC_Player();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public: // Input mapped actions

	void Move(const struct FInputActionValue& Value);
	
	/// <summary>
	/// Move가 끝날 때 호출됨, AnimCharacter에서 Speed Lerp할 값 setting 및 Turn in place 동작 초기 setting 
	/// </summary>
	void MoveEnd(const struct FInputActionValue& Value);

	void Look(const struct FInputActionValue& Value);
	void Walk(const struct FInputActionValue& Value);
	void Sprint();


	void Crouch();
	void Crawl();

	void OnJump();
	void CancelTurnInPlaceMotion();
	//Alt 키 누를때 이름 추천부탁
	void HoldDirection();
	void ReleaseDirection();
	void HandleControllerRotation(float DeltaTime);

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

	//상호작용(F)
	//오브젝트의 묶음별로 만들어야 할 수도?
	//아니면 그냥 UObject로 만들기
	void Interaction();

/// <summary>
/// 아이템 상호작용을 위한 변수와 함수.
/// 구를 통해 아이템과의 충동을 감지하는 함수.
/// </summary>
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* DetectionSphere;

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	class UC_InputComponent* GetInvenComponent() { return MyInputComponent; }//부모 클래스에 존재
public:
	//Aim Press Camera
	UPROPERTY(EditDefaultsOnly)
	class UCameraComponent* AimCamera;
	UPROPERTY(EditDefaultsOnly)
	class USpringArmComponent* AimSpringArmTemp;
protected:
	//Aim Press Camera 위치 조정 함수
	void HandleAimPressCameraLocation();
protected:

	/*
	카메라가 바라보는 방향으로 몸체가 돌아가는 기본 StrafeOn 설정 :
	BPC_Player(self) : Use Control Rotation Yaw			= true
	Spring Arm : Use Pawn Control Rotation				= true
	Character Movement : Orient Rotation to Movement	= false;

	가만히 있을 시 Use Control Rotation Yaw : false

	*/

	/// <summary>
	/// <para> 캐릭터가 멈춰있을 때, 캐릭터가 오브젝트가 바라보는 방향과 컨트롤러(카메라)가 </para>
	/// <para> 바라보는 방향의 각이 90도 이상이면, Turn In place로 캐릭터 조정 </para>
	/// </summary>
	void HandleTurnInPlace();
	
	/// <summary>
	/// 멈춰 있을 때의 Rotation 세팅 값들로 돌아가기
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void SetStrafeRotationToIdleStop();

private:

	/// <summary>
	/// Turn Anim Montage 초기화
	/// </summary>
	void InitTurnAnimMontageMap();
public:
	/// <summary>
	/// 조준에 따른 카메라 변경
	/// </summary>
	void SetToAimKeyPress();
	void SetToAimDownSight();
	void BackToMainCamera();
protected:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly) 
	class UC_InputComponent* MyInputComponent{};   

protected: // Turn in place 애님 몽타주 관련

	/// <summary>
	/// 각 HandState와 PoseState에 따른 TurnAnimMontage 맵
	/// </summary>
	UPROPERTY(BluePrintReadWrite, EditAnywhere)
	TMap<EHandState, FPoseAnimMontage> TurnAnimMontageMap{};

	TArray<class AC_Item*> NearInventory;

	/*
	UENUM(BlueprintType)
	enum class EHandState : uint8
	{
		UNARMED,
		WEAWPON_GUN,
		WEAPON_MELEE,
		WEAPON_THROWABLE
	};
	*/
	
	/*
	enum class EPoseState : uint8
	{
		STAND,
		CROUCH,
		CRAWL
	};
	*/
};
