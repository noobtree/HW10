// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HealthWidget.h"
#include "Components/TimelineComponent.h"
#include "HealthWidget_GTFO.generated.h"

/**
 * 
 */
UCLASS()
class HEALTH_API UHealthWidget_GTFO : public UHealthWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	// 체력 % 안내 문구
	// Ex) 100% or 20%
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"));
	TObjectPtr<class UTextBlock> TextBlock_HealthIndicator;

	// 좌측 체력바의 크기를 제어하는 Widget
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"));
	TObjectPtr<class USizeBox> SizeBox_LeftHealthBar;

	// 좌측 데미지 표시의 크기를 제어하는 Widget
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"));
	TObjectPtr<class USizeBox> SizeBox_LeftDamageBar;

	// 우측 체력바의 크기를 제어하는 Widget
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"));
	TObjectPtr<class USizeBox> SizeBox_RightHealthBar;

	// 우측 데미지 표시의 크기를 제어하는 Widget
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"));
	TObjectPtr<class USizeBox> SizeBox_RightDamageBar;

	// 데미지 표시의 크기가 줄어드는 패턴을 정의하는 Curve Asset
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCurveFloat> DamageBarShrinkCurve;

	// 체력바의 최대 크기(Width)
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"));
	float HealthBarMaxWidth;

	// 데미지 표시의 크기가 줄어들기 시작할 때까지의 시간 지연을 제어하는 TimerHandle
	UPROPERTY()
	FTimerHandle DamageBarShrinkDelayTimer;

	// 데미지 표시의 크기가 줄어드는 패턴을 제어하는 Timeline
	UPROPERTY()
	FTimeline DamageBarShrinkTimeline;

protected:
	// 임의 컴포넌트에 맞추어 Widget의 초기 설정을 진행하는 함수
	virtual void InitializeWidgetByComponent_Implementation(UActorComponent* Component) override;

	// HealthComponent의 체력 변경 이벤트 발생 시 Widget에 연동하는 함수
	UFUNCTION()
	virtual void OnHealthChanged(float CurrentHealth, float MaxHealth);

	UFUNCTION()
	virtual void OnDamageBarShrinkTimelineUpdate(float CurveValue);
};
