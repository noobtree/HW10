// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "Managers/LocalWidgetManager.h"
#include "Widget/WidgetInitializeUtilityInterface.h"

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
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Orange, TEXT("UHealthComponent::SetMaxHealth Function can not be operated on Client"));
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
		return;
	}

	// Server 실행 방지
	if (Owner->HasAuthority() == true)
	{
		UE_LOG(LogTemp, Warning, TEXT("! UHealthComponent::InitWidget Function can not be operated on Server"));
		return;
	}

	// Local Client의 경우
	if (Owner->HasLocalNetOwner() == true)
	{
		UE_LOG(LogTemp, Display, TEXT("# Local Client UHealthComponent::InitWidget Function - Start"));

		// WidgetManager 얻기
		ULocalWidgetManager* WidgetManager = ULocalWidgetManager::GetInstance(this);
		if (IsValid(WidgetManager) == false)
		{
			return;
		}

		// Widget 생성
		UUserWidget* HealthWidget = WidgetManager->AddWidget(FName("Health"), LocalWidgetClass);
		if (HealthWidget == nullptr)
		{
			return;
		}

		HealthWidget->AddToViewport();
		IWidgetInitializeUtilityInterface::Execute_InitializeWidgetByComponent(HealthWidget, this);

		UE_LOG(LogTemp, Display, TEXT("# Local Client UHealthComponent::InitWidget Function - End"));
		return;
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("# Other Client UHealthComponent::InitWidget Function - Start"));

		// WidgetManager 얻기
		ULocalWidgetManager* WidgetManager = ULocalWidgetManager::GetInstance(this);
		if (IsValid(WidgetManager) == false)
		{
			return;
		}

		// Widget 생성
		UUserWidget* HealthWidget = WidgetManager->AddWidget(FName("Health"), RemoteWidgetClass);
		if (HealthWidget == nullptr)
		{
			return;
		}

		HealthWidget->AddToViewport();
		IWidgetInitializeUtilityInterface::Execute_InitializeWidgetByComponent(HealthWidget, this);

		UE_LOG(LogTemp, Display, TEXT("# Other Client UHealthComponent::InitWidget Function - End"));
		return;
	}
}

void UHealthComponent::OnTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	// Client 실행 방지
	if (IsValid(GetOwner()) == false || GetOwner()->HasAuthority() == false)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Orange, TEXT("UHealthComponent::OnTakeAnyDamage Function can not be operated on Client"));
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
	UE_LOG(LogTemp, Display, TEXT("UHealthComponent::CurrentHealth Property changed to %f"), CurrentHealth);

	// 체력 변화 이벤트 실행
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void UHealthComponent::OnRep_MaxHealth()
{
	UE_LOG(LogTemp, Display, TEXT("UHealthComponent::MaxHealth Property changed to %f"), MaxHealth);

	// 체력 변화 이벤트 실행
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

