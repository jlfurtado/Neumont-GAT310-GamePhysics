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
        DontKeepTitleMusic();
        SceneManager.LoadScene(Scenes.LEVEL_ONE);
    }

    public void MoveToLevelTwo()
    {
        DontKeepTitleMusic();
        SceneManager.LoadScene(Scenes.LEVEL_TWO);
    }

    public void MoveToLevelThree()
    {
        DontKeepTitleMusic();
        SceneManager.LoadScene(Scenes.LEVEL_THREE);
    }

    public void MoveToLevelFour()
    {
        DontKeepTitleMusic();
        SceneManager.LoadScene(Scenes.LEVEL_FOUR);
    }

    public void MoveToLevelFive()
    {
        DontKeepTitleMusic();
        SceneManager.LoadScene(Scenes.LEVEL_FIVE);
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
