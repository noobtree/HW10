// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ClientState.generated.h"

/**
 * 
 */
UCLASS()
class HW10_API AClientState : public APlayerState
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
};
