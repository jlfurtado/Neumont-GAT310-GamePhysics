using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Interactable : MonoBehaviour {
    protected Rigidbody myRigidBody = null;

    protected virtual void Awake()
    {
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
