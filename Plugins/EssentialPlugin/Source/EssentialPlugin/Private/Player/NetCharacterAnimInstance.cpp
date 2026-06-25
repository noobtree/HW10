// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/NetCharacterAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UNetCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	CharacterOwner = Cast<ACharacter>(GetOwningActor());
	if (IsValid(CharacterOwner) == false)
	{
		EnableUpdateAnimation(false);
		return;
	}

	OwnerMovementComponent = CharacterOwner->GetCharacterMovement();
}

void UNetCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	Velocity = CharacterOwner->GetVelocity();
	bIsAccelerated = (IsValid(OwnerMovementComponent) == true ? OwnerMovementComponent->GetCurrentAcceleration().Length() : Velocity.Size2D()) > 0;
	bIsFalling = IsValid(OwnerMovementComponent) == true ? OwnerMovementComponent->IsFalling() : Velocity.Z < 0;

	
}
