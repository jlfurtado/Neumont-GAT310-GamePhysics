using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SpinnerController : Interactable {
    public float RotateSpeed;
    private float rotation = 0.0f;
    private Vector3 myStartUp = Vector3.up;
    private Vector3 myStartForward = Vector3.forward;
    private Quaternion myQuat1;

    protected override void Awake()
    {
        base.Awake();
        myQuat1 = transform.rotation;
        myStartUp = transform.rotation * Vector3.up;
        myStartForward = transform.rotation * Vector3.forward;
    }

    // Update is called once per frame
    protected override void Update()
    {
        rotation += RotateSpeed * Time.deltaTime;
        //myRigidBody.transform.rotation = Quaternion.LookRotation(Quaternion.AngleAxis(rotation, myStartUp) * myStartForward, myStartUp);
        transform.rotation = Quaternion.AngleAxis(rotation, myStartUp) * myQuat1;
    }

    public override void Interact()
    {
        RotateSpeed = -RotateSpeed;
    }
}
