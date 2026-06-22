// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTags.h"
#include "ChatMessage.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct CHATTING_API FChatMessage
{
	GENERATED_BODY()

public:
	FChatMessage() : ChannelTag(FGameplayTag::EmptyTag), SenderName(TEXT("")), ReceiverName(TEXT("")), Message(TEXT("")) {}

	FChatMessage(const FGameplayTag& ChannelTag, const FString& SenderName, const FString& ReceiverName, const FString& Message)
		: ChannelTag(ChannelTag), SenderName(SenderName), ReceiverName(ReceiverName), Message(Message)
	{
	}
	
public:
	// 채팅 채널(그룹)
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	FGameplayTag ChannelTag;

	// 전송자 이름 (PlayerState::PlayerName)
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	FString SenderName;

	// 전송자 이름 (PlayerState::PlayerName)
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	FString ReceiverName;

	// 채팅 내용
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	FString Message;
};
