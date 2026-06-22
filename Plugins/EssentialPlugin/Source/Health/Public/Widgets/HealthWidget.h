// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widgets/WidgetInitializeUtilityInterface.h"
#include "HealthWidget.generated.h"

// UChatBoxWidget 클래스에서 사용할 UE_LOG 카테고리 선언 및 정의
DECLARE_LOG_CATEGORY_EXTERN(HealthWidget, Log, All);

/**
 * 
 */
UCLASS()
class HEALTH_API UHealthWidget : public UUserWidget, public IWidgetInitializeUtilityInterface
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
