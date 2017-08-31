using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class FaceCamera : MonoBehaviour {
    private Camera mainCameraRef;

	// Use this for initialization
	void Awake () {
        mainCameraRef = GameObject.FindGameObjectWithTag(Tags.MAIN_CAMERA).GetComponent<Camera>();
        transform.rotation = Quaternion.LookRotation(mainCameraRef.transform.forward, mainCameraRef.transform.up);
	}
}
