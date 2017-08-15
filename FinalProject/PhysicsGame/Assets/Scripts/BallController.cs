using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

[RequireComponent(typeof(Rigidbody))]
public class BallController : MonoBehaviour {
    public Text tempText;
    public float SlowThreshold;
    public float ThresholdTime;
    private Rigidbody myRigidbody;
    private float stopAccumulator = 0.0f;

    void Awake()
    {
        myRigidbody = GetComponent<Rigidbody>();
    }

    void Update()
    {
        if (myRigidbody.velocity.magnitude < SlowThreshold)
        {
            stopAccumulator += Time.deltaTime;
            if (stopAccumulator > ThresholdTime)
            {
                // TODO PLAY WITH MAGIC VALUES OR... If this condition && not getting closer to goal :)
                // maybe players will like it because it favors them
                tempText.text = "Ball is Stuck!";
            }
        }
        else
        {
            stopAccumulator = 0.0f;
        }
    }

    void OnTriggerEnter(Collider other)
    {
        if (other.gameObject.tag.Equals(Tags.GOAL))
        {
            tempText.text = "You win!";
        }
        else if (other.gameObject.tag.Equals(Tags.LETHAL))
        {
            tempText.text = "You Lose!";
        }
    }
}
