// Fill out your copyright notice in the Description page of Project Settings.


#include "C_SmokeOverlappedHandler.h"

#include "C_ThrowingWeapon.h"
#include "Character/Component/C_SmokeEnteredChecker.h"
#include "Components/BoxComponent.h"
#include "Utility/C_Util.h"


UC_SmokeOverlappedHandler::UC_SmokeOverlappedHandler()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UC_SmokeOverlappedHandler::BeginPlay()
{
	Super::BeginPlay();

	SmokeAreaCollider = Cast<UBoxComponent>(GetOwner()->GetComponentByClass(UBoxComponent::StaticClass()));
	if (!SmokeAreaCollider)
	{
		UC_Util::Print("From UC_SmokeOverlappedHandler::BeginPlay : Smoke Area Collider not inited!", FColor::Red, 10.f);
		return;
	}

	SmokeAreaCollider->OnComponentBeginOverlap.AddDynamic(this, &UC_SmokeOverlappedHandler::OnSmokeAreaBeginOverlap);
	SmokeAreaCollider->OnComponentEndOverlap.AddDynamic(this, &UC_SmokeOverlappedHandler::OnSmokeAreaEndOverlap);
}

void UC_SmokeOverlappedHandler::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bHasExploded) return;
	UpdateSmokeAreaCollider(DeltaTime);
}

bool UC_SmokeOverlappedHandler::SetOwnerSmokeGrenade(AC_ThrowingWeapon* InOwnerSmokeGrenade)
{
	if (InOwnerSmokeGrenade->GetThrowableType() != EThrowableType::SMOKE) return false;
	OwnerSmokeGrenade = InOwnerSmokeGrenade;
	return true;
}

void UC_SmokeOverlappedHandler::OnExplodeStart()
{
	SmokeAreaCollider->SetWorldRotation(FRotator(0.f, 0.f, 0.f));
	SmokeAreaCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SmokeAreaOriginLocation = SmokeAreaCollider->GetComponentLocation(); 
	bHasExploded = true;
}

void UC_SmokeOverlappedHandler::OnDestroyOwnerSmokeGrenade()
{
	if (Delegate_OnSmokeDurationFinished.IsBound())
		Delegate_OnSmokeDurationFinished.Broadcast(this);

	Delegate_OnSmokeDurationFinished.Clear();
}

FVector UC_SmokeOverlappedHandler::GetRandomLocationInSmokeArea()
{
	FVector BoxExtent = SmokeAreaCollider->GetScaledBoxExtent();

	FVector MinPoint = -BoxExtent;
	FVector MaxPoint = BoxExtent;
	
	FVector RandomLocation{};
	RandomLocation.X = FMath::RandRange(MinPoint.X, MaxPoint.X);
	RandomLocation.Y = FMath::RandRange(MinPoint.Y, MaxPoint.Y);
	RandomLocation.Z = FMath::RandRange(MinPoint.Z, MaxPoint.Z);

	return SmokeAreaCollider->GetComponentTransform().TransformPosition(RandomLocation);
}

void UC_SmokeOverlappedHandler::UpdateSmokeAreaCollider(const float& DeltaTime)
{
	static const float XAndYExtentDest			= 650.f;
	static const float XAndYExtentTotalDuration = 13.f;
	static const float ZExtentDest				= 100.f;
	static const float ZExtentTotalDuration		= 5.f;

	Timer += DeltaTime;

	float XYLerpAlpha = FMath::Clamp(Timer / XAndYExtentTotalDuration, 0.f, 1.f);
	float ZLerpAlpha  = FMath::Clamp(Timer / ZExtentTotalDuration, 0.f, 1.f);
	float XAndY       = FMath::Lerp(1.f, XAndYExtentDest, XYLerpAlpha);
	float Z           = FMath::Lerp(1.f, ZExtentDest, ZLerpAlpha);

	SmokeAreaCollider->SetBoxExtent(FVector(XAndY, XAndY, Z));

	FVector NewLocation = SmokeAreaOriginLocation + FVector::UnitZ() * Z;
	SmokeAreaCollider->SetWorldLocation(NewLocation);
}

void UC_SmokeOverlappedHandler::OnSmokeAreaBeginOverlap
(
	UPrimitiveComponent*	OverlappedComponent,
	AActor*					OtherActor,
	UPrimitiveComponent*	OtherComp,
	int32					OtherBodyIndex,
	bool					bFromSweep,
	const FHitResult&		SweepResult
)
{
	if (AC_BasicCharacter* Character = Cast<AC_BasicCharacter>(OtherActor))
		Character->GetSmokeEnteredChecker()->AddEnteredSmoke(this);
}

void UC_SmokeOverlappedHandler::OnSmokeAreaEndOverlap
(	UPrimitiveComponent*	OverlappedComponent,
	AActor*					OtherActor,
	UPrimitiveComponent*	OtherComp,
	int32					OtherBodyIndex
)
{
	if (AC_BasicCharacter* Character = Cast<AC_BasicCharacter>(OtherActor))
		Character->GetSmokeEnteredChecker()->RemoveEnteredSmoke(this);
}
