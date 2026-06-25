// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/HealthWidget_GTFO.h"
#include "Components/TextBlock.h"
#include "Components/SizeBox.h"
#include "Components/HealthComponent.h"
#include "LogUtility.h"

void UHealthWidget_GTFO::NativeConstruct()
{
	Super::NativeConstruct();

#pragma region 데미지 표시 Timeline 설정

	// Timeline의 Tick에 따라 호출되는 Delegate에 함수 연결
	FOnTimelineFloat timelineDelegate;
	timelineDelegate.BindDynamic(this, &UHealthWidget_GTFO::OnDamageBarShrinkTimelineUpdate);

	// Timeline에 Curve 및 Delegate 등록
	if (DamageBarShrinkCurve != nullptr)
	{
		UE_LOG(HealthWidget, Warning, TEXT("# %s - Invalid CurveFloat Asset"), FUNCTION_SIG);
		DamageBarShrinkTimeline.AddInterpFloat(DamageBarShrinkCurve, timelineDelegate);
	}

#pragma endregion

}

void UHealthWidget_GTFO::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	DamageBarShrinkTimeline.TickTimeline(InDeltaTime);
}

void UHealthWidget_GTFO::InitializeWidgetByComponent_Implementation(UActorComponent* Component)
{
	// 화면 해상도에 따른 체력바의 크기 설정
	HealthBarMaxWidth = GSystemResolution.ResX * 0.15;

	// HealthComponent 확인
	UHealthComponent* HealthComp = Cast<UHealthComponent>(Component);
	if (IsValid(HealthComp) == true)
	{
		// 현재 체력 상태에 맞추어 UI 업데이트
		OnHealthChanged(HealthComp->GetCurrentHealth(), HealthComp->GetMaxHealth());
	}
	else
	{
		//UE_LOG(HealthWidget, Warning, TEXT("# %s - Widget Has not Initialized with Health Component!"), FUNCTION_SIG);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, *FString::Printf(TEXT("# %s - Widget Has not Initialized with Health Component!"), FUNCTION_SIG));
		// 체력 0% 상태로 업데이트
		OnHealthChanged(0, 100);
	}

	// 데미지 표시의 크기를 체력바 크기와 동일하게 설정 (최초 체력바 UI 생성 시 피격되는 듯한 착시 제거)
	SizeBox_LeftDamageBar->SetWidthOverride(SizeBox_LeftHealthBar->GetWidthOverride());
	SizeBox_RightDamageBar->SetWidthOverride(SizeBox_RightHealthBar->GetWidthOverride());
}

void UHealthWidget_GTFO::OnHealthChanged(float CurrentHealth, float MaxHealth)
{
	float HealthRatio = CurrentHealth / MaxHealth;		// 실제 체력의 비율 계산
	float CriticalFactor = FMath::GetRangePct(0.2f, 1.f, HealthRatio);		// 0.2(20%) - 1(100%) 사이에서 체력 비율이 어느정도인지 계산

#pragma region 체력바 Size 조정

	// HealthBar Width
	float Width = FMath::Lerp(0, HealthBarMaxWidth, HealthRatio);
	SizeBox_LeftHealthBar->SetWidthOverride(Width);
	SizeBox_RightHealthBar->SetWidthOverride(Width);

	// HealthBar Height
	float Height = FMath::Lerp(4.f, 16.f, 1 - CriticalFactor);
	SizeBox_LeftHealthBar->SetHeightOverride(Height);
	SizeBox_RightHealthBar->SetHeightOverride(Height);

	// DamageBar Height
	SizeBox_LeftDamageBar->SetHeightOverride(Height);
	SizeBox_RightDamageBar->SetHeightOverride(Height);

#pragma endregion

#pragma region 체력바 Color 변경

	FLinearColor TextColor = FLinearColor(1, CriticalFactor, CriticalFactor, 1);
	TextBlock_HealthIndicator->SetColorAndOpacity(FSlateColor(TextColor));

#pragma endregion

#pragma region 데미지 표시 축소 예약/실행

	// 데미지 표시 축소 기능이 실행중인지 확인
	if (DamageBarShrinkTimeline.IsPlaying() == false)
	{
		// 데미지 표시 축소 기능을 실행할 예정이 존재하는지 확인
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		if (TimerManager.IsTimerActive(DamageBarShrinkDelayTimer) == false)
		{
			// 데미지 표시 축소 시작 예약
			FTimerDelegate timerDelegate = FTimerDelegate::CreateLambda([&]() { DamageBarShrinkTimeline.PlayFromStart(); });
			TimerManager.SetTimer(DamageBarShrinkDelayTimer, timerDelegate, 0.1f, false);
			return;
		}

		// 데미지 표시 축소 시작 대기
	}

	// 데미지 표시 축소 재시작
	DamageBarShrinkTimeline.PlayFromStart();

#pragma endregion

}

void UHealthWidget_GTFO::OnDamageBarShrinkTimelineUpdate(float CurveValue)
{
	// 데미지 표시의 새로운 Width 계산
	float NewWidth = FMath::Lerp(SizeBox_LeftDamageBar->GetWidthOverride(), SizeBox_LeftHealthBar->GetWidthOverride(), CurveValue);

	// 데미지 표시의 새로운 Width 적용
	SizeBox_LeftDamageBar->SetWidthOverride(NewWidth);
	SizeBox_RightDamageBar->SetWidthOverride(NewWidth);
}
