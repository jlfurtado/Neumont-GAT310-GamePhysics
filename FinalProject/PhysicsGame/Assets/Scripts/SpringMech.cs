using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SpringMech : Interactable {
    public GameObject Spring;
    public Rigidbody Block1;
    public Rigidbody Block2;

    //private Renderer Renderer1;
    //private Renderer Renderer2;
    private SpringJoint Spring1;
    private SpringJoint Spring2;

    private bool locked = true;
    private float baseSpring1;
    private float baseSpring2;
    private Vector3 baseToBlockOne;
    private const float COMPRESS = 3.0f;

	// Use this for initialization
	protected override void Awake () {
        //Renderer1 = Block1.gameObject.GetComponent<Renderer>();
        //Renderer2 = Block2.gameObject.GetComponent<Renderer>();
        Spring1 = Block1.gameObject.GetComponent<SpringJoint>();
        Spring2 = Block2.gameObject.GetComponent<SpringJoint>();

        baseSpring1 = Spring1.spring;
        baseSpring2 = Spring2.spring;

        Spring1.spring = 0.0f;
        Spring2.spring = 0.0f;

        baseToBlockOne = Block1.position - Block2.position;
        Block1.transform.position = Block2.transform.position + baseToBlockOne / COMPRESS;

        MyHelperFuncs.StopRigid(Block1);
        MyHelperFuncs.StopRigid(Block2);
    }
	
    void Update()
    {
        if (locked)
        {
            Block1.transform.position = Vector3.Lerp(Block1.transform.position, Block2.transform.position + baseToBlockOne / COMPRESS, 10.0f * Time.deltaTime);
        }

        Vector3 toBlockTwo = Block2.transform.position - Block1.transform.position;
        Spring.transform.position = (Block1.transform.position + Block2.transform.position) / 2.0f;
        Spring.transform.rotation = Quaternion.LookRotation(Vector3.up, toBlockTwo.normalized);
        Spring.transform.localScale = new Vector3(Spring.transform.localScale.x, ((toBlockTwo.magnitude - 1.0f) * 0.49f), Spring.transform.localScale.z);
    }

    public override void Interact()
    {
        locked = !locked;

        Spring1.spring = locked ? 0.0f : baseSpring1;
        Spring2.spring = locked ? 0.0f : baseSpring2;

        if (locked)
        {
            MyHelperFuncs.StopRigid(Block1);
            MyHelperFuncs.StopRigid(Block2);
        }
        else
        {
            MyHelperFuncs.StartRigid(Block1);
            MyHelperFuncs.StartRigid(Block2);
        }
    }


}
