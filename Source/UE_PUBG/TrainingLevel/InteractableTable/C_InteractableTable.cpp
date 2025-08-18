// Fill out your copyright notice in the Description page of Project Settings.


#include "C_InteractableTable.h"

#include "Character/C_Player.h"
#include "Components/ShapeComponent.h"
#include "Components/WidgetComponent.h"
#include "Utility/C_Util.h"


AC_InteractableTable::AC_InteractableTable()
{
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootComp);
}

void AC_InteractableTable::BeginPlay()
{
	Super::BeginPlay();
	
	BoxColliderComponent = Cast<UShapeComponent>(GetDefaultSubobjectByName(TEXT("BoxCollider")));
	if (!BoxColliderComponent) UC_Util::Print("From AC_InteractableTable::BeginPlay : Invalid BoxComponent!", FColor::Red, 10.f);

	BoxColliderComponent->OnComponentBeginOverlap.AddDynamic(this, &AC_InteractableTable::OnBoxColliderBeginOverlap);
	BoxColliderComponent->OnComponentEndOverlap.AddDynamic(this, &AC_InteractableTable::OnBoxColliderEndOverlap);

	StaticMeshComponent = Cast<UStaticMeshComponent>(GetDefaultSubobjectByName(TEXT("StaticMesh")));
	
	if (StaticMeshComponent) GetComponents<UPrimitiveComponent>(PrimitiveComponents);
	else UC_Util::Print("From AC_InteractableTable::BeginPlay : Invalid StaticMeshComponent!", FColor::Red, 10.f);

	TriangleWidgetComponent = Cast<UWidgetComponent>(GetDefaultSubobjectByName(TEXT("TriangleWidget")));
	if (TriangleWidgetComponent)
	{
		TriangleWidget = TriangleWidgetComponent->GetWidget();
		TriangleWidget->SetRenderOpacity(1.f);
	}
	else UC_Util::Print("From AC_InteractableTable::BeginPlay : Invalid TriangleWidgetComponent!", FColor::Red, 10.f);
}

void AC_InteractableTable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const float TriangleWidgetOpacity = FMath::Lerp(TriangleWidget->GetRenderOpacity(), TriangleWidgetOpacityDestination, DeltaTime * 10.f);
	TriangleWidget->SetRenderOpacity(TriangleWidgetOpacity);

	if (TriangleWidgetOpacity > KINDA_SMALL_NUMBER)
	{
		FRotator Rotation = TriangleWidgetComponent->GetRelativeRotation();
		Rotation.Yaw += 102.f * DeltaTime;
		TriangleWidgetComponent->SetRelativeRotation(Rotation);
	}
}

void AC_InteractableTable::OnBoxColliderBeginOverlap
(
	UPrimitiveComponent*	OverlappedComp,
	AActor*					OtherActor,
	UPrimitiveComponent*	OtherComp,
	int32					OtherBodyIndex,
	bool					bFromSweep,
	const FHitResult&		SweepResult
)
{
	if (!Cast<AC_Player>(OtherActor)) return;

	// Focused effect 켜기
	for (UPrimitiveComponent* PrimitiveComponent : PrimitiveComponents)
	{
		if (!PrimitiveComponent) continue;
		PrimitiveComponent->SetRenderCustomDepth(true);
		PrimitiveComponent->SetCustomDepthStencilValue(1);

		TriangleWidgetOpacityDestination = 0.f;
		bIsFocused = true;
	}
}

void AC_InteractableTable::OnBoxColliderEndOverlap
(
	UPrimitiveComponent*	OverlappedComp,
	AActor*					OtherActor,
	UPrimitiveComponent*	OtherComp,
	int32					OtherBodyIndex
)
{
	if (!Cast<AC_Player>(OtherActor)) return;
	
	// Focused Effect 끄기
	for (UPrimitiveComponent* PrimitiveComponent : PrimitiveComponents)
	{
		if (!PrimitiveComponent) continue;
		PrimitiveComponent->SetRenderCustomDepth(false);
		PrimitiveComponent->SetCustomDepthStencilValue(0);
		
		TriangleWidgetOpacityDestination = 1.f;
		bIsFocused = false;
	}
}

