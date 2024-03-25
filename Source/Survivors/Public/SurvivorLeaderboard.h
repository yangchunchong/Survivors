// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ThirdParty/Steamworks/Steamv153/sdk/public/steam/steam_api.h"
#include "Interfaces/OnlineLeaderboardInterface.h"
#include "SurvivorGameMode.h"

#pragma once
DECLARE_LOG_CATEGORY_EXTERN(SurvivorLeaderboardLog, Log, All);

/**
 * 
 */
class SURVIVORS_API FSteamLeaderboards
{
private:
	SteamLeaderboard_t m_CurrentLeaderboard; // Handle to leaderboard

public:
	//SteamLeaderboard_t m_CurrentLeaderboard;// Handle to leaderboard
	int m_nLeaderboardEntries; // How many entries do we have?
	LeaderboardEntry_t m_leaderboardEntries[100]; // The entries

	FSteamLeaderboards();
	~FSteamLeaderboards() {};

	void InitSteamLeaderboard(ASurvivorGameMode* GameMode);
	void FindLeaderboard(FString pchLeaderboardName);
	void FindOrCreateLeaderboard(FString pchLeaderboardName);
	bool UploadScore(int32 score);
	//Get Before + After + Self
	bool DownloadUserScores(int32 Before, int32 After);
	bool bLeaderboardFound = false;
	bool bLeaderboardScoresFound = false;

	void OnFindLeaderboard(LeaderboardFindResult_t* pResult, bool bIOFailure);
	CCallResult<FSteamLeaderboards, LeaderboardFindResult_t> m_callResultFindLeaderboard;
	void OnUploadScore(LeaderboardScoreUploaded_t* pResult, bool bIOFailure);
	CCallResult<FSteamLeaderboards, LeaderboardScoreUploaded_t> m_callResultUploadScore;
	void OnDownloadScore(LeaderboardScoresDownloaded_t* pResult, bool bIOFailure);
	CCallResult<FSteamLeaderboards, LeaderboardScoresDownloaded_t> m_callResultDownloadScore;

	FString GetCurrentLBName();
	bool IsLeaderboardFound(const FString& LeaderboardName);
private:
	FString m_LastLeaderboardName;
	ASurvivorGameMode* m_SurvivorGameMode;
};