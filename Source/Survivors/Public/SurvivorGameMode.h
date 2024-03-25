// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Engine/UserDefinedStruct.h"

#include "LeaderboardQueryCallbackProxy.h"

#include "SurvivorGameMode.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FScorePackage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Leaderboard")
	FString PlayerName = "working";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Leaderboard")
	int32 Rank = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Leaderboard")
	int32 Score = 0;

};

class FSteamLeaderboards;


UCLASS()
class SURVIVORS_API ASurvivorGameMode : public AGameMode
{
	GENERATED_BODY()

private:
	bool bLeaderboardFound = false;
	bool bLeaderboardScoresFound = false;
	int Score = 22;
	bool bSetScore = false;
	int ScoresAhead = 0;
	int ScoreStartIndex = 0;
	int NumberOfScores = 0;
	bool bGetScore = false;
	TArray<FScorePackage> Scores;

public:
	//The name of the leaderboard we're looking up from Steam
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Leaderboard")
	FString LBName;

	UFUNCTION(BlueprintCallable, Category = "SteamFunctions")
	void SetLeaderboardScore(const FString& LeaderboardName, int32 Score);

	void SetLBScore();

	UFUNCTION(BlueprintCallable, Category = "SteamFunctions")
	void GetLeaderboardScore(int32 scoreStartIndex, int32 numberOfScores);

	void GetLBScores();

	UFUNCTION(BlueprintCallable, Category = "SteamFunctions")
	void SetLeaderboardName(FString LeaderboardName);

	//// Called when there is a successful leaderboard query
	//UPROPERTY(BlueprintAssignable)
	//FLeaderboardQueryResult OnSuccess;

	//// Called when there is an unsuccessful leaderboard query
	//UPROPERTY(BlueprintAssignable)
	//FLeaderboardQueryResult OnFailure;

	UFUNCTION(BlueprintNativeEvent, Category = "SteamFunctions")
	void LeaderboardFound(const FString& LeaderboardName);
	virtual void LeaderboardFound_Implementation(const FString& LeaderboardName);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SteamFunctions")
	void LeaderboardScoresFound(const TArray<FScorePackage>& scores);
	virtual void LeaderboardScoresFound_Implementation(TArray<FScorePackage>& scores);

	FSteamLeaderboards* g_SteamLeaderboards = NULL;

	FString GetCurrentLBName();

	UFUNCTION(BlueprintImplementableEvent)
	void OnFindLeaderboard();
public:

	ASurvivorGameMode();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
public:
	UFUNCTION(BlueprintPure, Category = "SteamFunctions")
	bool IsLeaderboardFound(const FString& LeaderboardName);
};
