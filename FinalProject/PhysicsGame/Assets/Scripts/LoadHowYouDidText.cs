using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

[RequireComponent(typeof(Text))]
public class LoadHowYouDidText : MonoBehaviour {
    private const string BEST_TIME = "New best time:\n";
    private const string WORSE_TIME = "You did not beat the best time for this level of: \n";
    private const string YOUR_TIME = "Your time was: \n";
    private const string NEWLINE = "\n";

    void Awake()
    {
        Text myText = GetComponent<Text>();
        myText.text = DataHelper.DidBeatBest ? string.Concat(BEST_TIME, DataHelper.GetCurrentLevelTime())
                                             : string.Concat(WORSE_TIME, DataHelper.GetCurrentLevelTime(), NEWLINE, YOUR_TIME, DataHelper.LastTime.ToString(DataHelper.FORMAT));
    }
}
