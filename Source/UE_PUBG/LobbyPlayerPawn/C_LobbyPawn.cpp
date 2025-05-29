// Fill out your copyright notice in the Description page of Project Settings.


#include "C_LobbyPawn.h"

#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Singleton/C_GameInstance.h"
#include "Utility/C_Util.h"


AC_LobbyPawn::AC_LobbyPawn()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AC_LobbyPawn::BeginPlay()
{
	Super::BeginPlay();

	CameraComponent = FindComponentByClass<UCameraComponent>();
	if (!CameraComponent) UC_Util::Print("From AC_LobbyPawn::BeginPlay : Camera not found", FColor::Red, 20.f);

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC && PC->PlayerCameraManager) PC->PlayerCameraManager->StartCameraShake(CameraShakeClass);
	
	// Log-in 여부에 따른 초기 위치 조정
	UC_GameInstance* GameInstance = Cast<UC_GameInstance>(GetGameInstance());
	if (!GameInstance->GetPlayerNickNameSet())
	{
		FVector Location = GetActorLocation();
		Location.X				 = LogInXLocation;
		CurrentXLocationLerpDest = LogInXLocation;
		SetActorLocation(Location);
	}
	else InitCharacterNameTag();
}

void AC_LobbyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	HandleLerpLocation(DeltaTime);
}

void AC_LobbyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AC_LobbyPawn::HandleLerpLocation(float DeltaTime)
{
	const float NewX = FMath::Lerp(GetActorLocation().X, CurrentXLocationLerpDest, DeltaTime * 2.f);
	FVector Location = GetActorLocation();
	Location.X = NewX;
	SetActorLocation(Location);
}

