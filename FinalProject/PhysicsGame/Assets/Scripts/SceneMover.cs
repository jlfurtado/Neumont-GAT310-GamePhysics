using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class SceneMover : MonoBehaviour {
    public void MoveToTitle()
    {
        KeepTitleMusic();
        SceneManager.LoadScene(Scenes.TITLE);
    }

    public void MoveToVictory()
    {
        DontKeepTitleMusic();
        SceneManager.LoadScene(Scenes.VICTORY);
    }

    public void MoveToOptions()
    {
        KeepTitleMusic();
        SceneManager.LoadScene(Scenes.OPTIONS);
    }

    public void MoveToHowToPlay()
    {
        KeepTitleMusic();
        SceneManager.LoadScene(Scenes.HOW_TO_PLAY);
    }

    public void MoveToLevelSelect()
    {
        KeepTitleMusic();
        SceneManager.LoadScene(Scenes.LEVEL_SELECT);
    }

    public void MoveToLevelOne()
    {
        DataHelper.CurrentLevel = DataHelper.LEVEL_ONE_DATA;
        DontKeepTitleMusic();
        SceneManager.LoadScene(Scenes.LEVEL_ONE);
    }

    public void MoveToLevelTwo()
    {
        DataHelper.CurrentLevel = DataHelper.LEVEL_TWO_DATA;
        DontKeepTitleMusic();
        SceneManager.LoadScene(Scenes.LEVEL_TWO);
    }

    public void MoveToLevelThree()
    {
        DataHelper.CurrentLevel = DataHelper.LEVEL_THREE_DATA;
        DontKeepTitleMusic();
        SceneManager.LoadScene(Scenes.LEVEL_THREE);
    }

    public void MoveToLevelFour()
    {
        DataHelper.CurrentLevel = DataHelper.LEVEL_FOUR_DATA;
        DontKeepTitleMusic();
        SceneManager.LoadScene(Scenes.LEVEL_FOUR);
    }

    public void MoveToLevelFive()
    {
        DataHelper.CurrentLevel = DataHelper.LEVEL_FIVE_DATA;
        DontKeepTitleMusic();
        SceneManager.LoadScene(Scenes.LEVEL_FIVE);
    }

    public void MoveToLevelSix()
    {
        DataHelper.CurrentLevel = DataHelper.LEVEL_SIX_DATA;
        DontKeepTitleMusic();
        SceneManager.LoadScene(Scenes.LEVEL_SIX);
    }
    public void MoveToLevelSeven()
    {
        DataHelper.CurrentLevel = DataHelper.LEVEL_SEVEN_DATA;
        DontKeepTitleMusic();
        SceneManager.LoadScene(Scenes.LEVEL_SEVEN);
    }

    public void MoveToLevelEight()
    {
        DataHelper.CurrentLevel = DataHelper.LEVEL_EIGHT_DATA;
        DontKeepTitleMusic();
        SceneManager.LoadScene(Scenes.LEVEL_EIGHT);
    }

    public void ResetData()
    {
        DataHelper.ResetLevelData();
    }

    private void KeepTitleMusic()
    {
        GameObject titleMusic = GameObject.FindGameObjectWithTag(Tags.TITLE_MUSIC);
        if (titleMusic != null) { DontDestroyOnLoad(titleMusic); }
    }

    private void DontKeepTitleMusic()
    {
        GameObject titleMusic = GameObject.FindGameObjectWithTag(Tags.TITLE_MUSIC);
        if (titleMusic != null) { Destroy(titleMusic); }
    }

    public void QuitGame()
    {
        Application.Quit();
    }

}
