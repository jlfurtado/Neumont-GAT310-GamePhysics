using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public static class DataHelper
{
    public static bool DidBeatBest = false;
    public static float LastTime = 999999.99f;

    public const string LEVEL_ONE_DATA = "LevelOneData:";
    public const string LEVEL_TWO_DATA = "LevelTwoData:";
    public const string LEVEL_THREE_DATA = "LevelThreeData:";
    public const string LEVEL_FOUR_DATA = "LevelFourData:";
    public const string LEVEL_FIVE_DATA = "LevelFiveData:";
    public const string LEVEL_SIX_DATA = "LevelSixData:";
    public const string LEVEL_SEVEN_DATA = "LevelSevenData:";
    public const string LEVEL_EIGHT_DATA = "LevelEightData:";
    
    private const string BEATEN = "Beaten:";
    private const string BEST = "Best:";
    private const string DEFAULT_VALUES = "Beaten:false,Best:999999.99";
    private const string DELIMITER = ",";
    public const string FORMAT = "0.00";

    public static string CurrentLevel;

    public static void ResetLevelData()
    {
        PlayerPrefs.SetString(LEVEL_ONE_DATA, DEFAULT_VALUES);
        PlayerPrefs.SetString(LEVEL_TWO_DATA, DEFAULT_VALUES);
        PlayerPrefs.SetString(LEVEL_THREE_DATA, DEFAULT_VALUES);
        PlayerPrefs.SetString(LEVEL_FOUR_DATA, DEFAULT_VALUES);
        PlayerPrefs.SetString(LEVEL_FIVE_DATA, DEFAULT_VALUES);
        PlayerPrefs.SetString(LEVEL_SIX_DATA, DEFAULT_VALUES);
        PlayerPrefs.SetString(LEVEL_SEVEN_DATA, DEFAULT_VALUES);
        PlayerPrefs.SetString(LEVEL_EIGHT_DATA, DEFAULT_VALUES);
    }

    public static void SetCurrentLevelData(bool won, float time)
    {
        SetLevelData(CurrentLevel, won, time);
    }

    public static bool GetCurrentLevelWon()
    {
        return GetLevelWon(CurrentLevel);
    }

    public static float GetCurrentLevelTime()
    {
        return GetLevelTime(CurrentLevel);
    }

    public static void SetLevelData(string key, bool levelWon, float time)
    {
        PlayerPrefs.SetString(key, string.Concat(BEATEN, levelWon, DELIMITER, BEST, time.ToString(FORMAT)));
    }

    public static bool GetLevelWon(string key)
    {
        string data = PlayerPrefs.GetString(key, DEFAULT_VALUES);
        data = data.Substring(data.IndexOf(BEATEN) + BEATEN.Length);
        data = data.Substring(0, data.IndexOf(DELIMITER));
        return bool.Parse(data);
    }

    public static float GetLevelTime(string key)
    {
        string data = PlayerPrefs.GetString(key, DEFAULT_VALUES);
        return float.Parse(data.Substring(data.IndexOf(BEST) + BEST.Length));
    }

}