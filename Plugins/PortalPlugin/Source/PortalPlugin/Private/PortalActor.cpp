// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalActor.h"
#include "Components/BoxComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
APortalActor::APortalActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(FName("DefaultSceneRoot"));
	PortalPlaneMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName("PortalPlaneMesh"));
	CollisionShapeComponent = CreateDefaultSubobject<UBoxComponent>(FName("BoxCollision"));
	SceneCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(FName("SceneCapture"));

	SetRootComponent(DefaultSceneRoot);
	PortalPlaneMeshComponent->SetupAttachment(DefaultSceneRoot);
	CollisionShapeComponent->SetupAttachment(DefaultSceneRoot);
	SceneCaptureComponent->SetupAttachment(DefaultSceneRoot);

	PortalPlaneMeshComponent->SetRelativeRotation(FRotator(-90, 0, 0));
	PortalPlaneMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SceneCaptureComponent->bEnableClipPlane = true;
	SceneCaptureComponent->CaptureSource = ESceneCaptureSource::SCS_SceneColorHDR;
}

// Called when the game starts or when spawned
void APortalActor::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = GetWorld()->GetFirstPlayerController();

	SetTickGroup(ETickingGroup::TG_PostUpdateWork);
	CreatePortalMaterialInstance();
	CreatePortalRenderTextureTarget();
	SetLinkedPortal(LinkedPortal.Get());

}

// Called every frame
void APortalActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdatePortalCapturedScene();
	CheckRenderTextureTargetSize();
}

void APortalActor::SetLinkedPortal(APortalActor* OtherPortal)
{
	// 기존에 연결된 포탈의 SceneCapture Component의 TextureTarget 초기화
	if (LinkedPortal.IsValid() == true)
	{
		LinkedPortal->SceneCaptureComponent->TextureTarget = nullptr;
	}

	// 연결하려는 포탈의 유효성 검사
	if (IsValid(OtherPortal) == false || OtherPortal == this)
	{
		// 연결된 포탈 제거
		LinkedPortal = nullptr;

		return;
	}

	// 연결된 포탈 변경
	LinkedPortal = OtherPortal;

	// 연결하는 포탈의 SceneCapture Component의 TextureTarget 설정
	OtherPortal->SceneCaptureComponent->TextureTarget = RenderTextureTarget;

	// 현재 포탈의 SceneCapture Component의 TextureTarget 설정
	SceneCaptureComponent->TextureTarget = OtherPortal->RenderTextureTarget;

	// 현재 포탈의 위치 기준, 정면 방향으로 Clip
	SceneCaptureComponent->ClipPlaneBase = PortalPlaneMeshComponent->GetComponentLocation();
	SceneCaptureComponent->ClipPlaneNormal = GetActorForwardVector();
	return;
}

void APortalActor::CreatePortalMaterialInstance()
{
	// 생성된 Material Instance가 유효하면 Mesh의 Material으로 설정
	PortalMaterialInstance = UMaterialInstanceDynamic::Create(PortalMaterial, this);
	if (IsValid(PortalMaterialInstance) == true)
	{
		PortalPlaneMeshComponent->SetMaterial(0, PortalMaterialInstance);
	}
}

void APortalActor::CreatePortalRenderTextureTarget()
{
	// 연결된 다른 포탈의 SceneCapture Component에서 캡처한 Scene을 저장할 RenderTexture의 Instance 생성
	RenderTextureTarget = NewObject<UTextureRenderTarget2D>(this);
	if (IsValid(RenderTextureTarget) == false)
	{
		return;
	}

	// Render Texture의 기본 설정 진행
	RenderTextureTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA16f;
	RenderTextureTarget->ClearColor = FLinearColor::Transparent;
	RenderTextureTarget->bAutoGenerateMips = false;
	RenderTextureTarget->bSupportsUAV = false;

	// Render Texture의 크기는 사용자 Viewport의 크기와 동일하게 설정
	int32 ViewportWidth, ViewportHeight;
	PlayerController->GetViewportSize(ViewportWidth, ViewportHeight);
	RenderTextureTarget->InitAutoFormat(ViewportWidth, ViewportHeight);
	RenderTextureTarget->UpdateResourceImmediate(true);

	// Logging
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Black, *FString::Printf(TEXT("Viewport Size = (%d, %d)"), ViewportWidth, ViewportHeight));

	// 포탈에 사용된 Material Instance에 대하여 Texture 파라미터 값 변경
	if (IsValid(PortalMaterialInstance) == true)
	{
		PortalMaterialInstance->SetTextureParameterValue(MaterialTextureParameterName, RenderTextureTarget);
	}
}

void APortalActor::UpdatePortalCapturedScene()
{
	if (LinkedPortal.IsValid() == false)
	{
		return;
	}

	// 플레이어 카메라의 World 위치, 회전 값 얻기
	FVector PlayerCameraWorldLocation;
	FRotator PlayerCameraWorldRotation;
	PlayerController->GetPlayerViewPoint(PlayerCameraWorldLocation, PlayerCameraWorldRotation);

	// 포탈의 위치 정보 얻기
	FTransform ThisPortalTransform = GetActorTransform();

	// 연결된 포탈의 위치 정보 얻기
	FTransform LinkedPortalTransform = LinkedPortal->GetActorTransform();

#pragma region Location

	// Flip
	FTransform FlipedTransform = FTransform(ThisPortalTransform.GetRotation(), ThisPortalTransform.GetLocation(), ThisPortalTransform.GetScale3D() * FVector(-1, -1, 1));

	// 포탈 기준 상대 좌표 계산
	FVector PlayerCameraRelativeLocation = FlipedTransform.InverseTransformPosition(PlayerCameraWorldLocation);

	// 연결된 포탈의 SceneCapture Component의 World 위치 계산
	FVector NewComponentWorldLocation = LinkedPortalTransform.TransformPosition(PlayerCameraRelativeLocation);

#pragma endregion

#pragma region Rotation

	// 플레이어 카메라의 회전값을 각각의 축으로 분리
	FVector AxisX, AxisY, AxisZ;
	FRotationMatrix(PlayerCameraWorldRotation).GetScaledAxes(AxisX, AxisY, AxisZ);

	AxisX = ThisPortalTransform.InverseTransformVectorNoScale(AxisX);
	AxisX = FMath::GetReflectionVector(AxisX, FVector(1, 0, 0));
	AxisX = FMath::GetReflectionVector(AxisX, FVector(0, 1, 0));
	AxisX = LinkedPortalTransform.TransformVectorNoScale(AxisX);

	AxisY = ThisPortalTransform.InverseTransformVectorNoScale(AxisY);
	AxisY = FMath::GetReflectionVector(AxisY, FVector(1, 0, 0));
	AxisY = FMath::GetReflectionVector(AxisY, FVector(0, 1, 0));
	AxisY = LinkedPortalTransform.TransformVectorNoScale(AxisY);

	AxisZ = ThisPortalTransform.InverseTransformVectorNoScale(AxisZ);
	AxisZ = FMath::GetReflectionVector(AxisZ, FVector(1, 0, 0));
	AxisZ = FMath::GetReflectionVector(AxisZ, FVector(0, 1, 0));
	AxisZ = LinkedPortalTransform.TransformVectorNoScale(AxisZ);

	FMatrix RotationMatrix = FMatrix(AxisX, AxisY, AxisZ, FVector::ZeroVector);
	FRotator NewComponentWorldRotation = RotationMatrix.Rotator();

#pragma endregion

	// 연결된 포탈의 SceneCapture Component의 위치값 변경
	LinkedPortal->SceneCaptureComponent->SetWorldLocationAndRotation(NewComponentWorldLocation, NewComponentWorldRotation);
}

void APortalActor::CheckRenderTextureTargetSize()
{
	if (IsValid(RenderTextureTarget) == false)
	{
		return;
	}

	// 현재 플레이어의 Viewport 사이즈 얻기
	int32 ViewportWidth, ViewportHeight;
	PlayerController->GetViewportSize(ViewportWidth, ViewportHeight);

	// 설정된 Render Texture의 사이즈와 Viewport 사이즈가 다르면 Render Texture 사이즈 재설정
	if (ViewportWidth != RenderTextureTarget->SizeX || ViewportHeight != RenderTextureTarget->SizeY)
	{
		RenderTextureTarget->ResizeTarget(ViewportWidth, ViewportHeight);
	}
}

void APortalActor::IsActorCrossingPortal(AActor* Actor)
{
}

void APortalActor::OnCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}
