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

    public void MoveToLevelOne()
    {
        DontKeepTitleMusic();
        SceneManager.LoadScene(Scenes.LEVEL_ONE);
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
