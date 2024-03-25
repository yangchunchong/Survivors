// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivorLeaderboard.h"
#include "OnlineSubsystemUtils.h"

DEFINE_LOG_CATEGORY(SurvivorLeaderboardLog);

FSteamLeaderboards::FSteamLeaderboards() :
	m_CurrentLeaderboard(NULL),
	m_nLeaderboardEntries(0)
{
}

void FSteamLeaderboards::InitSteamLeaderboard(ASurvivorGameMode* GameMode)
{
	if(m_SurvivorGameMode != GameMode)
		m_SurvivorGameMode = GameMode;
}

void FSteamLeaderboards::FindLeaderboard(FString pchLeaderboardName)
{
	FString name = "FindLeaderboard Called   ------  ";
	name += pchLeaderboardName;
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, name);

	m_CurrentLeaderboard = NULL;

	SteamAPICall_t hSteamAPICall = SteamUserStats()->FindLeaderboard(TCHAR_TO_UTF8(*pchLeaderboardName));
	m_callResultFindLeaderboard.Set(hSteamAPICall, this, &FSteamLeaderboards::OnFindLeaderboard);
	if (m_LastLeaderboardName != pchLeaderboardName)
		m_LastLeaderboardName = pchLeaderboardName;
}

void FSteamLeaderboards::FindOrCreateLeaderboard(FString pchLeaderboardName)
{
}

void FSteamLeaderboards::OnFindLeaderboard(LeaderboardFindResult_t* pCallback, bool bIOFailure)
{
	const char* name = "OnFindLeaderboard Called";
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, name);

	// see if we encountered an error during the call
	if (!pCallback->m_bLeaderboardFound)
	{
		name = "Leaderboard could not be found";
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, name);
		return;
	}
	else if (bIOFailure)
	{
		name = "bIOFailure";
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, name);
		return;
	}

	m_CurrentLeaderboard = pCallback->m_hSteamLeaderboard;

	if (m_CurrentLeaderboard != NULL)
	{
		bLeaderboardFound = true;
		name = "m_CurrentLeaderboard is valid";
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Yellow, name);
		m_SurvivorGameMode->OnFindLeaderboard();
	}
}

bool FSteamLeaderboards::UploadScore(int32 score)
{
	const char* name = "UploadScore Called";
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, name);

	if (!m_CurrentLeaderboard) 
	{
		name = "no leaderboard object";
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, name);
		return false;
	}


	SteamAPICall_t hSteamAPICall =
		SteamUserStats()->UploadLeaderboardScore(m_CurrentLeaderboard, k_ELeaderboardUploadScoreMethodKeepBest, score, NULL, 0);

	m_callResultUploadScore.Set(hSteamAPICall, this, &FSteamLeaderboards::OnUploadScore);

	return true;
}

void FSteamLeaderboards::OnUploadScore(LeaderboardScoreUploaded_t* pCallback, bool bIOFailure)
{
	const char* name = "OnUploadScore Called";
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, name);

	if (!pCallback->m_bSuccess)
	{
		//uint8 m_bSuccess;			// 1 if the call was successful
		//SteamLeaderboard_t m_hSteamLeaderboard;	// the leaderboard handle that was
		//int32 m_nScore;				// the score that was attempted to set
		//uint8 m_bScoreChanged;		// true if the score in the leaderboard change, false if the existing score was better
		//int m_nGlobalRankNew;		// the new global rank of the user in this leaderboard
		//int m_nGlobalRankPrevious;

		FString DebugOutput = "";
		if (pCallback->m_bSuccess) DebugOutput += "Success:";
		else DebugOutput += "Failure: ";
		DebugOutput += FString::Printf(TEXT("Attempted Score: %i, bScoreChanged: %i"), pCallback->m_nScore, pCallback->m_bScoreChanged);
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, DebugOutput);

		name = "Score could not be uploaded to Steam: WAS NOT SUCCESSFUL";
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, name);
	}
	else if (bIOFailure)
	{
		name = "Score could not be uploaded to Steam: IO ERROR";
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, name);
	}
	else
	{
		name = "Score was uploaded to Steam";
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, name);
	}
}


bool FSteamLeaderboards::DownloadUserScores(int32 Before, int32 After)
{
	const char* name = "DownloadScores Called";
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Blue, name);

	if (!m_CurrentLeaderboard)
		return false;

	// load the specified leaderboard data from first to number of places
	SteamAPICall_t hSteamAPICall = SteamUserStats()->DownloadLeaderboardEntries(
		m_CurrentLeaderboard, k_ELeaderboardDataRequestGlobal, Before, After);

	m_callResultDownloadScore.Set(hSteamAPICall, this, &FSteamLeaderboards::OnDownloadScore);

	return true;
}

void FSteamLeaderboards::OnDownloadScore(LeaderboardScoresDownloaded_t* pCallback, bool bIOFailure)
{
	FString name = "OnDownloadScore Called";
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Blue, name);

	if (!bIOFailure)
	{

		m_nLeaderboardEntries = FMath::Min(pCallback->m_cEntryCount, 100);

		for (int index = 0; index < m_nLeaderboardEntries; index++)
		{
			LeaderboardEntry_t* LeaderboardEntry = &m_leaderboardEntries[index];
			SteamUserStats()->GetDownloadedLeaderboardEntry(
				pCallback->m_hSteamLeaderboardEntries, index, LeaderboardEntry, NULL, 0);
			name = FString::Printf(TEXT("index = %d ---------  SteamID = %d Rank = %d Score = %d"), index, LeaderboardEntry->m_steamIDUser.GetAccountID(),
				LeaderboardEntry->m_nGlobalRank, LeaderboardEntry->m_nScore);
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Blue, name);
		}
		bLeaderboardScoresFound = true;
	}
}

FString FSteamLeaderboards::GetCurrentLBName()
{
	return m_LastLeaderboardName;
}

bool FSteamLeaderboards::IsLeaderboardFound(const FString& LeaderboardName)
{
	if (m_LastLeaderboardName == LeaderboardName && bLeaderboardFound)
		return true;
	return false;
}
