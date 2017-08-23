using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BombMech : Interactable {
    public float ExplosionStrength = 1.0f;
    public float ParticleTime = 1.0f;
    public float Radius = 2.0f;

    public GameObject ParticlePrefab;
    private GameObject myParticles;
    private bool interactable = true;

    protected override void Awake()
    {
        myParticles = Instantiate(ParticlePrefab);
        myParticles.transform.parent = transform;
        myParticles.transform.localPosition = Vector3.zero;

        myParticles.SetActive(false);
        base.Awake();
    }

    //protected override void FixedUpdate()
    //{
    //    base.FixedUpdate();
    //}

    public override void Interact()
    {
        if (interactable)
        {
            StartCoroutine(DoExplode(ParticleTime));
        }
    }

    private IEnumerator DoExplode(float time)
    {
        interactable = false;
        myParticles.SetActive(true);

        Vector3 toBall = ballRef.transform.position - transform.position;

        RaycastHit hit;
        if (Physics.Raycast(transform.position, toBall.normalized, out hit, Radius) && hit.rigidbody != null)
        {
            hit.rigidbody.AddForceAtPosition(toBall.normalized * ExplosionStrength, hit.point, ForceMode.Impulse);
        }

        yield return new WaitForSeconds(time);

        myParticles.SetActive(false);
        interactable = true;
    }
}
