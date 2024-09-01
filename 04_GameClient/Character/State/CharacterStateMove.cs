// 2024080318:08
// 2024080318:08
// 김호준rkaghMy projectAssembly-CSharpCharacterStateMove.cs


using UnityEngine;

public class CharacterStateMove : BaseState<CharacterState>
{
    private Character character;
    private int hashIsMove;
    
    public CharacterStateMove(GameObject go) : base(go)
    {
        hashIsMove = Animator.StringToHash("isRun");
        character = go.GetComponent<Character>();
    }

    public override void OnEnter(CharacterState stateNum)
    {
        character.animator.SetBool(hashIsMove, true);
    }

    public override void OnUpdate()
    {
        Vector3 target = character.path.GetCur(); 
        target.y = character.transform.position.y;
        character.transform.position = Vector3.MoveTowards(character.transform.position, target,  character.stat.MoveSpeed * Time.deltaTime);
        character.transform.LookAt(target);
        if (target == character.transform.position)
        {
            character.path.Next();
        }
        
        bool isStop = character.path.Left() == 0;
        if (isStop)
        {
            character.stateMachine.ChangeState(CharacterState.IDLE);
        }
    }

    public override void OnFixedUpdate()
    {

    }

    public override void OnExit()
    {

    }
}