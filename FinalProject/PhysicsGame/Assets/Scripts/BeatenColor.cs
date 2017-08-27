using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

[RequireComponent(typeof(Button))]
public class BeatenColor : MonoBehaviour {
    public ColorBlock BeatenColors;
    public string LevelDataKey;
    public string PrerequisiteLevelDataKey;
    public bool First;

	void Awake()
    {
        Button b = GetComponent<Button>();
        if (!DataHelper.GetLevelWon(PrerequisiteLevelDataKey) && !First) { b.interactable = false; }
        else if (DataHelper.GetLevelWon(LevelDataKey)) { b.colors = BeatenColors; }  
    }


}
