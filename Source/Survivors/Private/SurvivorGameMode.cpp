// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivorGameMode.h"
#include "SurvivorLeaderboard.h"
#include "ThirdParty/Steamworks/Steamv153/sdk/public/steam/steam_api.h"
#include "SurvivorGameplayStatics.h"
#include "GameFramework/PlayerState.h"


ASurvivorGameMode::ASurvivorGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	//set default values, if any
}


void ASurvivorGameMode::BeginPlay()
{
	Super::BeginPlay();
	if (SteamAPI_Init())
	{
		if (!g_SteamLeaderboards) 
		{
			g_SteamLeaderboards = new FSteamLeaderboards();
			g_SteamLeaderboards->InitSteamLeaderboard(this);
		}
		const char* name = "steam API working";
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, name);
	}
}

//manage the callbacks on the tick, not sure if there is another way
void ASurvivorGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (SteamAPI_Init()) 
	{
		//SteamAPI_RunCallbacks();
		if (g_SteamLeaderboards) 
		{
			if (!bLeaderboardFound)
			{
				bLeaderboardFound = g_SteamLeaderboards->bLeaderboardFound;
			}
			else if (bSetScore)
			{
				SetLBScore();
			}
			else if (bGetScore && !bSetScore)
			{
				GetLBScores();
			}
			if (g_SteamLeaderboards->bLeaderboardScoresFound)
			{
				LeaderboardScoresFound_Implementation(Scores);
			}
		}
	}
}

bool ASurvivorGameMode::IsLeaderboardFound(const FString& LeaderboardName)
{
	if(g_SteamLeaderboards)
		return g_SteamLeaderboards->IsLeaderboardFound(LeaderboardName);
	return false;
}

//Blueprint set leaderboard name
void ASurvivorGameMode::SetLeaderboardName(FString LeaderboardName)
{
	const char* name = "SetLeaderboardName Called";
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, name);
	if (SteamAPI_Init())
	{
		name = "SteamAPI_Init Called";
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, name);
		if (g_SteamLeaderboards)
		{
			LBName = LeaderboardName;
			const char* chaLBName = TCHAR_TO_ANSI(*LBName);
			g_SteamLeaderboards->FindLeaderboard(chaLBName);
		}
	}
}

//does not do anything
void ASurvivorGameMode::LeaderboardFound_Implementation(const FString& LeaderboardName)
{
	const char* name = "Leaderboard Found From Tick";
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Yellow, name);
	if (g_SteamLeaderboards)
	{
		g_SteamLeaderboards->FindLeaderboard(LeaderboardName);
	}
}



// you will want to adjust this depending on what scores you want to show in your game
void ASurvivorGameMode::LeaderboardScoresFound_Implementation(TArray<FScorePackage>& scores)
{
	if (SteamAPI_Init())
	{
		if (g_SteamLeaderboards)
		{
			FScorePackage ThisScore;
			TArray<FScorePackage> TempScores;
			TempScores.Init(ThisScore, NumberOfScores);
			for (int i = 0; i < NumberOfScores; i++)
			{
				LeaderboardEntry_t LBRow = g_SteamLeaderboards->m_leaderboardEntries[i];
				ThisScore.PlayerName = SteamFriends()->GetFriendPersonaName(LBRow.m_steamIDUser);
				if (ThisScore.PlayerName == "") 
				{ 
					//check if returning a null entry
					ThisScore.PlayerName = "---";
					ThisScore.Rank = -1; //just looks cleaner 
					ThisScore.Score = 0;
				}
				else 
				{
					ThisScore.Rank = LBRow.m_nGlobalRank;
					ThisScore.Score = LBRow.m_nScore;
				}

				TempScores[i] = ThisScore;
			}
			g_SteamLeaderboards->bLeaderboardScoresFound = false;
			LeaderboardScoresFound(TempScores);
		}
	}
}

FString ASurvivorGameMode::GetCurrentLBName()
{
	if (g_SteamLeaderboards)
		return g_SteamLeaderboards->GetCurrentLBName();
	return FString();
}

//Blueprint node sets the current player score
void ASurvivorGameMode::SetLeaderboardScore(const FString& LeaderboardName, int32 score)
{
	if (!g_SteamLeaderboards->bLeaderboardFound)
		g_SteamLeaderboards->FindLeaderboard(LeaderboardName);
	FString name = "SetLeaderboardScore Called ~~~~~~~~~~";
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, name);
	Score = score;
	bSetScore = true;
}

//Setting the player score
void ASurvivorGameMode::SetLBScore()
{
	FString name = TEXT("--------------------- SetLBScores Called");
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, name);
	if (SteamAPI_Init())
	{
		if (g_SteamLeaderboards) 
		{
			g_SteamLeaderboards->UploadScore(Score);
			bSetScore = false;
			APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
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
							name = FString::Printf(TEXT(" --------------- SessionName = %s"), *PlayerState->SessionName.ToString());
							GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, name);
						}
					}
				}
			}
		}
	}
}

//Blueprint node gets range of scores needed
void ASurvivorGameMode::GetLeaderboardScore(int32 scoreStartIndex, int32 numberOfScores)
{
	const char* name = "GetLeaderboardScore Called";
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, name);
	ScoreStartIndex = scoreStartIndex;
	NumberOfScores = numberOfScores;
	bGetScore = true;

	FString Name = FString::Printf(TEXT(" bLeaderboardFound = %d bSetScore = %d bSetScore = %d "), bLeaderboardFound, bSetScore, bSetScore);
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, Name);
}

//Getting the range of scores
void ASurvivorGameMode::GetLBScores()
{
	const char* name = "GetLBScores Called";
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, name);
	if (SteamAPI_Init())
	{
		if (g_SteamLeaderboards) 
		{
			g_SteamLeaderboards->DownloadUserScores(ScoreStartIndex, NumberOfScores);
			bGetScore = false;
		}
	}
}
