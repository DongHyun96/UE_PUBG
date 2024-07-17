// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/ThrowingWeapon/C_ThrowingWeapon.h"

#include "Item/Weapon/WeaponStrategy/I_WeaponButtonStrategy.h"
#include "Item/Weapon/WeaponStrategy/C_ThrowingWeaponStrategy.h"

#include "Components/ShapeComponent.h"
#include "Components/CapsuleComponent.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Character/Component/C_EquippedComponent.h"
#include "Character/C_AnimBasicCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

TArray<AC_ThrowingWeapon*> AC_ThrowingWeapon::ThrowablePool{};

AC_ThrowingWeapon::AC_ThrowingWeapon()
{
	WeaponButtonStrategy = CreateDefaultSubobject<AC_ThrowingWeaponStrategy>("ThrowingWeaponStrategy");

	//RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRootComponent"));

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");

	Collider = CreateDefaultSubobject<UCapsuleComponent>("Capsule");
	Collider->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//RootComponent = Collider;
}

void AC_ThrowingWeapon::BeginPlay()
{
	Super::BeginPlay();

	ProjectileMovement->Deactivate();
}

void AC_ThrowingWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsValid(OwnerCharacter))
		return;

	CurDrawMontage			= DrawMontages[OwnerCharacter->GetPoseState()];
	CurSheathMontage		= SheathMontages[OwnerCharacter->GetPoseState()];
	CurThrowProcessMontages = ThrowProcessMontages[OwnerCharacter->GetPoseState()];
	
	HandlePredictedPath();
}

bool AC_ThrowingWeapon::AttachToHolster(USceneComponent* InParent)
{
	// TODO : ��ô���� �ɱ����� �̾Ұ� ��ŷ�� ������ �� �ٽ� �������
	// TODO : ��ô���� ���������̱��� �̾Ҵٸ� ���� ��ġ�� ���� ��ô�� �׳� �ٴڿ� ����

	SetActorHiddenInGame(true);

	ProjectileMovement->Deactivate();

	return AttachToComponent
	(
		InParent,
		FAttachmentTransformRules(EAttachmentRule::KeepRelative, true),
		HOLSTER_SOCKET_NAME
	);
}

bool AC_ThrowingWeapon::AttachToHand(USceneComponent* InParent)
{
	//this->SetHidden(false);

	SetActorHiddenInGame(false);

	ProjectileMovement->Deactivate();

	OwnerCharacter->SetHandState(EHandState::WEAPON_THROWABLE);
	
	// Self init
	bIsCharging = false;
	bIsOnThrowProcess = false;

	UINT numOfGrenade =  ThrowablePool.Num() + 1;
	FString DebugMessage = "Left Grenade Count : " + FString::FromInt(numOfGrenade);
	GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, *DebugMessage);

	return AttachToComponent
	(
		InParent,
		FAttachmentTransformRules(EAttachmentRule::KeepRelative, true),
		EQUIPPED_SOCKET_NAME
	);
}

void AC_ThrowingWeapon::InitTestPool(AC_BasicCharacter* InOwnerCharacter, UClass* Class, UC_EquippedComponent* EquippedComponent)
{
	ThrowablePool.Empty();

	// �賶�� �ִ� ��ó�� ����
	for (int i = 0; i < TESTPOOLCNT; i++)
	{
		FActorSpawnParameters Param{};
		Param.Owner = InOwnerCharacter;

		AC_ThrowingWeapon* ThrowWeapon = EquippedComponent->GetWorld()->SpawnActor<AC_ThrowingWeapon>(Class, Param);

		if (ThrowWeapon)
		{
			ThrowWeapon->SetOwnerCharacter(InOwnerCharacter);
			ThrowablePool.Add(ThrowWeapon);
		}
	}
}

void AC_ThrowingWeapon::OnRemovePinFin()
{
	OwnerCharacter->PlayAnimMontage(CurThrowProcessMontages.ThrowReadyMontage);
}

void AC_ThrowingWeapon::OnThrowReadyLoop()
{
	if (!bIsCharging) OwnerCharacter->PlayAnimMontage(CurThrowProcessMontages.ThrowMontage);
}

void AC_ThrowingWeapon::OnThrowThrowable()
{
	// TODO : ���������� ������
	// ���⼭ ������ ����ź�� ���� ���� Destroy self
	
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	// Direction ���ϴ� ��� 1
	/*FVector ActorForward = FRotationMatrix(OwnerCharacter->GetActorRotation()).GetUnitAxis(EAxis::X);

	UC_AnimBasicCharacter* OwnerAnim = Cast<UC_AnimBasicCharacter>(OwnerCharacter->GetMesh()->GetAnimInstance());
	
	if (!IsValid(OwnerAnim))
	{
		FString DebugMessage = "AC_ThrowingWeapon::OnThrowThrowable : OwnerAnim not valid!";
		GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, *DebugMessage);

		Direction = ActorForward;
	}
	else
	{
		FRotator SpineRotation	  = OwnerAnim->GetCSpineRotation();
		FVector SpineForward	  = FRotationMatrix(SpineRotation).GetUnitAxis(EAxis::X);
		FVector CombinedDirection = FRotationMatrix(OwnerCharacter->GetActorRotation()).TransformVector(SpineForward);
		Direction = CombinedDirection;
	}*/

	UpdateProjectileLaunchValues();

	Collider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	this->SetActorLocation(ProjStartLocation);
	
	ProjectileMovement->Velocity = ProjLaunchVelocity;
	ProjectileMovement->Activate();
}

void AC_ThrowingWeapon::OnThrowProcessEnd()
{
	bIsOnThrowProcess = false;

	// ���� Throw AnimMontage ���߱� (�켱���� ������ ����� ��)
	OwnerCharacter->GetMesh()->GetAnimInstance()->Montage_Stop(0.2f);

	UC_EquippedComponent* OwnerEquippedComponent = OwnerCharacter->GetEquippedComponent();

	// ���� ��ô�� ���� ���� �ٷ� �ϱ�
	OwnerEquippedComponent->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, nullptr);

	// TODO : ���濡 ��ô�� �ִ��� Ȯ���ؼ� �ٷ� ���� ���� �����ϱ� (�켱���� - ���� ������ ��ô��)

	if (!ThrowablePool.IsEmpty())
	{
		AC_ThrowingWeapon* ThrowWeapon = ThrowablePool[0];

		OwnerEquippedComponent->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, ThrowWeapon);

		// ���濡 �ִ� ��ô�� �ϳ� �����
		ThrowablePool.RemoveAt(0);

		// �ٷ� ���� ��ô�� ������
		OwnerCharacter->PlayAnimMontage(ThrowWeapon->GetCurDrawMontage());

		return;
	}

	/*
	���� �ִ� ��ô���� ���ٸ� �ֹ���1, �ֹ���2 ������ ���� �õ�, ���ٸ� UnArmed�� ���·� ���ư���
	*/

	if (OwnerEquippedComponent->ChangeCurWeapon(EWeaponSlot::MAIN_GUN)) return;
	if (OwnerEquippedComponent->ChangeCurWeapon(EWeaponSlot::SUB_GUN)) return;

	OwnerEquippedComponent->ChangeCurWeapon(EWeaponSlot::NONE);
}

void AC_ThrowingWeapon::DrawDebugPredictedPath()
{
	UpdateProjectileLaunchValues();

	static const float MaxSimTime	= 1.0f; // Max time to simulate
	static const float TimeStep		= 0.1f; // Time step for simulation
	FVector Gravity					= GetWorld()->GetGravityZ() * FVector::UpVector;

	TArray<FVector> PathPoints{};
	FVector CurrentLocation = ProjStartLocation;
	FVector CurrentVelocity = ProjLaunchVelocity;

	for (float Time = 0; Time < MaxSimTime; Time += TimeStep)
	{
		FVector NextLocation = CurrentLocation + CurrentVelocity * TimeStep + 0.5f * Gravity * TimeStep * TimeStep;
		CurrentVelocity += Gravity * TimeStep;

		PathPoints.Add(NextLocation);
		CurrentLocation = NextLocation;

		if (PathPoints.Num() > 1)
		{
			DrawDebugLine
			(
				GetWorld(),
				PathPoints[PathPoints.Num() - 2], PathPoints[PathPoints.Num() - 1],
				FColor::Green,
				false,
				TimeStep * 2.0f,
				0,
				1.0f
			);
		}
	}
}

void AC_ThrowingWeapon::DrawNiagaraPredictedPath()
{
	UpdateProjectileLaunchValues();

	static const float MaxSimTime	= 1.0f; // Max time to simulate
	static const float TimeStep		= 0.1f; // Time step for simulation
	FVector Gravity					= GetWorld()->GetGravityZ() * FVector::UpVector;

	TArray<FVector> PathPoints{};
	FVector CurrentLocation = ProjStartLocation;
	FVector CurrentVelocity = ProjLaunchVelocity;

	UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation
	(
		GetWorld(),
		NiagaraSystem,
		ProjStartLocation
	);

	for (float Time = 0; Time < MaxSimTime; Time += TimeStep)
	{
		FVector NextLocation = CurrentLocation + CurrentVelocity * TimeStep + 0.5f * Gravity * TimeStep * TimeStep;
		CurrentVelocity = CurrentVelocity + Gravity * TimeStep;

		PathPoints.Add(NextLocation);
		CurrentLocation = NextLocation;

		// Update Niagara component location to follow the path
		NiagaraComponent->SetWorldLocation(CurrentLocation);
		NiagaraComponent->SetNiagaraVariableVec3(FString("User.Position"), CurrentLocation);
	}

}

void AC_ThrowingWeapon::HandlePredictedPath()
{
	bDrawPredictedPath = false;

	// Projectile Path
	// TODO : �÷��̾��� ��쿡�� �׸��� (����, GameManager ��������� Player�� ��ü �����غ� ��)

	// ���� OwnerCharacter�� �տ� ������ ��Ȳ���� Ȯ��
	if (!IsValid(this->GetAttachParentActor()) || this->GetAttachParentSocketName() != EQUIPPED_SOCKET_NAME) return;

	if (!OwnerCharacter->GetMesh()->GetAnimInstance()->Montage_IsPlaying(CurThrowProcessMontages.ThrowReadyMontage.AnimMontage))
		return;

	bDrawPredictedPath = true;

	UpdateProjectileLaunchValues();

	//DrawDebugPredictedPath();
	//DrawNiagaraPredictedPath();
}

void AC_ThrowingWeapon::UpdateProjectileLaunchValues()
{
	// TODO : Init ProjStartLocation & ProjLaunchVelocity
	// TODO : Enemy AI�� ��� ����ź ������ ������ �ٸ� ������� ������� ��

	FRotator CameraRotation = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetCameraRotation();

	FVector Direction = FRotationMatrix(CameraRotation).GetUnitAxis(EAxis::X).GetSafeNormal();

	ProjStartLocation     = OwnerCharacter->GetMesh()->GetSocketLocation(THROW_START_SOCKET_NAME);
	ProjLaunchVelocity    = Direction * Speed;
	ProjLaunchVelocity.Z += UP_DIR_BOOST_OFFSET;
}
