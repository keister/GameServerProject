using System;
using System.Collections;
using System.Collections.Generic;
using Controller;
using Controller.UI;
using Unity.VisualScripting;
using UnityEngine;

[Serializable]
public class MonsterNameData : ILoader<UInt64, string>
{
    [Serializable]
    public class MonsterName
    {
        public UInt64 id;
        public string name;
        // public int level;
        // public int hp;
        // public int attackDamage;
        // public int speed;
        // public float attackRange;
    }
    public List<MonsterName> monsters = new();
    public Dictionary<UInt64, string> MakeDict()
    {
        Dictionary<UInt64, string> dict = new();

        foreach (MonsterName data in monsters)
        {
            dict.Add(data.id, data.name);
        }

        return dict;
    }
}
public class Monster : MonoBehaviour
{
    public UInt64 id;
    public Vector3 target;
    public Animator anim;
    public Stat stat;
    public int hashIsMove;
    public int hashIsAttack;
    public Rigidbody rigidBody { get; private set; }
    public StateMachine<MonsterState> StateMachine;
    public UI_HPBar _UIHpBar;
    public PathReciever path;
    private void Awake()
    {
        target = transform.position;
        anim = GetComponent<Animator>();
        stat = gameObject.AddComponent<Stat>();
        rigidBody = Util.GetOrAddComponent<Rigidbody>(gameObject);
        rigidBody.constraints = RigidbodyConstraints.FreezePositionX | RigidbodyConstraints.FreezePositionZ | RigidbodyConstraints.FreezeRotation;
        rigidBody.excludeLayers = LayerMask.GetMask("Monster", "Player");
        path = gameObject.AddComponent<PathReciever>();
        hashIsAttack = Animator.StringToHash("isAttack");
        hashIsMove = Animator.StringToHash("isMove");
        _UIHpBar = Managers.UI.MakeWorldSpace<UI_HPBar>(transform);

        StateMachine = new StateMachine<MonsterState>(MonsterState.IDLE, new MonsterStateIdle(gameObject));
        StateMachine.AddState(MonsterState.MOVE, new MonsterStateMove(gameObject));
        StateMachine.AddState(MonsterState.ATTACK, new MonsterStateAttack(gameObject));
    }

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        if (anim != null)
            StateMachine?.UpdateState();
    }
}
