using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SpinnerController : Interactable {
    public float TorqueStrength = 1.0f;
    private float dir = 1.0f;
    private Vector3 myStartUp = Vector3.up;

    protected override void Awake()
    {
        base.Awake();
        myStartUp = transform.rotation * Vector3.up;
        myRigidBody.centerOfMass = Vector3.zero;
    }

    // Update is called once per frame
    protected override void FixedUpdate()
    {
        myRigidBody.AddTorque(dir * myStartUp * TorqueStrength * Time.deltaTime);
    }

    public override void Interact()
    {
        dir = -dir;
        myRigidBody.angularVelocity = -myRigidBody.angularVelocity; 
    }
}
