using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PortalExit : MonoBehaviour {
    public Rigidbody ExitRigidBody { get; private set; }
    public PortalController Teleporter;
    public Vector3 Vel;
    public float ToggleTime;
    public float time;

    void Awake()
    {
        ExitRigidBody = GetComponent<Rigidbody>();
        time = ToggleTime;
    }

    void Update()
    {
        time -= Time.deltaTime;
        if (time <= 0.0f)
        {
            Toggle();
        }

        ExitRigidBody.velocity = Vel;
    }

    private void Toggle()
    {
        Vel = -Vel;
        time = ToggleTime;
    }

    //void OnTriggerEnter(Collider other)
    //{
    //    if (other.CompareTag(Tags.PLAYER_BALL))
    //    {
            
    //    }
    //}
}
