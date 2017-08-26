using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[RequireComponent(typeof(Rigidbody))]
public class PortalEntrance : MonoBehaviour {
    public PortalExit AttachedExit;
    public PortalController Teleporter;
    public Rigidbody EntranceRigidBody { get; private set; }
    public Vector3 Vel;
    public float ToggleTime;
    public float time;

	void Awake()
    {
        EntranceRigidBody = GetComponent<Rigidbody>();
        time = ToggleTime;
    }

    void Update()
    {
        time -= Time.deltaTime;
        if (time <= 0.0f)
        {
            Toggle();
        }

        EntranceRigidBody.velocity = Vel;
    }

    private void Toggle()
    {
        Vel = -Vel;
        time = ToggleTime;
    }

    void OnTriggerEnter(Collider other)
    {
        if (other.CompareTag(Tags.PLAYER_BALL))
        {
            Teleporter.GetBallRef().MoveTo(AttachedExit.ExitRigidBody.position);
        }
    }
}
