// 2024080922:08
// 2024080922:08
// 김호준rkaghMy projectAssembly-CSharpMonsterStateAttack.cs


using UnityEngine;

public class MonsterStateAttack : BaseState<MonsterState>
{
    public float timeDuration = 0f;
    private int hashIsAttack;
    private Monster monster;

    public MonsterStateAttack(GameObject go) : base(go)
    {
        hashIsAttack = Animator.StringToHash("isAttack");
        monster = go.GetComponent<Monster>();
    }

    public override void OnEnter(MonsterState stateNum)
    {
        monster.anim.SetTrigger(hashIsAttack);
    }

    public override void OnUpdate()
    {
        monster.StateMachine.ChangeState(MonsterState.IDLE);
    }

    public override void OnFixedUpdate()
    {
    }

    public override void OnExit()
    {
        timeDuration = 0f;
    }
}