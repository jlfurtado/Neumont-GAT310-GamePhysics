using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

[RequireComponent(typeof(Button))]
public class BeatenColor : MonoBehaviour {
    public ColorBlock BeatenColors;
    public string LevelDataKey;

	void Awake()
    {
        if (DataHelper.GetLevelWon(LevelDataKey)) { GetComponent<Button>().colors = BeatenColors; }  
    }


}
