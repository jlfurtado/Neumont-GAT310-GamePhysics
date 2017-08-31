using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class KeepMe : MonoBehaviour {
    public GameObject backgroundMusicPrefab;

    void Start()
    {
        if (GameObject.FindGameObjectWithTag(Tags.TITLE_MUSIC) == null)
        {
            Instantiate(backgroundMusicPrefab);
        }
    }
}
