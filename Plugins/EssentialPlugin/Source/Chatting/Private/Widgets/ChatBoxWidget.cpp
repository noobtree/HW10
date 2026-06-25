// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ChatBoxWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/RichTextBlock.h"
#include "Components/EditableTextBox.h"
#include "Components/ComboBoxKey.h"
#include "Components/ScrollBox.h"
#include "GameplayTags.h"
#include "Components/ChattingComponent.h"
#include "ChatMessage.h"
#include "LogUtility.h"

// UChatBoxWidget 클래스에서 사용할 UE_LOG 카테고리 선언 및 정의
DEFINE_LOG_CATEGORY_STATIC(ChatBoxWidget, Log, All);

void UChatBoxWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (InputField->OnTextCommitted.IsAlreadyBound(this, &UChatBoxWidget::OnMultiLineEditableTextCommitted) == false)
	{
		InputField->OnTextCommitted.AddDynamic(this, &UChatBoxWidget::OnMultiLineEditableTextCommitted);
	}
}

void UChatBoxWidget::NativeDestruct()
{
	if (InputField->OnTextCommitted.IsAlreadyBound(this, &UChatBoxWidget::OnMultiLineEditableTextCommitted) == true)
	{
		InputField->OnTextCommitted.RemoveDynamic(this, &UChatBoxWidget::OnMultiLineEditableTextCommitted);
	}

	Super::NativeDestruct();
}

void UChatBoxWidget::InitializeWidgetByContextObject_Implementation(UObject* ContextObject)
{
}

void UChatBoxWidget::InitializeWidgetByComponent_Implementation(UActorComponent* Component)
{
	// 채팅 관리 컴포넌트 확인
	UChattingComponent* ChatComponent = Cast<UChattingComponent>(Component);
	if (IsValid(ChatComponent) == false)
	{
		return;
	}

	// 채팅 수신 이벤트 연결
	ChatComponent->OnChatMessageReceived.AddDynamic(this, &UChatBoxWidget::OnChatMessageReceived);
}

void UChatBoxWidget::InitializeWidgetByActor_Implementation(AActor* Actor)
{
}

void UChatBoxWidget::OnChatMessageReceived(const FChatMessage& ChatMessage)
{
	if (IsValid(WidgetTree) == false)
	{
		UE_LOG(ChatBoxWidget, Error, TEXT("# %s - Invalid WidgetTree"), FUNCTION_SIG);
		return;
	}

	URichTextBlock* ChatLogTextBlock = WidgetTree->ConstructWidget<URichTextBlock>();
	if (ChatLogTextBlock == nullptr)
	{
		UE_LOG(ChatBoxWidget, Error, TEXT("# %s - ConstructWiget Function occured error"), FUNCTION_SIG);
		return;
	}

	if (IsValid(LogTextStyleSet) == false)
	{
		UE_LOG(ChatBoxWidget, Warning, TEXT("# %s - Invalid DataTable Asset for RichTextBlock"), FUNCTION_SIG);
		return;
	}

	// RichTextBlock의 TextStyle 지정 및 ScrollBox에 추가
	ChatLogTextBlock->SetTextStyleSet(LogTextStyleSet);
	ChatLogScrollBox->AddChild(ChatLogTextBlock);
	
	// 수신된 메시지를 RichText 형식으로 변환 및 적용
	FString MessageRichText = FString::Printf(TEXT("[%s] <%s>%s</> : %s"), *FDateTime::Now().ToFormattedString(TEXT("%H:%M")), *ChatMessage.ChannelTag.ToString(), *ChatMessage.SenderName, *ChatMessage.Message);
	ChatLogTextBlock->SetText(FText::FromString(MessageRichText));
}

void UChatBoxWidget::OnMultiLineEditableTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	// Enter 키 입력을 통해서만 메시지 전송
	if (CommitMethod != ETextCommit::OnEnter)
	{
		UE_LOG(ChatBoxWidget, Display, TEXT("# %s - User commit message with %s"), FUNCTION_SIG, *UEnum::GetValueAsString(CommitMethod));
		return;
	}

	// PlayerController 얻기
	APlayerController* Controller = GetOwningPlayer();
	if (Controller == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, *FString::Printf(TEXT("# %s - Null Reference on Owning PlayerController"), FUNCTION_SIG));
		return;
	}

	// 채팅 채널 얻기
	FName ChannelKey = ChannelComboBox->GetSelectedOption();
	//FGameplayTag ChannelTag = FGameplayTag::RequestGameplayTag(ChannelKey);
	FGameplayTag ChannelTag = FGameplayTag::EmptyTag;

	// 입력 문자열 얻기
	FString Message = Text.ToString();

	// 입력 문자열 초기화
	InputField->SetText(FText::GetEmpty());

	// 채팅 메시지 전송 이벤트 실행
	OnChatMessageCommitted.Broadcast(ChannelTag, Message);
}
