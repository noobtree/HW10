// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/HealthWidget.h"
#include "Components/CanvasPanelSlot.h"

DEFINE_LOG_CATEGORY(HealthWidget);

void UHealthWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Canavs Slot으로 생성되었을 경우 초기 위치 설정
	UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Slot);
	if (IsValid(CanvasSlot) == true)
	{
		CanvasSlot->SetAnchors(InitialAnchor);
		CanvasSlot->SetAlignment(InitialAlignment);
		CanvasSlot->SetPosition(InitialPosition);
	}
}

void UHealthWidget::InitializeWidgetByContextObject_Implementation(UObject* ContextObject)
{
}

void UHealthWidget::InitializeWidgetByComponent_Implementation(UActorComponent* Component)
{
}

void UHealthWidget::InitializeWidgetByActor_Implementation(AActor* Actor)
{
}
