using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

[RequireComponent(typeof(Rigidbody))]
public class BallController : MonoBehaviour {
    public Text tempText;
    public GameObject PlacePlane;
    public float SlowThreshold;
    public float ThresholdTime;
    private Rigidbody myRigidbody;
    private float stopAccumulator = 0.0f;
    private bool rigidEnabled = true;

    void Awake()
    {
        myRigidbody = GetComponent<Rigidbody>();
    }

    void Update()
    {
        if (rigidEnabled && myRigidbody.velocity.magnitude < SlowThreshold)
        {
            stopAccumulator += Time.deltaTime;
            if (stopAccumulator > ThresholdTime)
            {
                // TODO PLAY WITH MAGIC VALUES OR... If this condition && not getting closer to goal :)
                // maybe players will like it because it favors them
                tempText.text = "Ball is Stuck!";
                Reset();
            }
        }
        else
        {
            stopAccumulator = 0.0f;
        }
    }

    void OnTriggerEnter(Collider other)
    {
        if (other.CompareTag(Tags.GOAL))
        {
            tempText.text = "You win!";
            Stop();
        }
        else if (other.CompareTag(Tags.LETHAL))
        {
            tempText.text = "You Lose!";
            Reset();
        }
    }

    private void Stop()
    {
        DisableRigid();
    }

    private void EnableRigid()
    {
        myRigidbody.isKinematic = false;
        myRigidbody.detectCollisions = true;
        myRigidbody.useGravity = true;
        rigidEnabled = true;
    }

    private void DisableRigid()
    {
        myRigidbody.isKinematic = true;
        myRigidbody.detectCollisions = false;
        myRigidbody.useGravity = false;
        rigidEnabled = false;
    }

    private void Reset()
    {
        stopAccumulator = 0.0f;
        PlacePlane.SetActive(true);
        gameObject.SetActive(false);
    }
}
