// 2024080318:08
// 2024080318:08
// 김호준rkaghMy projectAssembly-CSharpCharacterStateIdle.cs


using UnityEngine;

public class CharacterStateIdle : BaseState<CharacterState>
{
    private Character character;
    private int hashIsMove;
    
    public CharacterStateIdle(GameObject go) : base(go)
    {
        hashIsMove = Animator.StringToHash("isRun");
        character = go.GetComponent<Character>();
    }

    public override void OnEnter(CharacterState stateNum)
    {
        character.animator.SetBool(hashIsMove, false);
    }

    public override void OnUpdate()
    {
        // bool isStop = diff.x == 0 && diff.z == 0;
        bool isStop = character.path.Left() == 0;
        if (!isStop)
        {
            character.stateMachine.ChangeState(CharacterState.MOVE);
        }
    }

    public override void OnFixedUpdate()
    {
    }

    public override void OnExit()
    {
        
    }
}