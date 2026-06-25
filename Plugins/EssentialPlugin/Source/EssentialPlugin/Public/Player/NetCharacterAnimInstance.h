// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "NetCharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ESSENTIALPLUGIN_API UNetCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

public:
	// AnimInstance가 적용된 USkeletonMeshComponent의 Owner 객체 (Character)
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ACharacter> CharacterOwner;

	// Owner의 MovementComponent Instance (CharacterMovementComponent)
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class UCharacterMovementComponent> OwnerMovementComponent;

	// 객체의 현재 이동 속도
	UPROPERTY(BlueprintReadOnly)
	FVector Velocity;

	// 객체의 정면 방향과 카메라의 정면 방향의 차이 (PitchDiff, YawDiff)
	UPROPERTY(BlueprintReadOnly)
	FVector2D AimOffsetRotation;

	// 객체의 가속도가 0보다 큰 상태를 나타내는 플래그
	UPROPERTY(BlueprintReadOnly)
	uint8 bIsAccelerated : 1;

	// 객체의 현재 추락 상태를 나타내는 플래그
	UPROPERTY(BlueprintReadOnly)
	uint8 bIsFalling : 1;
};
