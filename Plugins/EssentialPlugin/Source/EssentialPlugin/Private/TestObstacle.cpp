// Fill out your copyright notice in the Description page of Project Settings.


#include "TestObstacle.h"

// Sets default values
ATestObstacle::ATestObstacle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(FName("DefaultSceneRoot"));
	SetRootComponent(DefaultSceneRoot);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("StaticMeshComponent"));
	Mesh->SetupAttachment(DefaultSceneRoot);
	
	Mesh->SetSimulatePhysics(true);
	UStaticMesh* DefaultMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/EssentialPlugin/Geometries/SM_TestObstacle.SM_TestObstacle"));
	if(DefaultMesh != nullptr)
	{
		Mesh->SetStaticMesh(DefaultMesh);
	}
}

// Called when the game starts or when spawned
void ATestObstacle::BeginPlay()
{
	Super::BeginPlay();
	
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Black, TEXT("Test Obstacle was spawned!"));
}

