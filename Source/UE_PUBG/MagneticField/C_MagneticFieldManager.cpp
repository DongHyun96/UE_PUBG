// Fill out your copyright notice in the Description page of Project Settings.


#include "MagneticField/C_MagneticFieldManager.h"
#include "C_MagneticWall.h"
#include "C_WaterTileCheckerComponent.h"
#include "Character/C_Enemy.h"
#include "UObject/ConstructorHelpers.h"
#include "Utility/C_Util.h"

#include "Singleton/C_GameSceneManager.h"

#include "Character/C_Player.h"
#include "Character/Component/EnemyComponent/C_TargetLocationSettingHelper.h"
#include "HUD/C_HUDWidget.h"
#include "HUD/C_InstructionWidget.h"
#include "HUD/C_MagneticFieldIndicatorWidget.h"
#include "HUD/C_MapWidget.h"
#include "HUD/C_MainMapWidget.h"


AC_MagneticFieldManager::AC_MagneticFieldManager()
{
	PrimaryActorTick.bCanEverTick = true;

	WaterTileCheckerComponent = CreateDefaultSubobject<UC_WaterTileCheckerComponent>("WaterTileCheckerComponent");
	WaterTileCheckerComponent->SetMagneticFieldManager(this);
}

void AC_MagneticFieldManager::BeginPlay()
{
	Super::BeginPlay();
	/*FTimerHandle& TimerHandle = GAMESCENE_MANAGER->GetTimerHandle();
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AC_MagneticFieldManager::ActivateMagneticField, 5.f);*/
}

void AC_MagneticFieldManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Timer += DeltaTime;

	//UC_Util::Print(CurrentPhase);
	if (!bIsHandleUpdateStateStarted) return;
	
	HandleUpdateState(DeltaTime);
	HandleDamagingCharacters(DeltaTime);
}

void AC_MagneticFieldManager::ActivateMagneticField()
{
	bIsHandleUpdateStateStarted = true;

	Timer = 0.f;
	
	const int Minutes = PhaseInfos[CurrentPhase].ShrinkDelayTime / 60;
	const int Seconds = PhaseInfos[CurrentPhase].ShrinkDelayTime - 60 * Minutes;

	FString MagneticFieldInfoString = "PROCEED TO PLAY AREA MARKED ON THE MAP IN ";
		
	if (Minutes > 1)		MagneticFieldInfoString += FString::FromInt(Minutes) + " MINUTES ";
	else if (Minutes == 1)	MagneticFieldInfoString += FString::FromInt(Minutes) + " MINUTE ";
	if (Seconds != 0)		MagneticFieldInfoString += FString::FromInt(Seconds) + " SECONDS ";

	MagneticFieldInfoString += "!";

	UC_HUDWidget* HUDWidget = GAMESCENE_MANAGER->GetPlayer()->GetHUDWidget();
	
	HUDWidget->GetInstructionWidget()->ActivateMagneticFieldInstructionText(MagneticFieldInfoString);
	HUDWidget->GetMagneticFieldIndicatorWidget()->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	HUDWidget->GetMagneticFieldIndicatorWidget()->SetPhaseText("PHASE  1");

	InitDelayTimeLeftNotifiedChecker();
}

bool AC_MagneticFieldManager::IsInMainCircle(AC_BasicCharacter* Character) const
{
	FVector2D CharacterPos  = UC_Util::GetXY(Character->GetActorLocation());
	FVector2D MainCirclePos = UC_Util::GetXY(MainCircle.MidLocation);
	return FVector2D::Distance(CharacterPos, MainCirclePos) < MainCircle.Radius;
}

void AC_MagneticFieldManager::HandleUpdateState(const float& DeltaTime)
{
	switch (MagneticFieldState)
	{
	case EMagneticFieldState::IDLE:

		if (Timer < PhaseInfos[CurrentPhase].ShrinkDelayTime)
		{
			HandleDelayTimeHUDNotification();
			float Time = PhaseInfos[CurrentPhase].ShrinkDelayTime - Timer;
			GAMESCENE_MANAGER->GetPlayer()->GetHUDWidget()->GetMagneticFieldIndicatorWidget()->SetTimerText(Time);
			
			return; // 보류시간이 아직 남았음
		}

		/* 보류시간이 다 지났을 때 */
		Timer = 0.f;

		MagneticFieldState = EMagneticFieldState::SHRINK;

		// Enemy들 NextCircle random TargetLocation 잡아주기
		for (AC_Enemy* Enemy : GAMESCENE_MANAGER->GetEnemies())
		{
			if (!IsValid(Enemy)) continue;
			if (Enemy->GetMainState() == EMainState::DEAD) continue;
			
			Enemy->GetTargetLocationSettingHelper()->TrySetRandomInCircleTargetLocationAtMagneticCircle(NextCircle);
		}

		{
			UC_HUDWidget* HUDWidget = GAMESCENE_MANAGER->GetPlayer()->GetHUDWidget(); 
			HUDWidget->GetInstructionWidget()->ActivateMagneticFieldInstructionText("RESTRICTING PLAY AREA!");
			HUDWidget->GetMagneticFieldIndicatorWidget()->ToggleWarningTriangleVisibility(true);
		}
		
		return;
	case EMagneticFieldState::SHRINK:


		// 줄어드는 시간이 완전히 끝났을 때
		if (Timer > PhaseInfos[CurrentPhase].ShrinkTotalTime)
		{
			UpdateWalls(MainCircle.MidLocation, MainCircle.Radius);
			UpdateMainCircleInfoOnMapUI();
			Timer = 0.f;
			MagneticFieldState = EMagneticFieldState::SHRINK_COMPLETED;
			return;
		}

		// 일정한 속력 및 방향으로 MainCircle을 줄이고 중앙점도 일정한 속력과 방향으로 옮기기
		MainCircle.Radius -= PhaseInfos[CurrentPhase].RadiusShrinkSpeed * DeltaTime;

		MainCircle.MidLocation +=	PhaseInfos[CurrentPhase].MidPointMoveDirection * 
									PhaseInfos[CurrentPhase].MidPointMoveSpeed *
									DeltaTime;

		UpdateWalls(MainCircle.MidLocation, MainCircle.Radius);
		UpdateMainCircleInfoOnMapUI();

		// UI Update
		{
			UC_MagneticFieldIndicatorWidget* IndicatorWidget = GAMESCENE_MANAGER->GetPlayer()->GetHUDWidget()->GetMagneticFieldIndicatorWidget();
			IndicatorWidget->SetTimerText(PhaseInfos[CurrentPhase].ShrinkTotalTime - Timer);
			IndicatorWidget->SetProgressBarPercent(Timer / PhaseInfos[CurrentPhase].ShrinkTotalTime);
		}

		return;
	case EMagneticFieldState::SHRINK_COMPLETED:
	{
		// 값 정확히 맞춰주기
		MainCircle = NextCircle;

		// 마지막 phase의 Shrink completed
		if (CurrentPhase >= LAST_PHASE) return;

		CurrentPhase++;

		// 마지막 자기장의 줄어드는 곳은 정 가운데
		if (CurrentPhase == LAST_PHASE)
		{
			NextCircle.MidLocation = MainCircle.MidLocation;
			NextCircle.Radius = PhaseInfos[CurrentPhase + 1].PhaseRadius;

			// 속력 세팅
			float RadiusDifference = MainCircle.Radius - NextCircle.Radius;
			PhaseInfos[CurrentPhase].RadiusShrinkSpeed = RadiusDifference / PhaseInfos[CurrentPhase].ShrinkTotalTime;
			PhaseInfos[CurrentPhase].MidPointMoveSpeed = 0.f;
		}
		else SetRandomNextCircleAndSpeedDirection(); // Random한 Next Circle 뽑기
		
		UpdateNextCircleInfoOnMapUI();
		
		{
			const int Minutes = PhaseInfos[CurrentPhase].ShrinkDelayTime / 60;
			const int Seconds = PhaseInfos[CurrentPhase].ShrinkDelayTime - 60 * Minutes;
			
			FString MagneticFieldInfoString = "PROCEED TO PLAY AREA MARKED ON THE MAP IN ";
			
			if (Minutes > 1)		MagneticFieldInfoString += FString::FromInt(Minutes) + " MINUTES";
			else if (Minutes == 1)	MagneticFieldInfoString += FString::FromInt(Minutes) + " MINUTE";
			if (Seconds != 0)		MagneticFieldInfoString += FString::FromInt(Seconds) + " SECONDS";

			MagneticFieldInfoString += "!";

			UC_HUDWidget* HUDWidget = GAMESCENE_MANAGER->GetPlayer()->GetHUDWidget(); 
			HUDWidget->GetInstructionWidget()->ActivateMagneticFieldInstructionText(MagneticFieldInfoString);
			HUDWidget->GetMagneticFieldIndicatorWidget()->ToggleWarningTriangleVisibility(false);
			HUDWidget->GetMagneticFieldIndicatorWidget()->SetProgressBarPercent(0.f);
			HUDWidget->GetMagneticFieldIndicatorWidget()->SetPhaseText("PHASE  " + FString::FromInt(CurrentPhase));
		}
		
		MagneticFieldState = EMagneticFieldState::IDLE;

		Timer = 0.f;

		InitDelayTimeLeftNotifiedChecker();
	}
		return;
	default: return; 
	}
}

void AC_MagneticFieldManager::HandleDamagingCharacters(const float& DeltaTime)
{
	static float DamageTimer{};

	DamageTimer += DeltaTime;

	// 1초 단위로 Damage 적용
	if (DamageTimer > 1.f)
	{
		DamageTimer -= 1.f;

		for (AC_BasicCharacter* Character : GAMESCENE_MANAGER->GetAllCharacters())
		{
			if (!IsInMainCircle(Character))
			{
				float DamageAmount = PhaseInfos[CurrentPhase].DamagePerSecond;
				
				FKillFeedDescriptor KillFeedDescriptor =
				{
					EDamageType::BlueZone,
					nullptr,
					nullptr,
					nullptr,
					false,
					0
				};
				
				Character->GetStatComponent()->TakeDamage(DamageAmount, KillFeedDescriptor);
			}
		}
	}
}

void AC_MagneticFieldManager::InitManager()
{
	// 첫 Main 자기장은 아예 맵 밖에 존재 -> 1페이즈 줄어들 때의 NextCircle로 줄어들었을 때 비로소 맵에 잡힘

	// 멤버변수들 초기화
	CurrentPhase		= 1;
	MagneticFieldState	= EMagneticFieldState::IDLE;
	Timer				= 0.f;
	
	MainCircle.MidLocation	= { 0.f, 0.f, WALL_Z_LOCATION }; // 맵의 중점으로 setting
	MainCircle.Radius		= PhaseInfos[1].PhaseRadius;

	// Next Circle 설정해주기
	SetRandomNextCircleAndSpeedDirection();

	// TODO : 비행기 경로 도달 모두 끝낸 뒤 페이즈 시작

	UpdateWalls(MainCircle.MidLocation, MainCircle.Radius);

	
	//UpdateNextCircleInfoOnMapUI();
	//UpdateMainCircleInfoOnMapUI();
	FTimerHandle TimerHandle = GAMESCENE_MANAGER->GetTimerHandle();
	FTimerHandle TimerHandle2 = GAMESCENE_MANAGER->GetTimerHandle();
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AC_MagneticFieldManager::UpdateMainCircleInfoOnMapUI, 2.f, false);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle2, this, &AC_MagneticFieldManager::UpdateNextCircleInfoOnMapUI, 2.f, false);
}

void AC_MagneticFieldManager::UpdateWalls(const FVector& MidLocation, const float& Radius)
{
	FVector FirstPos = FVector(MidLocation.X + Radius, MidLocation.Y, MidLocation.Z);
	MagneticWalls[0]->SetActorLocation(FirstPos);

	float WallWidth = 2 * PI * Radius / float(SLICE_COUNT);
	WallWidth *= 0.01f; // 기본 단위가 1m로 맞춰져 있음

	// Scale X가 높이, Y가 Width로 처리됨, z는 평면의 두께로 처리되는데 어차피 PlaneMesh라 반영 x
	MagneticWalls[0]->SetActorScale3D(FVector(100.f, WallWidth, 1.f));
	MagneticWalls[0]->UpdateMaterialVTiling();

	for (int i = 1; i < SLICE_COUNT; i++)
	{
		MagneticWalls[i]->SetActorScale3D(FVector(100.f, WallWidth, 1.f));

		float DegTheta = 360.f * i / SLICE_COUNT;
		MagneticWalls[i]->SetActorRotation(FRotator(90.f, DegTheta, 0.f));

		float XPos = Radius * FMath::Cos(2 * PI * i / SLICE_COUNT);
		float YPos = Radius * FMath::Sin(2 * PI * i / SLICE_COUNT);

		FVector CurrentPos = FVector(MidLocation.X + XPos, MidLocation.Y + YPos, MidLocation.Z);

		MagneticWalls[i]->SetActorLocation(CurrentPos);
		MagneticWalls[i]->UpdateMaterialVTiling();
	}
}

void AC_MagneticFieldManager::SetRandomNextCircleAndSpeedDirection()
{
	// Next random circle setting

	// Set Radius
	NextCircle.Radius = PhaseInfos[CurrentPhase + 1].PhaseRadius;
	
	if (PhaseInfos[CurrentPhase + 1].bHasExactLocation)
		NextCircle.MidLocation = PhaseInfos[CurrentPhase + 1].ExactPhaseLocation;
	else
	{
		uint16 TryCount{};
		while (true)
		{
			float XDir = FMath::FRandRange(-1.f, 1.f);
			float YDir = FMath::FRandRange(-1.f, 1.f);

			FVector RandomDirection = FVector(XDir, YDir, 0.f);
			RandomDirection.Normalize();

			float RandomScalar = FMath::FRandRange(0.f, MainCircle.Radius - NextCircle.Radius);

			FVector RandomNextCirclePos = MainCircle.MidLocation + RandomDirection * RandomScalar;

			NextCircle.MidLocation	= RandomNextCirclePos; // Set NextCircle MidLocation

			if (!PhaseInfos[CurrentPhase + 1].bShouldTryToAvoidWater) break;
			
			// 만약 WaterTile 개수를 따져서 위치를 잡아야 한다면 제대로 된 위치가 잡힐 때 까지 반복
			uint8 WaterTileCount = WaterTileCheckerComponent->GetWaterTileCount(NextCircle);
			if (WaterTileCount <= PhaseInfos[CurrentPhase + 1].WaterTileCountLimit) break;

			// 500번 시도해서 WaterTile을 피해도 적절한 위치가 나오지 않았을 때에는 적절한 위치가 없다고 판단, 그냥 진행하기
			if (++TryCount > 500) break;
		}
		
		UC_Util::Print("Try Avoiding Water count : " + FString::FromInt(TryCount), FColor::Cyan, 10.f);
	}
	
	/* Speed and direction settings */
	
	// 속력 setting
	float RadiusDifference = MainCircle.Radius - NextCircle.Radius;
	PhaseInfos[CurrentPhase].RadiusShrinkSpeed = RadiusDifference / PhaseInfos[CurrentPhase].ShrinkTotalTime;

	float MidPointsDistance = FVector::Distance(MainCircle.MidLocation, NextCircle.MidLocation);
	PhaseInfos[CurrentPhase].MidPointMoveSpeed = MidPointsDistance / PhaseInfos[CurrentPhase].ShrinkTotalTime;

	// Direction setting
	PhaseInfos[CurrentPhase].MidPointMoveDirection = NextCircle.MidLocation - MainCircle.MidLocation;
	PhaseInfos[CurrentPhase].MidPointMoveDirection.Normalize();
}

void AC_MagneticFieldManager::UpdateMainCircleInfoOnMapUI()
{
	UC_MapWidget* MiniMapWidget = GAMESCENE_MANAGER->GetPlayer()->GetHUDWidget()->GetMiniMapWidget();
	UC_MapWidget* MainMapWidget = GAMESCENE_MANAGER->GetPlayer()->GetMainMapWidget();

	// 중점과 반지름 UV 좌표계로 변환
	float U		=  MainCircle.MidLocation.Y * MAP_LENGTH_TO_UV_FACTOR + 0.5f;
	float V		= -MainCircle.MidLocation.X * MAP_LENGTH_TO_UV_FACTOR + 0.5f;
	float Rad	=  MainCircle.Radius * MAP_LENGTH_TO_UV_FACTOR;

	MiniMapWidget->SetMainCircleInfo(U, V, Rad);
	MainMapWidget->SetMainCircleInfo(U, V, Rad);
}

void AC_MagneticFieldManager::UpdateNextCircleInfoOnMapUI()
{
	UC_MapWidget* MiniMapWidget = GAMESCENE_MANAGER->GetPlayer()->GetHUDWidget()->GetMiniMapWidget();
	UC_MapWidget* MainMapWidget = GAMESCENE_MANAGER->GetPlayer()->GetMainMapWidget();

	// 중점과 반지름 UV 좌표계로 변환
	float U		=  NextCircle.MidLocation.Y * MAP_LENGTH_TO_UV_FACTOR + 0.5f;
	float V		= -NextCircle.MidLocation.X * MAP_LENGTH_TO_UV_FACTOR + 0.5f;
	float Rad	=  NextCircle.Radius * MAP_LENGTH_TO_UV_FACTOR;

	MiniMapWidget->SetNextCircleInfo(U, V, Rad);
	MainMapWidget->SetNextCircleInfo(U, V, Rad);
}

void AC_MagneticFieldManager::HandleDelayTimeHUDNotification()
{
	for (TPair<float, bool>& Checker : DelayTimeLeftNotifiedChecker)
	{
		if (Checker.Value) continue;

		// float LeftTime = PhaseInfos[CurrentPhase + 1].ShrinkTotalTime;
		float Difference = FMath::Abs(Checker.Key - (PhaseInfos[CurrentPhase].ShrinkDelayTime - Timer));

		if (Difference < 0.1f)
		{
			FString NotifyLog = "RESTRICTING THE PLAY AREA IN ";
			NotifyLog += (Checker.Key == 60.f) ? "1 MINUTE" : FString::FromInt(static_cast<int>(Checker.Key)) + " SECONDS"; 
			
			GAMESCENE_MANAGER->GetPlayer()->GetHUDWidget()->GetInstructionWidget()->ActivateMagneticFieldInstructionText(NotifyLog);
			Checker.Value = true;
		}
	}
}

void AC_MagneticFieldManager::InitDelayTimeLeftNotifiedChecker()
{
	for (TPair<float, bool>& NotifiedChecker : DelayTimeLeftNotifiedChecker)
		NotifiedChecker.Value = (PhaseInfos[CurrentPhase].ShrinkDelayTime < NotifiedChecker.Key); // 총 지연시간보다 넘어가는 Notify 지점은 굳이 체크하지 않도록 설정
}

