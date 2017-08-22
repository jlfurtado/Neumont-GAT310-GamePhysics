using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public static class MyHelperFuncs
{
    public static void EnableRigid(Rigidbody rigid)
    {
        rigid.isKinematic = false;
        rigid.detectCollisions = true;
        rigid.useGravity = true;
    }

    public static void DisableRigid(Rigidbody rigid)
    {
        rigid.isKinematic = true;
        rigid.detectCollisions = false;
        rigid.useGravity = false;
    }

    public static void StopRigid(Rigidbody rigid)
    {
        rigid.isKinematic = true;
        rigid.velocity = Vector3.zero;
    }

    public static void StartRigid(Rigidbody rigid)
    {
        rigid.isKinematic = false;
    }
}
