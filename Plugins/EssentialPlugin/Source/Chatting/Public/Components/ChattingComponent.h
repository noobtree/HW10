// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NativeGameplayTags.h"
#include "ChattingComponent.generated.h"

namespace ChatChannels
{
	// 시스템 공지
	CHATTING_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(ChatChannel_System);

	// 전체 채팅
	CHATTING_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(ChatChannel_Global);

	// 팀 채팅
	CHATTING_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(ChatChannel_Team);

	// 관전자 채팅
	CHATTING_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(ChatChannel_Spectator);
}

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChatMessageReceivedSignature, const struct FChatMessage&, ChatMessage);

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CHATTING_API UChattingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UChattingComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// 메시지 수신 시 실행되는 이벤트 (const struct FChatMessage& ChatMessage)
	UPROPERTY(BlueprintAssignable)
	FOnChatMessageReceivedSignature OnChatMessageReceived;

protected:
	// 컴포넌트 소유 액터(PlayerController)
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	APlayerController* OwnerController;

	// 채팅 UI 클래스
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UUserWidget> WidgetClass;

	// 플레이어가 수신하는 채팅 채널을 설정한 태그
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	FGameplayTagContainer ListenableChannelTags;

	// 플레이어의 동적 팀을 나타내는 ID
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Replicated)
	FGuid TeamID;

public:
	// Component에서 사용하는 Widget을 생성 및 초기화하는 함수
	UFUNCTION(BlueprintCallable)
	virtual void InitWidget();

protected:
	// (Server RPC) 채팅 메시지를 전송하려는 경우 호출되는 함수
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerOnChatMessageCommitted(const FGameplayTag& ChannelTag, const FString& MessageString);

	// 채팅 메시지를 설정된 수신자에게 전송하는 함수 (서버 전용)
	UFUNCTION(BlueprintCallable)
	void ServerSendChatMessage(const struct FChatMessage& ChatMessage);

	// (Client RPC) 수신된 채팅 메시지를 RichText로 바꾸어 UI에 출력하는 함수
	UFUNCTION(BlueprintCallable, Client, Reliable)
	void ClientReceiveChatMessage(const struct FChatMessage& ChatMessage);
};
