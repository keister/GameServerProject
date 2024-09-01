// 2024071519:07
// 2024071519:07
// 김호준rkaghMy projectAssembly-CSharpMovableObject.cs


using System;
using Controller.UI;
using UnityEngine;

public class MovableObject : MonoBehaviour
{
    private Vector3 target;

    private Vector3 direction;
    
    public Animator anim;
    private Character character;
    
    private void Awake()
    {
        target = gameObject.transform.position;
        anim = Util.FindChild<Animator>(gameObject, "@model", true);
        character = gameObject.GetComponent<Character>();
    }


    private void Update()
    {
        transform.position = Vector3.MoveTowards(transform.position, target, character.stat.MoveSpeed * Time.deltaTime);
        transform.LookAt(transform.position + direction);
        if (transform.position == target)
        {
            anim.SetBool("isRun", false);
        }
        
    }

    public void Attack()
    {
        anim.SetBool("isAttack", true);
        var effect = Managers.Resource.Instantiate("VFX/swing", gameObject.transform);
        effect.transform.localPosition = new Vector3(0.236f, 0.576f, -0.13f);
        effect.transform.Rotate(-66.533f, -56.243f, -185.717f);
        effect.GetComponent<ParticleSystem>().Play();
        Invoke("AttackRelease", 0.8f);
    }

    void AttackRelease()
    {
        anim.SetBool("isAttack", false);
    }

    public void SetTargetPosition(Vector3 vec)
    {
        target = vec;
        direction = (vec - transform.position).normalized;
    }

    public void Stop()
    {
        direction = Vector3.zero;
    }
}