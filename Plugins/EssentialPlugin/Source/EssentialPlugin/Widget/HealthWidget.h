// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WidgetInitializeUtilityInterface.h"
#include "HealthWidget.generated.h"

/**
 * 
 */
UCLASS()
class ESSENTIALPLUGIN_API UHealthWidget : public UUserWidget, public IWidgetInitializeUtilityInterface
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
#pragma region Initial Settings On Canvas Panel Slot

	// 초기 Anchor
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Initial Settings")
	FAnchors InitialAnchor;

	// 초기 Alignment
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Initial Settings")
	FVector2D InitialAlignment;

	// 초기 Position
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Initial Settings")
	FVector2D InitialPosition;

#pragma endregion

protected:
	void InitializeWidgetByContextObject_Implementation(UObject* ContextObject) override;

	void InitializeWidgetByComponent_Implementation(UActorComponent* Component) override;

	void InitializeWidgetByActor_Implementation(AActor* Actor) override;

};
