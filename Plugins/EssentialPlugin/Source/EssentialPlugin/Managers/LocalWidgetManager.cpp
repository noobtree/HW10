// Fill out your copyright notice in the Description page of Project Settings.


#include "LocalWidgetManager.h"

ULocalWidgetManager* ULocalWidgetManager::GetInstance(const UObject* WorldContextObject)
{
	// 객체 유효성 확인
	if (IsValid(WorldContextObject) == false)
	{
		return nullptr;
	}

	// 월드 유효성 확인
	UWorld* World = WorldContextObject->GetWorld();
	if (IsValid(World) == false)
	{
		return nullptr;
	}

	// 월드에 존재하는 첫번째 플레이어 컨트롤러 얻기
	APlayerController* Controller = WorldContextObject->GetWorld()->GetFirstPlayerController();
	if (IsValid(Controller) == false)
	{
		return nullptr;
	}

	// 로컬 플레이어 객체 얻기
	ULocalPlayer* LocalPlayer = Controller->GetLocalPlayer();

	// SubSystem 얻기
	return LocalPlayer == nullptr ? nullptr : LocalPlayer->GetSubsystem<ULocalWidgetManager>();
}

UUserWidget* ULocalWidgetManager::FindWidget(const FName& WidgetName)
{
	//if (WidgetName.IsNone() == false && WidgetMap.Contains(WidgetName) == true)
	//{
	//	return WidgetMap[WidgetName];
	//}
	//return nullptr;

	if (WidgetName.IsNone() == true)
	{
		return nullptr;
	}

	// Key를 통해 WidgetInstance 찾기
	UUserWidget* WidgetInstance = WidgetMap.FindRef(WidgetName);
	return WidgetInstance;
}

UUserWidget* ULocalWidgetManager::AddWidget(const FName& WidgetName, const TSubclassOf<UUserWidget>& WidgetClass)
{
	// 추가하려는 Widget의 이름 및 Class 유효성 확인
	if (WidgetName.IsNone() == true || IsValid(WidgetClass) == false)
	{
		return nullptr;
	}

	// 동일한 키로 저장된 Widget Instance 얻기
	UUserWidget* WidgetInstance = FindWidget(WidgetName);
	if (IsValid(WidgetInstance) == true)
	{
		// 생성하려는 클래스로부터 파생된 Widget 여부 확인
		if (WidgetInstance->IsA(WidgetClass) == true)
		{
			// 기존 Widget Instance 반환
			return WidgetInstance;
		}

		// Widget 생성 및 등록 실패
		return nullptr;
	}
	// 새로운 Widget Instance 생성 및 저장

	// 로컬 플레이어 객체 얻기
	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (IsValid(LocalPlayer) == false)
	{
		return nullptr;
	}

	// Instance 생성
	WidgetInstance = CreateWidget(LocalPlayer->PlayerController, WidgetClass);

	// Instance 저장
	WidgetMap[WidgetName] = WidgetInstance;

	// Instance 반환
	return WidgetInstance;
}

bool ULocalWidgetManager::AddWidgetInstance(const FName& WidgetName, UUserWidget* WidgetInstance)
{
	// 추가하려는 Widget의 이름 및 Class 유효성 확인
	if (WidgetName.IsNone() == true || IsValid(WidgetInstance) == false)
	{
		return false;
	}

	// 동일한 키로 저장된 Widget Instance 얻기
	UUserWidget* ExistedInstance = FindWidget(WidgetName);
	if (IsValid(ExistedInstance) == true)
	{
		// 저장하려는 Widget Instance와 동일한 Instance 여부 확인
		return ExistedInstance == WidgetInstance;
	}

	// 새로운 Widget Instance 저장
	WidgetMap[WidgetName] = WidgetInstance;
	return true;
}

bool ULocalWidgetManager::RemoveWidget(const FName& WidgetName)
{
	// 제거하려는 Widget의 이름 유효성 확인
	if (WidgetName.IsNone() == true)
	{
		return false;
	}

	// 동일한 Key로 등록된 Widget Instance 얻기
	UUserWidget* WidgetInstance = WidgetMap.FindAndRemoveChecked(WidgetName);
	if (IsValid(WidgetInstance) == true)
	{
		// Instance 삭제
		WidgetInstance->RemoveFromParent();
	}
	return true;
}

void ULocalWidgetManager::SetWidgetHiddenInGame(const FName& WidgetName, bool bNewHidden)
{
	// Widget의 이름 유효성 확인
	if (WidgetName.IsNone() == true)
	{
		return;
	}

	// 동일한 Key로 등록된 Widget Instance 얻기
	UUserWidget* WidgetInstance = WidgetMap.FindAndRemoveChecked(WidgetName);
	if (IsValid(WidgetInstance) == true)
	{
		// Visibility 변경
		WidgetInstance->SetVisibility(bNewHidden == true ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
	}
	return;
}

void ULocalWidgetManager::ClearWidgetInGame()
{
	// 저장된 Widget Instance 제거
	for (const auto& pair : WidgetMap)
	{
		pair.Value->RemoveFromParent();
	}
	
	// 저장된 Widget 목록 초기화
	WidgetMap.Empty();
	return;
}
