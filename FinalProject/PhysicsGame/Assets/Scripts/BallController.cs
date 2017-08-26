using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

[RequireComponent(typeof(Rigidbody))]
public class BallController : MonoBehaviour {
    public GameObject PlacePlane;
    public Color FadeToColor;
    public float SlowThreshold;
    public float ThresholdTime;
    private Rigidbody myRigidbody;
    private float stopAccumulator = 0.0f;
    private bool rigidEnabled = true;
    private SceneMover sceneMoverRef = null;
    private Renderer myRenderer;
    private Color baseColor;
    private float victoryTime = 0.0f;
    private Text timeText;
    private const string BAST_TIME = "Time: ";

    void Awake()
    {
        timeText = GameObject.FindGameObjectWithTag(Tags.TIME_TEXT).GetComponent<Text>();
        myRenderer = GetComponent<Renderer>();
        baseColor = myRenderer.material.color;

        myRigidbody = GetComponent<Rigidbody>();
        sceneMoverRef = GameObject.FindGameObjectWithTag(Tags.SCENE_MOVER).GetComponent<SceneMover>();

        SetTimeText();
    }

    void Start()
    {
        StopMoving();
        gameObject.SetActive(false);
    }

    public void StopMoving()
    {
        myRigidbody.velocity = Vector3.zero;
    }

    void Update()
    {
        victoryTime += Time.deltaTime;
        SetTimeText();

        if (rigidEnabled && myRigidbody.velocity.magnitude < SlowThreshold)
        {
            stopAccumulator += Time.deltaTime;
            myRenderer.material.color = Color.Lerp(baseColor, FadeToColor, (stopAccumulator / ThresholdTime));

            if (stopAccumulator > ThresholdTime)
            {
                // TODO PLAY WITH MAGIC VALUES OR... If this condition && not getting closer to goal :)
                // maybe players will like it because it favors them
                Reset();
            }
        }
        else
        {
            myRenderer.material.color = baseColor;
            stopAccumulator = 0.0f;
        }
    }

    void OnTriggerEnter(Collider other)
    {
        if (other.CompareTag(Tags.GOAL))
        {
            DataHelper.DidBeatBest = DataHelper.GetCurrentLevelWon() == false || victoryTime < DataHelper.GetCurrentLevelTime();
            if (DataHelper.DidBeatBest) { DataHelper.SetCurrentLevelData(true, victoryTime); }
            DataHelper.LastTime = victoryTime;

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
        StopMoving();
        myRigidbody.isKinematic = false;
        myRigidbody.detectCollisions = true;
        myRigidbody.useGravity = true;
        rigidEnabled = true;
    }

    private void DisableRigid()
    {
        StopMoving();
        myRigidbody.isKinematic = true;
        myRigidbody.detectCollisions = false;
        myRigidbody.useGravity = false;
        rigidEnabled = false;
    }

    public void Reset()
    {
        stopAccumulator = 0.0f;
        victoryTime = 0.0f;
        SetTimeText();
        StopMoving();
        PlacePlane.SetActive(true);
        gameObject.SetActive(false);
    }

    private void SetTimeText()
    {
        timeText.text = string.Concat(BAST_TIME, victoryTime.ToString(DataHelper.FORMAT));
    }

    public void MoveTo(Vector3 teleportToPos)
    {
        myRigidbody.position = teleportToPos;
    }
}
