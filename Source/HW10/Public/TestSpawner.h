// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TestSpawner.generated.h"

UCLASS()
class HW10_API ATestSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATestSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> DefaultSceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> Mesh;

	// 액터 스폰 시간 간격
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "1"))
	float SpawnInterval = 1;

	// CPP 코드로 액터 스폰 시 위치 지정을 위하여 참조하는 액터
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	AActor* SpawnLocationTargetActor_CPP;

	// Blueprint로 액터 스폰 시 위치 지정을 위하여 참조하는 액터
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	AActor* SpawnLocationTargetActor_BP;

private:
	// C++ 스폰 타이머
	UPROPERTY()
	FTimerHandle CPPSpawnTimer;

	// Blueprint 스폰 타이머
	UPROPERTY()
	FTimerHandle BPSpawnTimer;

protected:
	// CPP 코드에서 액터를 스폰하도록 정의하는 함수
	UFUNCTION()
	void SpawnTemplateActor_CPP();

	// Blueprint에서 액터를 스폰하도록 정의하는 함수
	UFUNCTION(BlueprintImplementableEvent)
	void SpawnTemplateActor_BP();
};
