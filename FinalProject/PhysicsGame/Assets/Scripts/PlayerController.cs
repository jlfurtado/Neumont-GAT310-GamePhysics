using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class PlayerController : MonoBehaviour {
    public GameObject BallPrefab;
    public Text tempText;
    private BallController myBall;
    private const int LEFT_MOUSE = 0;
    private bool placingSphere;
    //private Interactable[] interactables = null;

	void Awake()
    {
        placingSphere = true;
        tempText.text = "PlacingBall";
        myBall = Instantiate(BallPrefab).GetComponent<BallController>();
        myBall.gameObject.name = "PlayerBall";
        myBall.gameObject.SetActive(false);
        myBall.tempText = tempText;

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
        if (rco.collider.gameObject.tag.Equals(Tags.PLACE_BOX))
        {
            rco.transform.gameObject.SetActive(false);
            myBall.transform.position = rco.point - ray.direction * 1.0f;
            myBall.gameObject.SetActive(true);
            placingSphere = false;
            tempText.text = "Waiting for ball";
        }
        else if (rco.collider.gameObject.tag.Equals(Tags.INTERACTABLE))
        {
            Interactable clickedObj = rco.collider.gameObject.GetComponentInParent<Interactable>(); // TODO CHANGE THIS LATER!!!
            clickedObj.Interact();
        }
    }
}
