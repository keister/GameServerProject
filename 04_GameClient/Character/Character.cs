using System.Collections;
using System.Collections.Generic;
using Controller.UI;
using UnityEngine;
using System;
using Controller;
using Unity.VisualScripting;

public class Character : MonoBehaviour
{
    public UInt64 id;
    public string nickname;
    public Int32 exp;
    public Int32 fieldId;
    public Int32 modelId;
    public Int32 weaponId;

    public Vector3 targetPos;
    public PathReciever path;
    
    public Stat stat;

    public StateMachine<CharacterState> stateMachine;
    
    public Rigidbody rigidBody { get; private set; }
    public Animator animator { get; private set; }
    public CapsuleCollider capsuleCollider { get; private set; }
    GameObject model;
    public GameObject prop;
    public GameObject currentWeapon;
    private readonly string[] weaponPath = { "Axe2H", "Mace2H", "Spear2H" };
    // Start is called before the first frame update
    void Awake()
    {
        gameObject.layer = LayerMask.NameToLayer("Player");
        rigidBody = Util.GetOrAddComponent<Rigidbody>(gameObject);
        rigidBody.constraints = RigidbodyConstraints.FreezePositionX | RigidbodyConstraints.FreezePositionZ | RigidbodyConstraints.FreezeRotation;
        rigidBody.excludeLayers = LayerMask.GetMask("Monster", "Player", "Interactable", "Obstacle", "SectionFilter", "Default");
        capsuleCollider = Util.GetOrAddComponent<CapsuleCollider>(gameObject);
        capsuleCollider.center = new Vector3(0f, 0.59f, 0f);
        capsuleCollider.height = 1.2f;
        
        
        stat = gameObject.AddComponent<Stat>();
        targetPos = transform.position;
        path = gameObject.AddComponent<PathReciever>();
        stateMachine = new StateMachine<CharacterState>(CharacterState.IDLE, new CharacterStateIdle(gameObject));
        stateMachine.AddState(CharacterState.MOVE, new CharacterStateMove(gameObject));
        stateMachine.AddState(CharacterState.ATTACK, new CharacterStateSkill(gameObject));
        stateMachine.AddState(CharacterState.DEATH, new CharacterStateDeath(gameObject));
    }
    public void SetModel()
    {
        if (model != null)
        {
            Destroy(model);
        }
        
        model = Managers.Resource.Instantiate($"Character/Type{modelId + 1}", transform);
        model.gameObject.name = "@model";
        animator = model.GetComponent<Animator>();
        
        prop = Util.FindChild(gameObject, "Prop_R", true);
        SetWeapon(weaponPath[weaponId]);
    }
    // Update is called once per frame
    void Update()
    {
        if (animator != null)
            stateMachine?.UpdateState();
    }

    private void FixedUpdate()
    {
        stateMachine?.FixedUpdateState();
    }

    public void SetWeapon(string path)
    {
        if (currentWeapon != null)
        {
            Managers.Resource.Destroy(currentWeapon);
        }
        
        currentWeapon = Managers.Resource.Instantiate($"Weapons/{path}", prop.transform);
    }
}
