// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Character/C_BasicCharacter.h"

#include "C_PoseColliderHandlerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE_PUBG_API UC_PoseColliderHandlerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UC_PoseColliderHandlerComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:

	void SetOwnerCharacter(class AC_BasicCharacter* InOwnerCharacter);

	void SetColliderByPoseState(EPoseState InPoseState);

public:

	/// <summary>
	/// ���� �ֺ�ȯ�濡�� �÷��̾� ĳ���Ͱ� �ڼ��� �ٲ� �� �ִ��� üũ
	/// </summary>
	/// <param name="InChangeTo"> : �ٲٷ��� �ϴ� �ڼ� </param>
	/// <returns> : �ٲ� �� �ִٸ� return true </returns>
	bool CanChangePoseOnCurrentSurroundEnvironment(EPoseState InChangeTo);

private:

	/// <summary>
	/// �ڼ� ��ȯ�� ���� Body Collider size �� Mesh ��ġ Lerp�� ���߱�
	/// </summary>
	/// <param name="DeltaTime"></param>
	void HandleLerpBodySizeByPose(const float& DeltaTime);

	/// <summary>
	/// Crawl ���� ��, Crawl Collider ���������� ���� ȸ�� �����ֱ�
	/// </summary>
	/// <param name="DeltaTime"></param>
	void HandleCrawlColliderRotation(const float& DeltaTime);

private:

	// TODO : Parameter�� Start�� Dest Location �������� -> �ٸ� �������� ��� ����
	float GetCrawlSlopeDegree();

	/// <summary>
	/// ������ HeadLocation�� PelvisLocation�� �־��� ��, �ش� ��ġ�� ��絵 ���ϱ�
	/// </summary>
	/// <param name="HeadLocation"> : ������ Head Location </param>
	/// <param name="PelvisLocation"> : ������ Pelvis Location </param>
	/// <returns> : ��絵 </returns>
	float GetCrawlSlopeDegree(const FVector& HeadLocation, const FVector& PelvisLocation);

private:
	
	class AC_BasicCharacter* OwnerCharacter{};

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UCapsuleComponent* CrawlCapsuleComponent{};

private:

	// �ڼ��� CapsuleComponent(RootComponent) Collider ���̿� Radius
	const TMap<EPoseState, TPair<float, float>> POSE_BY_ROOTCOLLIDER_HEIGHT_RADIUS =
	{
		{EPoseState::STAND,		{88.f, 34.f}},
		{EPoseState::CROUCH,	{67.f, 34.f}},
		{EPoseState::CRAWL,		{20.f, 20.f}}
		//{EPoseState::CRAWL,		{1.f, 1.f}}

	};

	// �ڼ� �� Mesh Z Pos
	const TMap<EPoseState, float> POSE_BY_MESH_Z_POS =
	{
		{EPoseState::STAND,		-90.f},
		{EPoseState::CROUCH,	-64.f},
		{EPoseState::CRAWL,		-20.f}
	};

	// �ڼ��� ũ�� �� ��ġ Lerp Destination
	TPair<float, float> RootColliderHeightRadiusLerpDest = POSE_BY_ROOTCOLLIDER_HEIGHT_RADIUS[EPoseState::STAND];
	float MeshZPosLerpDest = POSE_BY_MESH_Z_POS[EPoseState::STAND];
	float PoseBySizePosLerpSpeed = 10.f;
	
	// Lerp�� �����ϴ��� üũ
	bool PoseBySizeLerpFlag{};

private: // ChangePose Sweep testing constants

	const float SWEEP_SPHERE_RAD			= 34.f;
	const float CROUCH_TO_STAND_SWEEP_DIST	= (POSE_BY_ROOTCOLLIDER_HEIGHT_RADIUS[EPoseState::STAND].Key -
											   POSE_BY_ROOTCOLLIDER_HEIGHT_RADIUS[EPoseState::CROUCH].Key) * 2.f;
	const float CRAWL_TO_STAND_SWEEP_DIST	= 105.f;
	const float CRAWL_TO_CROUCH_SWEEP_DIST	= 60.f;

	const float CRAWL_LINETRACE_TEST_DIST	= 500.f;
	const float CRAWL_DEGREE_LIMIT			= 30.f; // 30�� �̻� ��絵 �� �� ���� ó��

private: // Crawl Collider Rotation Lerp ����

	float CrawlColliderPitchLerpDest{};
};
