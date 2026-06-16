// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "LocalWidgetManager.generated.h"

/**
 * 
 */
UCLASS()
class ESSENTIALPLUGIN_API ULocalWidgetManager : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(BlueprintReadOnly)
	TMap<FName, TObjectPtr<UUserWidget>> WidgetMap;

public:
	UFUNCTION()
	static ULocalWidgetManager* GetInstance(const UObject* WorldContextObject);

	// 임의 이름으로 등록된 Widget Instance를 반환하는 함수
	UFUNCTION(BlueprintCallable)
	UUserWidget* FindWidget(const FName& WidgetName);

	template<typename TUserWidget = UUserWidget>
	TUserWidget* FindWidget(const FName& WidgetName)
	{
		static_assert(TIsDerivedFrom<TUserWidget, UUserWidget>::Value, "TUserWidget must be class derived from UUserWidget");

		UUserWidget* WidgetInstance = FindWidget(WidgetName);
		if (IsValid(WidgetInstance) == true)
		{
			return Cast<TUserWidget>(WidgetInstance);
		}
		return nullptr;
	}

	// 임의 클래스로 Widget Instance를 생성, 임의 이름으로 등록하는 함수
	// 동일한 이름으로 등록된 WidgetInstance가 해당 클래스에서 파생된 경우 기존 WidgetInstance 반환
	// Instance 생성 및 등록에 실패할 경우 nullptr 반환
	UFUNCTION(BlueprintCallable)
	UUserWidget* AddWidget(const FName& WidgetName, const TSubclassOf<UUserWidget>& WidgetClass);

	template<typename TUserWidget = UUserWidget>
	TUserWidget* AddWidget(const FName& WidgetName, const TSubclassOf<UUserWidget>& WidgetClass = TUserWidget::StaticClass())
	{
		static_assert(TIsDerivedFrom<TUserWidget, UUserWidget>::Value, "TUserWidget must be class derived from UUserWidget");

		UUserWidget* WidgetInstance = AddWidget(WidgetName, WidgetClass);
		if (IsValid(WidgetInstance) == true)
		{
			return Cast<TUserWidget>(WidgetInstance);
		}
		return nullptr;
	}

	// Widget Instance를 임의 이름으로 등록하는 함수
	// Instance 등록에 실패하는 경우 false 반환
	UFUNCTION(BlueprintCallable)
	bool AddWidgetInstance(const FName& WidgetName, UUserWidget* WidgetInstance);

	// 임의 이름으로 등록된 Widget Instance를 제거하는 함수
	UFUNCTION(BlueprintCallable)
	bool RemoveWidget(const FName& WidgetName);

	// 임의 이름으로 등록된 Widget Instance의 Visibility를 변경하는 함수
	UFUNCTION(BlueprintCallable)
	void SetWidgetHiddenInGame(const FName& WidgetName, bool bNewHidden);

	// WidgetManager에 등록된 모든 Widget Instance를 제거하는 함수
	UFUNCTION(BlueprintCallable)
	void ClearWidgetInGame();
};
