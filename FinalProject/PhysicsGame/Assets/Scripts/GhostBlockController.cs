using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[RequireComponent(typeof(Rigidbody), typeof(Renderer))]
public class GhostBlockController : Interactable {
    public Material DefaultMat;
    public Material GhostMat;
    public bool StartGhost = false;
    private int defaultLayer;
    private int ghostLayer;
    private bool ghost;
    private Renderer myRenderer;

    protected override void Awake () {
        base.Awake();
        myRenderer = GetComponent<Renderer>();
        defaultLayer = LayerMask.NameToLayer(Layers.DEFAULT);
        ghostLayer = LayerMask.NameToLayer(Layers.GHOST);
        ghost = StartGhost;
        gameObject.layer = ghost ? ghostLayer : defaultLayer;
        myRenderer.material = ghost ? GhostMat : DefaultMat;
    }
	

    public override void Interact()
    {
        ghost = !ghost;
        gameObject.layer = ghost ? ghostLayer : defaultLayer;
        myRenderer.material = ghost ? GhostMat : DefaultMat;
    }
}
