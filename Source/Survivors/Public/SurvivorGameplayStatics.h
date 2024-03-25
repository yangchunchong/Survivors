// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystem.h"

#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineExternalUIInterface.h"
#include "Interfaces/OnlineLeaderboardInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "SurvivorGameplayStatics.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(SurvivorGameplayStaticsLog, Log, All);
/**
 * 
 */
UCLASS()
class SURVIVORS_API USurvivorGameplayStatics : public UGameplayStatics
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "GameplayStatics")
	static void OpenSteamURL(FString URL);

	UFUNCTION(BlueprintPure, Category = "GameplayStatics")
	static bool GetIsDemo();
	UFUNCTION(BlueprintPure, Category = "GameplayStatics")
	static bool IsServer();
	UFUNCTION(BlueprintPure, Category = "GameplayStatics")
	static bool IsEditor();


	UFUNCTION(BlueprintCallable, Category = "GameplayStatics")
	static void FindSteamLeaderboard(class APlayerController* PlayerController, const FString LeaderboardName);

	UFUNCTION(BlueprintCallable, Category = "GameplayStatics")
	static void WriteSteamLeaderboard(class APlayerController* PlayerController, const FString LeaderboardName, const int32 nScore);

	UFUNCTION(BlueprintCallable, Category = "GameplayStatics")
	static void FlushSteamLeaderboard(class APlayerController* PlayerController, const FString LeaderboardName);

	UFUNCTION(BlueprintCallable, Category = "GameplayStatics")
	static void PrintSteamLeaderboard(class APlayerController* PlayerController, const FString LeaderboardName);

	UFUNCTION(BlueprintCallable, Category = "GameplayStatics")
	static void PrintSteamLeaderboardController(class APlayerController* PlayerController, FName StatName, int32 StatValue);

};
