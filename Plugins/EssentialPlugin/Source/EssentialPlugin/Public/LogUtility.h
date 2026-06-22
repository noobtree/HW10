// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "UObject/NoExportTypes.h"
//#include "LogUtility.generated.h"

#define FUNCTION_SIG (ANSI_TO_TCHAR(__FUNCSIG__))
#define FUNCTION_TCHAR (ANSI_TO_TCHAR(__FUNCTION__))

#define NETMODE_TCHAR ((GetNetMode() == ENetMode::NM_Client) ? *FString::Printf(TEXT("Client%02d"), UE::GetPlayInEditorID()) : ((GetNetMode() == ENetMode::NM_Standalone) ? TEXT("StandAlone") : TEXT("Server")))

///**
// * 
// */
//UCLASS()
//class ESSENTIALPLUGIN_API ULogUtility : public UObject
//{
//	GENERATED_BODY()
//	
//public:
//};
