// 2024080922:08
// 2024080922:08
// 김호준rkaghMy projectAssembly-CSharpMonsterStateMove.cs


using UnityEngine;

public class MonsterStateMove : BaseState<MonsterState>
{
    private Monster monster;
    private int hashIsMove;

    public MonsterStateMove(GameObject go) : base(go)
    {
        hashIsMove = Animator.StringToHash("isMove");
        monster = go.GetComponent<Monster>();
    }

    public override void OnEnter(MonsterState stateNum)
    {
        monster.anim.SetBool(hashIsMove, true);
    }

    public override void OnUpdate()
    {
        Vector3 target = monster.path.GetCur(); 
        target.y = monster.transform.position.y;
        monster.transform.position = Vector3.MoveTowards(monster.transform.position, target,  monster.stat.MoveSpeed * Time.deltaTime);
        monster.transform.LookAt(target);
        
        if (target == monster.transform.position)
        {
            monster.path.Next();
        }
        
        bool isStop = monster.path.Left() == 0;
        if (isStop)
        {
            monster.StateMachine.ChangeState(MonsterState.IDLE);
        }
    }

    public override void OnFixedUpdate()
    {
    }

    public override void OnExit()
    {
    }
}