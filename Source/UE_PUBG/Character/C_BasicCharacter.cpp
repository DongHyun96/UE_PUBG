// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/C_BasicCharacter.h"

#include "C_Player.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PhysicsVolume.h"

#include "Character/Component/C_EquippedComponent.h"
#include "Character/Component/C_InvenComponent.h"

#include "Component/C_StatComponent.h"
#include "Component/C_ConsumableUsageMeshComponent.h"
#include "Component/C_PoseColliderHandlerComponent.h"
#include "Component/C_SwimmingComponent.h"
#include "Character/Component/C_AttachableItemMeshComponent.h"
#include "Character/Component/C_FeetComponent.h"

#include "Component/C_ParkourComponent.h"
#include "HUD/C_HUDWidget.h"

#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"

#include "Item/Weapon/C_Weapon.h"
#include "Item/Weapon/Gun/C_Bullet.h"

#include "MotionWarpingComponent.h"
#include "Component/C_SmokeEnteredChecker.h"
#include "HUD/C_InformWidget.h"
#include "HUD/C_TimeBoxWidget.h"
#include "Item/Weapon/Gun/C_Gun.h"
#include "Item/Weapon/Gun/C_SR.h"
#include "Item/Weapon/ThrowingWeapon/C_ThrowingWeapon.h"
#include "Particles/ParticleSystemComponent.h"

#include "Loot/C_LootCrate.h"

#include "Singleton/C_GameSceneManager.h"

#include "Utility/C_Util.h"


// Sets default values
AC_BasicCharacter::AC_BasicCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//PrimaryActorTick.bCanEverTick = false;

	EquippedComponent = CreateDefaultSubobject<UC_EquippedComponent>("EquippedComponent");
	EquippedComponent->SetOwnerCharacter(this);

	InvenComponent = CreateDefaultSubobject<UC_InvenComponent>("C_Inventory");
	InvenComponent->SetOwnerCharacter(this);

	//InvenSystem = CreateDefaultSubobject<UC_InvenSystem>("C_InvenSystem");
	//InvenSystem->SetOwnerCharacter(this);

	StatComponent = CreateDefaultSubobject<UC_StatComponent>("StatComponent");
	
	ConsumableUsageMeshComponent = CreateDefaultSubobject<UC_ConsumableUsageMeshComponent>("ConsumableUsageMeshComponent");
	ConsumableUsageMeshComponent->SetOwnerCharacter(this);

	PoseColliderHandlerComponent = CreateDefaultSubobject<UC_PoseColliderHandlerComponent>("PoseColliderHandlerComponent");
	PoseColliderHandlerComponent->SetOwnerCharacter(this);

	DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphere"));
	DetectionSphere->InitSphereRadius(140.0f); // 탐지 반경 설정
	DetectionSphere->SetupAttachment(RootComponent);

	//DetectionSphere->SetGenerateOverlapEvents(true);
	DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &AC_BasicCharacter::OnOverlapBegin);
	DetectionSphere->OnComponentEndOverlap.AddDynamic(this, &AC_BasicCharacter::OnOverlapEnd);

	SwimmingComponent = CreateDefaultSubobject<UC_SwimmingComponent>("SwimmingComponent");
	SwimmingComponent->SetOwnerCharacter(this);

	/*SkyDiveComponent = CreateDefaultSubobject<UC_SkyDivingComponent>("SkyDivingComponent");
	SkyDiveComponent->SetOwnerCharcter(this);*/

	AttachmentMeshComponent = CreateDefaultSubobject<UC_AttachableItemMeshComponent>("AttachmentMeshComponent");
	AttachmentMeshComponent->SetOwnerCharacter(this);

	ParkourComponent = CreateDefaultSubobject<UC_ParkourComponent>("ParkourComponent");
	ParkourComponent->SetOwnerCharacter(this);

	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>("MotionWarping");

	FeetComponent = CreateDefaultSubobject<UC_FeetComponent>("FeetComponent");

	SmokeEnteredChecker = CreateDefaultSubobject<UC_SmokeEnteredChecker>("SmokeEnteredChecker");
	SmokeEnteredChecker->SetOwnerCharacter(this);

	//ReloadMagazineAudioComponent = CreateDefaultSubobject<UAudioComponent>("ReloadMagazineAudioComponent");
	//ReloadBulletAudioComponent = CreateDefaultSubobject<UAudioComponent>("ReloadBulletAudioComponent");

}

// Called when the game starts or when spawned
void AC_BasicCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetPhysicsVolume()->FluidFriction = 2.5f;
	StatComponent->SetOwnerCharacter(this);

	PoolingBullets();
	InitializeBloodParticleComponents();
	//InvenSystem->GetInvenUI()->AddToViewport();
	//InvenSystem->GetInvenUI()->SetVisibility(ESlateVisibility::Hidden);
}

void AC_BasicCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GAMESCENE_MANAGER->GetAllCharacters().Remove(this);
	GAMESCENE_MANAGER->GetAllCharacters().Remove(this);
	// Enemy와 Player remove는 각 자식 단계에서 처리
}

// Called every frame
void AC_BasicCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CountBumpTimer(DeltaTime);
}

// Called to bind functionality to input
void AC_BasicCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent); 
	
}

float AC_BasicCharacter::PlayAnimMontage(UAnimMontage* AnimMontage, float InPlayRate, FName StartSectionName)
{
	// Deprecated in UE_PUBG Project
	// Do Nothing
	UC_Util::Print
	(
		"From AC_BasicCharacter:PlayAnimMontage : This function is deprecated. Use PriorityAnimMontage parameter function instead",
		FColor::Cyan,
		10.f
	);

	return 0.0f;
}

void AC_BasicCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	HandleOverlapBegin(OtherActor);
}

void AC_BasicCharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	HandleOverlapEnd(OtherActor);
}

float AC_BasicCharacter::PlayAnimMontage(const FPriorityAnimMontage& PAnimMontage, float InPlayRate, FName StartSectionName)
{
	if (!IsValid(PAnimMontage.AnimMontage))
	{
		UC_Util::Print("From AC_BasicCharacter::PlayAnimMontage : Invalid Montage received!", FColor::Red, 10.f);
		return 0.f;
	}

	FName TargetGroup = PAnimMontage.AnimMontage->GetGroupName();

	// 자신의 group내의 anim montage가 한번도 재생된 적 없을 땐 바로 재생
	if (!CurPriorityAnimMontageMap.Contains(TargetGroup))
	{
		CurPriorityAnimMontageMap.Add(TargetGroup, PAnimMontage);
		return Super::PlayAnimMontage(PAnimMontage.AnimMontage, InPlayRate, StartSectionName);
	}

	FPriorityAnimMontage TargetGroupCurMontage = CurPriorityAnimMontageMap[TargetGroup];

	// 직전의 AnimMontage의 재생이 이미 끝났을 때
	if (!GetMesh()->GetAnimInstance()->Montage_IsPlaying(TargetGroupCurMontage.AnimMontage))
	{
		CurPriorityAnimMontageMap[TargetGroup] = PAnimMontage;
		return Super::PlayAnimMontage(PAnimMontage.AnimMontage, InPlayRate, StartSectionName);
	}

	// 현재 재생중인 PriorityAnimMontage가 있을 때
	
	// Priority 비교해서 현재 Priority보다 크거나 같은 Priority라면 새로 들어온 AnimMontage 재생
	if (PAnimMontage.Priority >= TargetGroupCurMontage.Priority)
	{
		CurPriorityAnimMontageMap[TargetGroup] = PAnimMontage;
		return Super::PlayAnimMontage(PAnimMontage.AnimMontage, InPlayRate, StartSectionName);
	}

	// Priority가 현재 재생중인 Montage가 더 크다면 새로이 재생하지 않고 그냥 return
	return 0.0f;
}

void AC_BasicCharacter::CharacterDead(const FKillFeedDescriptor& KillFeedDescriptor)
{
	// 기존 처리 유지
	if (GetMesh()->GetSkeletalMeshAsset() == ParkourComponent->GetRootedSkeletalMesh())
		ParkourComponent->SwapMeshToMainSkeletalMesh();

	MainState = EMainState::DEAD;
	Ranking = GAMESCENE_MANAGER->GetCurrentRankingAndUpdateCurrentRanking(); // 이 캐릭터의 Ranking 업데이트

	// 투척류 투척 Process 중이었을 때, 손에 든 투척류 놓치고 죽기
	if (AC_ThrowingWeapon* ThrowingWeapon = Cast<AC_ThrowingWeapon>(EquippedComponent->GetCurWeapon()))
		if (ThrowingWeapon->GetIsOnThrowProcess())
		{
			ThrowingWeapon->StartCooking();
			ThrowingWeapon->ReleaseOnGround();
		}

	// 여기에 Player의 BackToMainCamera 처리를 해야 함 (무기 장착 해제 이전에) / Player에서 CharacterDead override를 했지만, 처리 순서 때문에 여기서밖에 처리를 못함
	if (Cast<AC_Player>(this)) if (AC_Gun* Gun = Cast<AC_Gun>(EquippedComponent->GetCurWeapon())) Gun->BackToMainCamera();
	
	FVector SpawnLocation = GetActorLocation() - FVector(0, 0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	GAMESCENE_MANAGER->SpawnLootCrateAt(SpawnLocation, this);

	// 죽기 직전, 힘 제거 및 블렌딩 제거
	//GetMesh()->bApplyImpulseOnDamage = false;
	//GetMesh()->SetAllBodiesPhysicsBlendWeight(0.0f);


	// 본별 물리 속도 제거
	TArray<FName> BoneNames;
	GetMesh()->GetBoneNames(BoneNames);
	for (const FName& BoneName : BoneNames)
	{
		GetMesh()->SetPhysicsLinearVelocity(FVector::ZeroVector, false, BoneName);
		GetMesh()->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector, false, BoneName);
	}

	// 바로 래그돌 적용
	EnableRagdoll();

	// 이 캐릭터가 TargetCharacter로 잡혀있는 Enemy에 대해 Delegate 호출 처리를 해줌
	if (Delegate_OnCharacterDead.IsBound()) Delegate_OnCharacterDead.Broadcast(this);
	Delegate_OnCharacterDead.Clear();

	if (KillFeedDescriptor.DamageCauser)
	{
		if (KillFeedDescriptor.DamageCauser != KillFeedDescriptor.DamageTaker)
			KillFeedDescriptor.DamageCauser->AddKillCount();
			
	}

	// 만약 이 캐릭터를 사망에 이르게 한 캐릭터가 Player라면, Player Middle KillFeed 정보 추가 & KillCount UI 정보 업데이트
	if (AC_Player* PlayerDamageCauser = Cast<AC_Player>(KillFeedDescriptor.DamageCauser))
	{
		PlayerDamageCauser->GetHUDWidget()->GetInformWidget()->ActivateMiddleKillFeedLog(KillFeedDescriptor);
		PlayerDamageCauser->GetHUDWidget()->GetTimeBoxWidget()->SetKilledCountText(PlayerDamageCauser->GetKillCount());
	}

	// Top KillFeed 추가
	GAMESCENE_MANAGER->GetPlayer()->GetHUDWidget()->GetInformWidget()->AddTopKillFeedLog(KillFeedDescriptor);

	// AliveCount 업데이트
	int LeftCharacterCount = GAMESCENE_MANAGER->RemoveOneFromCurrentAliveCharacterCount();
	GAMESCENE_MANAGER->GetPlayer()->GetHUDWidget()->GetTimeBoxWidget()->SetAliveCountText(LeftCharacterCount);
	
	FTimerHandle& TimerHandle = GAMESCENE_MANAGER->GetTimerHandle();
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AC_BasicCharacter::DestroyCharacter, 5.f, false);
}

void AC_BasicCharacter::EnableRagdoll()
{
	// 이동 중지
	GetCharacterMovement()->DisableMovement();

	// 캡슐 비활성화
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 메쉬만 루트로 설정
	GetMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	RootComponent = GetMesh();
	
	// 💡 충돌 프로필 설정 → 지형과만 충돌
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// 📌 충돌 채널을 손수 설정 (필요시)
	GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block); // 지형과만 충돌
	
	// 💡 물리 활성화
	GetMesh()->SetSimulatePhysics(true);
	//GetMesh()->SetAllBodiesBelowSimulatePhysics(TEXT("Hips"), true, true);

	// 💡 속도 제거
	GetMesh()->SetAllPhysicsLinearVelocity(FVector::ZeroVector);
	GetMesh()->SetAllPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	
	//SetActorEnableCollision(false);

	// FTimerHandle& TimerHandle = GAMESCENE_MANAGER->GetTimerHandle();
	//GetWorldTimerManager().SetTimer(TimerHandle, this, &AC_BasicCharacter::EnableRagdoll, 2.f, false);
}

float AC_BasicCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	FString Str = "Character Damaged! Damaged Amount : " + FString::SanitizeFloat(DamageAmount);

	UC_Util::Print(Str, FColor::Cyan, 3.f);
	UC_Util::Print("TakeDamage!!", FColor::Blue, 10);

	AC_BasicCharacter* BasicCharacterDamageCauser = Cast<AC_BasicCharacter>(DamageCauser);
	if (!BasicCharacterDamageCauser)
	{
		UC_Util::Print("From AC_BasicCharacter::TakeDamage : DamageCauser casting to BasicCharacter failed!", FColor::Red, 10.f);
		return 0.f;
	}

	FKillFeedDescriptor KillFeedDescriptor =
	{
		EDamageType::Default,
		BasicCharacterDamageCauser,
		this,
		nullptr,
		false,
		0
	};

	StatComponent->TakeDamage(DamageAmount, KillFeedDescriptor);

	if (bIsHitting) return DamageAmount; //hitting Motion이 출력중인지 체크.

	bIsHitting = true;

	PlayAnimMontage(HitMontage);

	return DamageAmount;
}

void AC_BasicCharacter::HandleOverlapBegin(AActor* OtherActor)
{

}

void AC_BasicCharacter::HandleOverlapEnd(AActor* OtherActor)
{
}

void AC_BasicCharacter::DestroyCharacter()
{
	GAMESCENE_MANAGER->GetAllCharacters().Remove(this);
	GAMESCENE_MANAGER->GetAllCharacterActors().Remove(this);
	
	//this->SetActorEnableCollision(false);
	//this->SetActorHiddenInGame(true);
}

void AC_BasicCharacter::UpdateMaxWalkSpeed(const FVector2D& MovementVector)
{
	//GetCharacterMovement()->MaxWalkSpeed =	(PoseState == EPoseState::STAND)  ? 370.f :
	//										(PoseState == EPoseState::CROUCH) ? 200.f :
	//										(PoseState == EPoseState::CRAWL)  ? 100.f : 600.f;

	if (MovementVector.X == 0.f && MovementVector.Y == 0.f) // No input
	{
		GetCharacterMovement()->MaxWalkSpeed = 0.f;
		return;
	}
	
	if (SwimmingComponent->IsSwimming())
	{
		GetCharacterMovement()->MaxWalkSpeed = 300.f;
		return;
	}

	switch (PoseState)
	{
	case EPoseState::STAND:
		if (bIsWalking || bIsActivatingConsumableItem)
		{
			GetCharacterMovement()->MaxWalkSpeed = 170.f;
			break;
		}

		if (MovementVector.X == 1.f) // 앞 , 앞 대각선
		{
			GetCharacterMovement()->MaxWalkSpeed = bIsSprinting ? 630.f : 470.f; 
			break;
		}
		if (MovementVector.X != -1.f && MovementVector.Y != 0.f) // Left right side
		{
			GetCharacterMovement()->MaxWalkSpeed = 400.f; 
			break;
		}

		GetCharacterMovement()->MaxWalkSpeed = 350.f; // 뒷 방향
		break;
	case EPoseState::CROUCH:
		if (bIsWalking || bIsActivatingConsumableItem)
		{
			GetCharacterMovement()->MaxWalkSpeed = 130.f;
			break;
		}
		if (MovementVector.X == 1.f) // 앞 , 앞 대각선
		{
			GetCharacterMovement()->MaxWalkSpeed = bIsSprinting ? 480.f : 340.f;
			break;
		}
		if (MovementVector.X != -1.f && MovementVector.Y != 0.f) // Left right side
		{
			GetCharacterMovement()->MaxWalkSpeed = 290.f;
			break;
		}

		GetCharacterMovement()->MaxWalkSpeed = 250.f; // 뒷 방향
		break;
	case EPoseState::CRAWL:
		GetCharacterMovement()->MaxWalkSpeed =	bIsActivatingConsumableItem ? 0.f :
												bIsWalking ? 60.f : 120.f;
		break;
	case EPoseState::POSE_MAX:
		break;
	default:
		break;
	}

	GetCharacterMovement()->MaxWalkSpeed *= BoostingSpeedFactor;
}

void AC_BasicCharacter::SetPoseState(EPoseState InPoseState)
{
	PoseState = InPoseState;
	PoseColliderHandlerComponent->SetColliderByPoseState(PoseState);
}

bool AC_BasicCharacter::SetPoseState(EPoseState InChangeFrom, EPoseState InChangeTo)
{
	// TODO : Enemy 캐릭터에 대한 자세 변환 적용 - Player는 작성 완료 BasicCharacter 작성 필요

	return false;
}

bool AC_BasicCharacter::GetIsHighEnoughToFall()
{
	return false;
}

bool AC_BasicCharacter::GetIsTooCloseToAimGun()
{
	if (EquippedComponent->GetCurWeaponType() != EWeaponSlot::MAIN_GUN && EquippedComponent->GetCurWeaponType() != EWeaponSlot::SUB_GUN) return false;
	FCollisionQueryParams CollisionParams{};
	CollisionParams.AddIgnoredActor(this);
	CollisionParams.AddIgnoredActor(EquippedComponent->GetCurWeapon());
	//CollisionParams.AddIgnoredActor(GAMESCENE_MANAGER->GetAirplaneManager()->GetAirplane());

	FHitResult HitResult{};
	FVector ForwardDirection = GetActorForwardVector().GetSafeNormal();
	FVector StartLocation = GetActorLocation();
	FVector DestLocation = StartLocation + ForwardDirection * 50.0f;

	bool HasHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, DestLocation, ECollisionChannel::ECC_Visibility, CollisionParams);
	return HasHit;
}

void AC_BasicCharacter::SetIsActivatingConsumableItem(bool InIsActivatingConsumableItem, AC_ConsumableItem* ActivatingConsumableItem)
{
	bIsActivatingConsumableItem = InIsActivatingConsumableItem;
	CurActivatingConsumableItem = ActivatingConsumableItem;
}

void AC_BasicCharacter::OnPoseTransitionGoing()
{
	PoseState = NextPoseState;
	PoseColliderHandlerComponent->SetColliderByPoseState(PoseState);
}

void AC_BasicCharacter::OnPoseTransitionFinish()
{
	//UC_Util::Print("Transition pose finished!", FColor::Cyan, 5.f);
	//PoseState = NextPoseState;
	//bCanMove = true; // PoseBySizeLerp가 끝난 뒤에 움직일 수 있도록 처리
	bIsPoseTransitioning = false;

	// Pose Transition이 끝난 뒤, Delegate call back 처리
	if (Delegate_OnPoseTransitionFin.IsBound()) Delegate_OnPoseTransitionFin.Broadcast();
}

bool AC_BasicCharacter::ExecutePoseTransitionAction(const FPriorityAnimMontage& TransitionMontage, EPoseState InNextPoseState)
{
	// 다른 PriorityAnimMontage에 의해 자세전환이 안된 상황이면 return false
	if (PlayAnimMontage(TransitionMontage) == 0.f) return false;

	// 이미 자세를 바꾸는 중이라면 return false
	//if (bIsPoseTransitioning) return false;

	NextPoseState			= InNextPoseState;
	bCanMove				= false;
	bIsPoseTransitioning	= true;

	// 만약에 현재 들고 있는 무기가 Gun이라면 Gun에 대한 추가 TransitionAction 처리
	if (AC_Gun* CurGun = Cast<AC_Gun>(EquippedComponent->GetCurWeapon()))
		ExecuteGunTransitionAction(CurGun, InNextPoseState);
	
	return true;
}

void AC_BasicCharacter::ExecuteGunTransitionAction(AC_Gun* CurGun, EPoseState InNextPoseState)
{
	AC_SR* TempSR = Cast<AC_SR>(CurGun);
	if (IsValid(TempSR)) if (TempSR->GetIsReloadingSR()) return;
	
	if (bIsReloadingBullet && IsValid(CurGun))
	{
		UAnimInstance* TempAnimInstance  = GetMesh()->GetAnimInstance();
		UAnimMontage*  CurReloadMontage  = nullptr;
		UAnimMontage*  NextReloadMontage = CurGun->ReloadMontages[InNextPoseState].Montages[CurGun->GetCurrentWeaponState()].AnimMontage;
		if (IsValid(TempAnimInstance)  && IsValid(NextReloadMontage))
		{
			float HighestWeight = 0.0f;
			for (const FAnimMontageInstance* MontageInstance : TempAnimInstance->MontageInstances)
			{
				if (MontageInstance->IsActive())
				{
					float CurrentWeight = MontageInstance->GetWeight();
					if (CurrentWeight > HighestWeight)
					{
						HighestWeight = CurrentWeight;
						CurReloadMontage = MontageInstance->Montage;
					}
				}
			}

			if (TempAnimInstance->Montage_IsPlaying(CurReloadMontage)) 
			{
				float CurrentPosition = TempAnimInstance->Montage_GetPosition(CurReloadMontage);
				float CurrentMontageLength = CurReloadMontage->GetPlayLength();
				float PlayRatio = 0.0f;
				if (CurrentMontageLength> 0.0f && CurrentPosition > 0.0f)
				{
					PlayRatio = CurrentPosition/CurrentMontageLength;
				}
				float NextMontageLength = NextReloadMontage->GetPlayLength();
				float NextStartPosition = NextMontageLength * PlayRatio;

				TempAnimInstance->Montage_Stop(0.1f,CurReloadMontage);
				TempAnimInstance->Montage_Play(NextReloadMontage);
				TempAnimInstance->Montage_SetPosition(NextReloadMontage, NextStartPosition);
			}
		}
	}
}

void AC_BasicCharacter::Jump()
{
	Super::Jump();
	bIsJumping = true;
}

void AC_BasicCharacter::PoolingBullets()
{
	FActorSpawnParameters Param2{};
	Param2.Owner = this;
	for (int i = 0; i < 30; i++)
	{
		UClass* BulletBPClass = StaticLoadClass(AC_Bullet::StaticClass(), nullptr, TEXT("/Game/Project_PUBG/Hyunho/Weapon/Bullet/BPC_Bullet.BPC_Bullet_C"));
		AC_Bullet* Bullet = GetWorld()->SpawnActor<AC_Bullet>(BulletBPClass, Param2);
		Bullet->SetInstanceNum(i);
		Bullet->DeactivateInstance();
		if (IsValid(Bullet))
		{
			Bullet->SetOwnerCharacter(this);
			//UC_Util::Print("Created Bullet");
			PooledBullets.Add(Bullet);
		}
	}
	for (auto& Bullet : PooledBullets)
	{
		Bullet->ActivateInstance();
		Bullet->DeactivateInstance();
	}
}

void AC_BasicCharacter::ActivateBloodParticle(FVector InLocation)
{
	for (auto BloodEffect : BloodParticleComponents)
	{
		if (BloodEffect->IsActive()) continue;
		BloodEffect->SetWorldLocation(InLocation);
		BloodEffect->SetActive(true);
		return;
	}
}

void AC_BasicCharacter::InitializeBloodParticleComponents()
{
	UParticleSystem* DestructionParticle = LoadObject<UParticleSystem>(
	nullptr, 
	TEXT("/Game/Project_PUBG/Common/VFX/Realistic_Starter_VFX_Pack_Vol2/Particles/Destruction/P_Destruction_Building_A"));
	// 유효성 검사
	if (!IsValid(DestructionParticle)) return;
	for (int32 i = 0; i < 10; i++)
	{
		UParticleSystemComponent* ParticleComp = NewObject<UParticleSystemComponent>(this);
        
		if (ParticleComp)
		{
			ParticleComp->RegisterComponent();
			ParticleComp->SetTemplate(DestructionParticle);
			//ParticleComp->SetWorldScale3D(FVector(0.1f, 0.1f, 0.1f));
			ParticleComp->Deactivate();
			// 배열에 추가
			BloodParticleComponents.Add(ParticleComp);

			// 파티클 실행
			//ParticleComp->Activate(true);
			
			//UC_Util::Print("Spawned Blood");
		}
	}
}

void AC_BasicCharacter::CountBumpTimer(float DeltaTime)
{
	BumpTimer -= DeltaTime;
	if (BumpTimer < 0) BumpTimer = 0.f;
}



