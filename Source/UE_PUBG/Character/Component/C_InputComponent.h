// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_InputComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
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

	void BindAction(UInputComponent* PlayerInputComponent, class AC_Player* Player);

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

};
