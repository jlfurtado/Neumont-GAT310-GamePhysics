using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class PlayerController : MonoBehaviour {
    public BallController Ball;
    
    private const int LEFT_MOUSE = 0;
    //private bool placingSphere;
    //private Interactable[] interactables = null;

	void Awake()
    {
        //placingSphere = true;

        Ball.gameObject.name = "PlayerBall";

        // TODO: Interact with all objs?
        //GameObject[] interactableObjects = GameObject.FindGameObjectsWithTag(Tags.INTERACTABLE);
        //interactables = new Interactable[interactableObjects.Length];

        //for (int i = 0; i < interactables.Length; ++i)
        //{
        //    interactables[i] = interactableObjects[i].GetComponent<Interactable>();
        //}
    }
	
	// Update is called once per frame
	void Update ()
    {
        if (Input.GetMouseButtonDown(LEFT_MOUSE))
        {
            RaycastHit rco;
            Ray ray = Camera.main.ScreenPointToRay(Input.mousePosition);
            if (Physics.Raycast(ray, out rco))
            {
                HandleClickObj(rco, ray);
            }
        }
	}

    private void HandleClickObj(RaycastHit rco, Ray ray)
    {
        if (rco.collider.CompareTag(Tags.PLACE_BOX))
        {
            rco.transform.gameObject.SetActive(false);
            Ball.transform.position = rco.point + rco.normal * 1.0f;
            Ball.gameObject.SetActive(true);
            //placingSphere = false;
            Ball.PlacePlane = rco.transform.gameObject;
            Ball.StopMoving();

        }
        else if (rco.collider.CompareTag(Tags.INTERACTABLE))
        {
            Interactable clickedObj = rco.collider.gameObject.GetComponentInParent<Interactable>(); // TODO CHANGE THIS LATER!!!
            clickedObj.Interact();
        }
    }

    public void RestartLevel()
    {
        // todo other reset stuff here
        Ball.Reset();
    }
}
