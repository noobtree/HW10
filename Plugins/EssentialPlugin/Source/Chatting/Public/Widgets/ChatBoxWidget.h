// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widgets/WidgetInitializeUtilityInterface.h"
#include "ChatBoxWidget.generated.h"

// EditableTextBox에서 Enter 키 입력을 통해 메시지 전송 시 실행되는 이벤트 대리자
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChatMessageCommittedSignature, const struct FGameplayTag&, ChannelTag, const FString&, MessageString);

/**
 * 
 */
UCLASS()
class CHATTING_API UChatBoxWidget : public UUserWidget, public IWidgetInitializeUtilityInterface
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

public:
	// 채팅 UI에서 메시지를 입력하고 전송하는 경우 실행되는 이벤트
	UPROPERTY(BlueprintAssignable)
	FOnChatMessageCommittedSignature OnChatMessageCommitted;
	
protected:
	// 채팅 로그를 관리하는 ScrollBox Widget
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<class UScrollBox> ChatLogScrollBox;

	// 채팅 메시지를 전송할 채널을 선택하는 ComboBox Widget
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<class UComboBoxKey> ChannelComboBox;

	// 채팅 메시지를 입력받는 MultiLineEditableText Widget
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<class UEditableTextBox> InputField;

	// 수신된 채팅 내역 텍스트의 스타일을 지정한 데이터 테이블
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UDataTable> LogTextStyleSet;

	// 초기 Anchor
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Initial Settings")
	FAnchors InitialAnchor;

	// 초기 Alignment
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Initial Settings")
	FVector2D InitialAlignment;

	// 초기 Position
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Initial Settings")
	FVector2D InitialPosition;

protected:
	void InitializeWidgetByContextObject_Implementation(UObject* ContextObject) override;

	void InitializeWidgetByComponent_Implementation(UActorComponent* Component) override;

	void InitializeWidgetByActor_Implementation(AActor* Actor) override;

	UFUNCTION(BlueprintCallable)
	virtual void OnChatMessageReceived(const struct FChatMessage& ChatMessage);

	UFUNCTION(BlueprintCallable)
	virtual void OnMultiLineEditableTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

};
