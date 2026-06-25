// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/HealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/GameState.h"
#include "Managers/LocalWidgetManager.h"
#include "Widgets/WidgetInitializeUtilityInterface.h"
#include "LogUtility.h"

// UHealthComponent 클래스에서 사용할 UE_LOG 카테고리 선언 및 정의
DEFINE_LOG_CATEGORY_STATIC(HealthComponent, Log, All);

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	SetIsReplicatedByDefault(true);
}

// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	// Server 환경에서 TakeDamage 이벤트 연결
	if (IsValid(GetOwner()) == true && GetOwner()->HasAuthority() == true)
	{
		GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::OnTakeAnyDamage);
	}
}

// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, CurrentHealth);
	DOREPLIFETIME(ThisClass, MaxHealth);
}

void UHealthComponent::SetMaxHealth(float NewMaxHealth)
{
	// Client 실행 방지
	if (IsValid(GetOwner()) == false || GetOwner()->HasAuthority() == false)
	{
		UE_LOG(HealthComponent, Warning, TEXT("# %s - This Function can be operated in Server only"), FUNCTION_SIG);
		return;
	}

	// 최대 체력 변경
	MaxHealth = NewMaxHealth < KINDA_SMALL_NUMBER ? 1 : NewMaxHealth;
	
	// 현재 체력 조정
	CurrentHealth = FMath::Clamp(CurrentHealth, 0, MaxHealth);

	// (서버) 체력 변화 이벤트 실행
	OnRep_MaxHealth();
}

void UHealthComponent::InitWidget()
{
	AActor* Owner = GetOwner();
	if (IsValid(Owner) == false)
	{
		UE_LOG(HealthComponent, Warning, TEXT("# %s - Invalid Component Owner"), FUNCTION_SIG);
		return;
	}

	// Server 실행 방지
	if (Owner->HasAuthority() == true)
	{
		UE_LOG(HealthComponent, Warning, TEXT("# %s - This Function can be operated in Client only"), FUNCTION_SIG);
		return;
	}

	// Local Client의 경우
	if (Owner->HasLocalNetOwner() == true)
	{
		UE_LOG(HealthComponent, Display, TEXT("# %s - Local Client"), FUNCTION_SIG);

		// WidgetManager 얻기
		ULocalWidgetManager* WidgetManager = ULocalWidgetManager::GetInstance(this);
		if (IsValid(WidgetManager) == false)
		{
			return;
		}

		// Widget 생성
		UUserWidget* WidgetInstance = WidgetManager->AddWidget(FName("Health"), LocalWidgetClass);
		if (WidgetInstance == nullptr)
		{
			return;
		}

		WidgetInstance->AddToViewport();
		IWidgetInitializeUtilityInterface::Execute_InitializeWidgetByComponent(WidgetInstance, this);
		return;
	}
	// Other Client의 경우
	else
	{
		UE_LOG(HealthComponent, Display, TEXT("# %s - Other Client"), FUNCTION_SIG);

		// WidgetManager 얻기
		ULocalWidgetManager* WidgetManager = ULocalWidgetManager::GetInstance(this);
		if (IsValid(WidgetManager) == false)
		{
			return;
		}

		// Widget 생성
		UUserWidget* WidgetInstance = WidgetManager->AddWidget(FName("Health"), RemoteWidgetClass);
		if (WidgetInstance == nullptr)
		{
			return;
		}

		WidgetInstance->AddToViewport();
		IWidgetInitializeUtilityInterface::Execute_InitializeWidgetByComponent(WidgetInstance, this);
		return;
	}
}

void UHealthComponent::OnTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	// Client 실행 방지
	if (IsValid(GetOwner()) == false || GetOwner()->HasAuthority() == false)
	{
		UE_LOG(HealthComponent, Warning, TEXT("# %s - This Function can be operated in Server only"), FUNCTION_SIG);
		return;
	}

	// 체력 변화 적용
	CurrentHealth -= Damage;

	// 체력 수치 제한 적용
	CurrentHealth = FMath::Clamp(CurrentHealth, 0, MaxHealth);

	// 체력 값이 매우 작을 경우 0으로 설정
	if (CurrentHealth < KINDA_SMALL_NUMBER)
	{
		CurrentHealth = 0;

		// 사망 이벤트 실행
		OnDeath.Broadcast(DamagedActor, InstigatedBy, DamageCauser);
	}

	// (서버) 체력 변화 이벤트 실행
	OnRep_CurrentHealth();
}

void UHealthComponent::OnRep_CurrentHealth()
{
	UE_LOG(HealthComponent, Display, TEXT("# %s - CurrentHealth was replicated"), FUNCTION_SIG);

	// 체력 변화 이벤트 실행
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void UHealthComponent::OnRep_MaxHealth()
{
	UE_LOG(HealthComponent, Display, TEXT("# %s - MaxHealth was replicated"), FUNCTION_SIG);

	// 체력 변화 이벤트 실행
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

