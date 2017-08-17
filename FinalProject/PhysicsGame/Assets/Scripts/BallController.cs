using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

[RequireComponent(typeof(Rigidbody))]
public class BallController : MonoBehaviour {
    public GameObject PlacePlane;
    public float SlowThreshold;
    public float ThresholdTime;
    private Rigidbody myRigidbody;
    private float stopAccumulator = 0.0f;
    private bool rigidEnabled = true;
    private SceneMover sceneMoverRef = null;

    void Awake()
    {
        myRigidbody = GetComponent<Rigidbody>();
        sceneMoverRef = GameObject.FindGameObjectWithTag(Tags.SCENE_MOVER).GetComponent<SceneMover>();
    }

    public void StopMoving()
    {
        myRigidbody.velocity = Vector3.zero;
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
            sceneMoverRef.MoveToVictory();
            Stop();
        }
        else if (other.CompareTag(Tags.LETHAL))
        {
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

    public void Reset()
    {
        stopAccumulator = 0.0f;
        PlacePlane.SetActive(true);
        gameObject.SetActive(false);
    }
}
