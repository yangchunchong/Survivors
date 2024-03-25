// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivorGameplayStatics.h"
#include "GameFramework/PlayerState.h"
#include "SurvivorGameMode.h"

DEFINE_LOG_CATEGORY(SurvivorGameplayStaticsLog);

void USurvivorGameplayStatics::OpenSteamURL(FString URL)
{
	IOnlineSubsystem* myOnlineSubsystem = IOnlineSubsystem::Get();
	//检查是否有效
	if (myOnlineSubsystem)
	{
		IOnlineExternalUIPtr myOnlineExternalUIInterface = myOnlineSubsystem->GetExternalUIInterface();
		if (myOnlineExternalUIInterface)
		{
			FShowWebUrlParams ShowParams;
			myOnlineExternalUIInterface->ShowWebURL(URL, ShowParams);
		}
	}
}
bool USurvivorGameplayStatics::GetIsDemo()
{
	bool IsDemo = false;
	GConfig->GetBool(TEXT("/Game/GameSetting"), TEXT("IsDemo"), IsDemo, GGameIni);
	return IsDemo;
}

bool USurvivorGameplayStatics::IsServer()
{
	return GIsServer;
}

bool USurvivorGameplayStatics::IsEditor()
{
	return GIsEditor;
}

void USurvivorGameplayStatics::WriteSteamLeaderboard(class APlayerController* PlayerController, const FString LeaderboardName, const int32 nScore)
{
	IOnlineSubsystem* myOnlineSubsystem = IOnlineSubsystem::Get();
	//检查是否有效
	if (myOnlineSubsystem)
	{
		IOnlineIdentityPtr myOnlineIdentityInterface = myOnlineSubsystem->GetIdentityInterface();
		IOnlineLeaderboardsPtr myOnlineLeaderboardsInterface = myOnlineSubsystem->GetLeaderboardsInterface();
		if (myOnlineLeaderboardsInterface)
		{
			ASurvivorGameMode* GameMode = Cast<ASurvivorGameMode>(GetGameMode(PlayerController->GetWorld()));
			if (GameMode)
			{
				if(LeaderboardName != GameMode->GetCurrentLBName())
					GameMode->SetLeaderboardName(LeaderboardName);
				GameMode->SetLeaderboardScore(LeaderboardName, nScore);
			}
		}
	}
}

void USurvivorGameplayStatics::FlushSteamLeaderboard(class APlayerController* PlayerController, const FString LeaderboardName)
{
	IOnlineSubsystem* myOnlineSubsystem = IOnlineSubsystem::Get();
	//检查是否有效
	if (myOnlineSubsystem)
	{
		IOnlineIdentityPtr myOnlineIdentityInterface = myOnlineSubsystem->GetIdentityInterface();
		IOnlineLeaderboardsPtr myOnlineLeaderboardsInterface = myOnlineSubsystem->GetLeaderboardsInterface();
		if (myOnlineLeaderboardsInterface)
		{
			myOnlineLeaderboardsInterface->FlushLeaderboards(FName(LeaderboardName));
		}
	}
}

void USurvivorGameplayStatics::PrintSteamLeaderboard(APlayerController* PlayerController, const FString LeaderboardName)
{
	ASurvivorGameMode* GameMode = Cast<ASurvivorGameMode>(GetGameMode(PlayerController->GetWorld()));
	if (GameMode)
	{
		GameMode->LeaderboardFound(LeaderboardName);
		GameMode->GetLeaderboardScore(0, 10);
	}
	//GetLeaderboardName
	//FindOrCreateLeaderboard
}

void USurvivorGameplayStatics::PrintSteamLeaderboardController(APlayerController* PlayerController, FName StatName, int32 StatValue)
{
	FString name = TEXT("PrintSteamLeaderboardController Called!");
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, name);
	if (APlayerState* PlayerState = (PlayerController != NULL) ? ToRawPtr(PlayerController->PlayerState) : NULL)
	{
		FUniqueNetIdPtr UserId = PlayerState->GetUniqueId().GetUniqueNetId();
		if (UserId.IsValid())
		{
			if (IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::IsLoaded() ? IOnlineSubsystem::Get() : nullptr)
			{
				IOnlineLeaderboardsPtr Leaderboards = OnlineSub->GetLeaderboardsInterface();
				if (Leaderboards.IsValid())
				{
					FOnlineLeaderboardWrite WriteObject;
					WriteObject.LeaderboardNames.Add(StatName);
					WriteObject.RatedStat = StatName;
					WriteObject.DisplayFormat = ELeaderboardFormat::Number;
					WriteObject.SortMethod = ELeaderboardSort::Descending;
					WriteObject.UpdateMethod = ELeaderboardUpdateMethod::KeepBest;
					WriteObject.SetIntStat(StatName, StatValue);

					// the call will copy the user id and write object to its own memory
					bool bResult = Leaderboards->WriteLeaderboards(PlayerState->SessionName, *UserId, WriteObject);
					name = TEXT("PrintSteamLeaderboardController  WriteLeaderboards  Called!");
					GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, name);

					// Flush the leaderboard immediately for now
					bool bFlushResult = Leaderboards->FlushLeaderboards(PlayerState->SessionName);
					name = TEXT("PrintSteamLeaderboardController  FlushLeaderboards  Called!");
					GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, name);
				}
			}
		}
	}


}

void USurvivorGameplayStatics::FindSteamLeaderboard(class APlayerController* PlayerController, const FString LeaderboardName)
{
	IOnlineSubsystem* myOnlineSubsystem = IOnlineSubsystem::Get();
	//检查是否有效
	if (myOnlineSubsystem)
	{
		IOnlineIdentityPtr myOnlineIdentityInterface = myOnlineSubsystem->GetIdentityInterface();
		IOnlineLeaderboardsPtr myOnlineLeaderboardsInterface = myOnlineSubsystem->GetLeaderboardsInterface();
		if (myOnlineLeaderboardsInterface)
		{

			ASurvivorGameMode* GameMode = Cast<ASurvivorGameMode>(GetGameMode(PlayerController->GetWorld()));
			if (GameMode)
			{
				GameMode->LeaderboardFound_Implementation(LeaderboardName);
			}
		}
	}
}
