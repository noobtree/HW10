// Fill out your copyright notice in the Description page of Project Settings.


#include "TestSpawner.h"
#include "TestObstacle.h"	// Build.cs의 -DependencyModuleNames에 해당 클래스가 작성된 모듈 추가 필요

// Sets default values
ATestSpawner::ATestSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(FName("DefaultSceneRoot"));
	SetRootComponent(DefaultSceneRoot);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("StaticMeshComponent"));
	Mesh->SetupAttachment(DefaultSceneRoot);
}

// Called when the game starts or when spawned
void ATestSpawner::BeginPlay()
{
	Super::BeginPlay();

	FTimerManager& WorldTimerManager = GetWorldTimerManager();
	WorldTimerManager.SetTimer(CPPSpawnTimer, this, &ATestSpawner::SpawnTemplateActor_CPP, SpawnInterval, true, 3);
	WorldTimerManager.SetTimer(BPSpawnTimer, this, &ATestSpawner::SpawnTemplateActor_BP, SpawnInterval, true, 3);
}

void ATestSpawner::SpawnTemplateActor_CPP()
{
	if (IsValid(SpawnLocationTargetActor_CPP) == false)
	{
		return;
	}

	FActorSpawnParameters spawnParams;
	spawnParams.Name = FName("Actor_SpawnedByCPP");
	spawnParams.NameMode = FActorSpawnParameters::ESpawnActorNameMode::Requested;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(ATestObstacle::StaticClass(), SpawnLocationTargetActor_CPP->GetActorLocation(), GetActorRotation());

	SpawnedActor->SetActorLabel(SpawnedActor->GetName());
}

