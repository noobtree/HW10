// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/ChattingComponent.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "EngineUtils.h"
#include "Managers/LocalWidgetManager.h"
#include "Widgets/ChatBoxWidget.h"
#include "ChatMessage.h"
#include "LogUtility.h"

namespace ChatChannels
{
	// 시스템 공지
	CHATTING_API UE_DEFINE_GAMEPLAY_TAG_COMMENT(ChatChannel_System, "Chat.System", "System Announcement");

	// 전체 채팅
	CHATTING_API UE_DEFINE_GAMEPLAY_TAG_COMMENT(ChatChannel_Global, "Chat.Global", "Global Chatting");

	// 팀 채팅
	CHATTING_API UE_DEFINE_GAMEPLAY_TAG_COMMENT(ChatChannel_Team, "Chat.Team", "Team Chatting");

	// 관전자 채팅
	CHATTING_API UE_DEFINE_GAMEPLAY_TAG_COMMENT(ChatChannel_Spectator, "Chat.Spectator", "Spectator Chatting");
}

// UChattingComponent 클래스에서 사용할 UE_LOG 카테고리 선언 및 정의
DEFINE_LOG_CATEGORY_STATIC(ChattingComponent, Log, All);

// Sets default values for this component's properties
UChattingComponent::UChattingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
	SetIsReplicatedByDefault(true);
}

// Called when the game starts
void UChattingComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	OwnerController = Cast<APlayerController>(GetOwner());
	if (IsValid(OwnerController) == false)
	{
		UE_LOG(ChattingComponent, Warning, TEXT("# %s - Component was attached to Invalid Type Owner"), FUNCTION_SIG);
		DestroyComponent();
		return;
	}
}

void UChattingComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, TeamID);
	DOREPLIFETIME(ThisClass, ListenableChannelTags);
}

void UChattingComponent::InitWidget()
{
	// Server 및 Other Client 환경에서의 실행 방지
	if (IsValid(GetOwner()) == false || GetOwner()->HasAuthority() == true || GetOwner()->HasLocalNetOwner() == false)
	{
		UE_LOG(ChattingComponent, Warning, TEXT("# %s - This Function can be operated in Local Client only"), FUNCTION_SIG);
		return;
	}

	// LocalWidgetManager 얻기
	ULocalWidgetManager* WidgetManager = ULocalWidgetManager::GetInstance(this);
	if (IsValid(WidgetManager) == false)
	{
		UE_LOG(ChattingComponent, Error, TEXT("# %s - Null Reference on LocalWidgetManager"), FUNCTION_SIG);
		return;
	}

	// 채팅 UI 생성
	UChatBoxWidget* ChatWidget = WidgetManager->AddWidget<UChatBoxWidget>(FName("ChatBox"), WidgetClass);
	if (ChatWidget == nullptr)
	{
		UE_LOG(ChattingComponent, Error, TEXT("# %s - Null Reference on created Widget by ULocalWidgetManager::AddWidget"), FUNCTION_SIG);
		return;
	}
	
	// 메시지 전송 이벤트 연결
	ChatWidget->OnChatMessageCommitted.AddDynamic(this, &UChattingComponent::ServerOnChatMessageCommitted);

	// 채팅 UI 초기 설정 진행
	IWidgetInitializeUtilityInterface::Execute_InitializeWidgetByComponent(ChatWidget, this);
	
	ChatWidget->AddToPlayerScreen();
}

void UChattingComponent::ServerOnChatMessageCommitted_Implementation(const FGameplayTag& ChannelTag, const FString& MessageString)
{
	// Client 환경에서의 실행 방지
	if (IsValid(OwnerController) == false || OwnerController->HasAuthority() == false)
	{
		UE_LOG(ChattingComponent, Warning, TEXT("# %s - This Function can be operated in Server only"), FUNCTION_SIG);
		return;
	}

	// 채팅 채널이 유효하지 않거나 메시지가 없으면 채팅 불가
	if (ChannelTag.IsValid() == false || MessageString.IsEmpty() == true)
	{
		UE_LOG(ChattingComponent, Display, TEXT("# %s - Invalid Chatting Channel or Empty message was committed"), FUNCTION_SIG);
		return;
	}

	// 채팅 메시지 구조체 생성
	FChatMessage ChatMessage;

	// 전송자의 이름 설정
	if (IsValid(OwnerController->PlayerState) == true)
	{
		ChatMessage.SenderName = OwnerController->PlayerState->GetPlayerName();
	}

	// 채팅 채널 설정
	ChatMessage.ChannelTag = ChannelTag;

	// 채팅 입력이 "/w"으로 시작하면 귓속말으로 처리
	if (MessageString.StartsWith(TEXT("/w"), ESearchCase::IgnoreCase) == true)
	{
		// "/w" 명령어를 제거한 문자열
		FString Buffer = MessageString.Mid(3).TrimStartAndEnd();

		// 남은 문자열에서 수신자 이름, 메시지 내용을 분리하여 저장
		Buffer.Split(TEXT(" "), &ChatMessage.ReceiverName, &ChatMessage.Message);
	}
	// 채팅 입력 전체를 메시지 내용으로 처리
	else
	{
		ChatMessage.Message = MessageString.TrimStartAndEnd();
	}

	// 공백 메시지 최종 확인
	if (ChatMessage.Message.IsEmpty() == true)
	{
		UE_LOG(ChattingComponent, Display, TEXT("# %s - Empty message was committed"), FUNCTION_SIG);
		return;
	}

	// (On Server) 각 Client에게 채팅 메시지 전송
	ServerSendChatMessage(ChatMessage);
}

void UChattingComponent::ServerSendChatMessage(const FChatMessage& ChatMessage)
{
	// Client 환경에서의 실행 방지
	if (IsValid(OwnerController) == false || OwnerController->HasAuthority() == false)
	{
		UE_LOG(ChattingComponent, Warning, TEXT("# %s - This Function can be operated in Server only"), FUNCTION_SIG);
		return;
	}

	// 전체 PlayerController 목록에서 수신자 선별
	for (TActorIterator<APlayerController> It(GetWorld()); It; ++It)
	{
		// PlayerController 유효성 확인
		APlayerController* Controller = *It;
		if (IsValid(Controller) == false)
		{
			UE_LOG(ChattingComponent, Warning, TEXT("# %s - Invalid PlayerController in World(%s)"), FUNCTION_SIG, *GetWorld()->GetMapName());
			continue;
		}

		// ChattingComponent 유효성 확인
		UChattingComponent* Component = Controller->GetComponentByClass<UChattingComponent>();
		if (IsValid(Component) == false)
		{
			UE_LOG(ChattingComponent, Display, TEXT("# %s - No ChattingComponent with PlayerController of Client"), FUNCTION_SIG);
			continue;
		}

		// 채팅 메시지의 지정 수신자가 존재하는지 확인
		if (ChatMessage.ReceiverName.IsEmpty() == false)
		{
			// 채팅 메시지의 지정 수신자와 동일한지 검사
			if (ChatMessage.ReceiverName == Controller->PlayerState->GetPlayerName())
			{
				// (Other Client) 채팅 메시지 수신 실행
				Component->ClientReceiveChatMessage(ChatMessage);

				// (Client) 채팅 메시지 수신 실행
				ClientReceiveChatMessage(ChatMessage);

				// 다른 클라이언트 검색 중지
				break;
			}

			// 다른 클라이언트 검색
			continue;
		}

		// 전송하는 채팅 채널을 수신하는지 확인
		if (Component->ListenableChannelTags.HasTag(ChatMessage.ChannelTag) == true)
		{
			// 채팅 채널이 ChatChannel_Team인 경우 TeamID가 다르면 메시지 미전송
			if (ChatMessage.ChannelTag.MatchesTag(ChatChannels::ChatChannel_Team) == true && Component->TeamID != this->TeamID)
			{
				continue;
			}

			// (Client) 채팅 메시지 수신 실행
			Component->ClientReceiveChatMessage(ChatMessage);
		}
	}
}

void UChattingComponent::ClientReceiveChatMessage_Implementation(const FChatMessage& ChatMessage)
{
	// Server 및 Other Client 환경에서의 실행 방지
	if (IsValid(OwnerController) == false || OwnerController->HasAuthority() == true)
	{
		UE_LOG(ChattingComponent, Warning, TEXT("# %s - This Function can be operated in Local Client only"), FUNCTION_SIG);
		return;
	}

	// 채팅 메시지 수신 이벤트 실행
	OnChatMessageReceived.Broadcast(ChatMessage);
}

