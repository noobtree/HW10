// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

// 체력이 변경되었을 때 실행되는 이벤트 타입
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChangedSignature, float, CurrentHealth, float, MaxHealth);
// 사망하였을 때 실행되는 이벤트 타입
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnDeathSignature, AActor*, KilledActor, class AController*, InstigatedBy, AActor*, KillCauser);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ESSENTIALPLUGIN_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Called for Replication (MultiPlay)
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

#pragma region Delegate

public:
	// 체력이 변경되었을 때 호출되는 이벤트 Broadcast(float CurrentHealth, float MaxHealth)
	// 서버와 클라이언트에서 호출됨
	UPROPERTY(BlueprintAssignable)
	FOnHealthChangedSignature OnHealthChanged;

	// 사망하였을 때 실행되는 호출되는 이벤트 Broadcast(AActor* KilledActor, AController* InstigatedBy, AActor* KillCauser)
	// 서버에서만 호출됨
	UPROPERTY(BlueprintAssignable)
	FOnDeathSignature OnDeath;

#pragma endregion

#pragma region Variables

protected:
	// 최대 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_MaxHealth)
	float MaxHealth;

	// 현재 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_CurrentHealth)
	float CurrentHealth;

	// Local Client에서 표시될 체력 Widget
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UUserWidget> LocalWidgetClass;

	// Other Client에서 표시될 체력 Widget
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UUserWidget> RemoteWidgetClass;

#pragma endregion

#pragma region Functions

public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsAlive() const { return CurrentHealth > 0; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }

	// 최대 체력을 업데이트하는 함수
	UFUNCTION(BlueprintCallable)
	void SetMaxHealth(float NewMaxHealth);

	// Component에서 사용하는 Widget을 생성 및 초기화하는 함수
	UFUNCTION(BlueprintCallable)
	virtual void InitWidget();

protected:
	// DamagedActor : 데미지를 받은 객체
	// Damage : 데미지 값
	// DamageType : 데미지 유형
	// InstigatedBy : 데미지를 가하는 주체(가해자)
	// DamageCauser : 데미지 유발 객체(흉기)
	virtual void OnTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	// (Client) 현재 체력이 변경되었을 때 호출되는 함수
	UFUNCTION()
	virtual void OnRep_CurrentHealth();
	
	// (Client) 최대 체력이 변경되었을 때 호출되는 함수
	UFUNCTION()
	virtual void OnRep_MaxHealth();

#pragma endregion

};
