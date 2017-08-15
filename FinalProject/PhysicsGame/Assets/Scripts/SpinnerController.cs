using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SpinnerController : Interactable {
    public float RotateSpeed;
    private float rotation = 0.0f;

    // Update is called once per frame
    protected override void Update()
    {
        rotation += RotateSpeed * Time.deltaTime;
        myRigidBody.transform.rotation = Quaternion.Euler(0.0f, rotation, 0.0f);
    }

    public override void Interact()
    {
        RotateSpeed = -RotateSpeed;
    }
}
