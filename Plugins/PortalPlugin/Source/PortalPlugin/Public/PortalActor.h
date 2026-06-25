// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PortalActor.generated.h"

UCLASS()
class PORTALPLUGIN_API APortalActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APortalActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
#pragma region Components

	// 액터의 DefaultSceneRoot
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Portal|Components")
	TObjectPtr<USceneComponent> DefaultSceneRoot;

	// 포탈의 평면을 담당하는 MeshComponent
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Portal|Components")
	TObjectPtr<UStaticMeshComponent> PortalPlaneMeshComponent;

	// 포탈 접촉을 감지하는 Collision 담당 Component
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Portal|Components")
	TObjectPtr<UShapeComponent> CollisionShapeComponent;

	// 연결된 포탈의 평면에 비춰질 장면(Scene)을 캡쳐하는 Component
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Portal|Components")
	TObjectPtr<class USceneCaptureComponent2D> SceneCaptureComponent;

#pragma endregion

	// 포탈을 관측하는 플레이어의 컨트롤러
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Portal")
	TWeakObjectPtr<APlayerController> PlayerController;

	// 연결된 다른 포탈
	// 순환 참조 방지를 위한 WeakObjectPtr 사용
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Portal")
	TWeakObjectPtr<APortalActor> LinkedPortal;

	// 포탈 동작 시 액터의 진행 방향 제한 설정
	// True : Front(Local +X) -> Back(Local -X) 방향으로 제한
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal|Teleport")
	bool bFrontSideOnly = true;

	// 해당 포탈에서 사용될 Material Instance를 생성하기 위한 Master Material
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Portal|Render")
	TObjectPtr<UMaterialInterface> PortalMaterial;

	// PortalMaterial을 통해 생성된 해당 포탈에서 사용되는 Material Instance
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Transient, Category = "Portal|Render")
	TObjectPtr<UMaterialInstanceDynamic> PortalMaterialInstance;

	// 포탈 Material에서 정의한 Texture 파라미터 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal|Render")
	FName MaterialTextureParameterName = NAME_None;

	// SceneCaptureComponent에서 캡처된 Texture RenderTarget
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Transient, Category = "Portal|Render")
	TObjectPtr<UTextureRenderTarget2D> RenderTextureTarget;

	TMap<AActor*, FVector> LastPositions;
	TMap<AActor*, bool> LastIsInFront;

public:
	// 임의 다른 포탈에 연결되도록 설정하는 함수
	void SetLinkedPortal(APortalActor* OtherPortal);

protected:
	// 서로 다른 포탈에서 Material에 SceneCapture Component로 캡처된 Render Texture를 사용할 수 있도록 Material Instance 생성
	void CreatePortalMaterialInstance();

	// 연결된 포탈의 SceneCapture Component에서 캡처된 Scene을 담을 Render Texture 생성
	void CreatePortalRenderTextureTarget();

	// 현재 프레임의 플레이어 카메라 위치와 회전 정보를 이용하여 연결된 다른 포탈의 SceneCapture Component의 위치 및 회전값을 조정하는 함수
	void UpdatePortalCapturedScene();

	// 현재 Viewport 크기와 생성된 RenderTexture의 크기를 비교하여 조정하는 함수
	void CheckRenderTextureTargetSize();

	void IsActorCrossingPortal(AActor* Actor);
	
	UFUNCTION()
	void OnCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
