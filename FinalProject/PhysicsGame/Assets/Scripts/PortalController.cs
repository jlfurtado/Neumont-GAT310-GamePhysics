using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PortalController : Interactable {
    public PortalEntrance EntranceRigid;
    public PortalExit ExitRigid;

    public override void Interact()
    {
        Vector3 exitPos = ExitRigid.ExitRigidBody.position;
        ExitRigid.ExitRigidBody.position = EntranceRigid.EntranceRigidBody.position;
        EntranceRigid.EntranceRigidBody.position = exitPos;

        Vector3 v1 = ExitRigid.Vel;
        ExitRigid.Vel = EntranceRigid.Vel;
        EntranceRigid.Vel = v1;

        float t1 = ExitRigid.ToggleTime;
        ExitRigid.ToggleTime = EntranceRigid.ToggleTime;
        EntranceRigid.ToggleTime = t1;

        float t2 = ExitRigid.time;
        ExitRigid.time = EntranceRigid.time;
        EntranceRigid.time = t2;
    }

    public BallController GetBallRef()
    {
        return ballRef;
    }
}
