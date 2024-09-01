// 2024080922:08
// 2024080922:08
// 김호준rkaghMy projectAssembly-CSharpMonsterStateIdle.cs


using UnityEngine;

public class MonsterStateIdle : BaseState<MonsterState>
{
    private Monster monster;
    private int hashIsMove;
    public MonsterStateIdle(GameObject go) : base(go)
    {
        hashIsMove = Animator.StringToHash("isMove");
        monster = go.GetComponent<Monster>();
    }

    public override void OnEnter(MonsterState stateNum)
    {
        monster.anim.SetBool(hashIsMove, false);
    }

    public override void OnUpdate()
    {
        bool isStop = monster.path.Left() == 0;
        if (!isStop)
        {
            monster.StateMachine.ChangeState(MonsterState.MOVE);
        }
    }

    public override void OnFixedUpdate()
    {
        
    }

    public override void OnExit()
    {
        
    }
}