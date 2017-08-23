using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Interactable : MonoBehaviour {
    protected Rigidbody myRigidBody = null;
    protected BallController ballRef;

    protected virtual void Awake()
    {
        ballRef = GameObject.FindGameObjectWithTag(Tags.PLAYER_BALL).GetComponent<BallController>();
        myRigidBody = GetComponent<Rigidbody>();
    }
	
	// Update is called once per frame
	protected virtual void FixedUpdate () {
	    // DO NOTHING FOR NOW	
	}

    public virtual void Interact()
    {
        Debug.Log("Interacted with!");
    }
}
