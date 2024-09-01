// 2024090103:09
// 2024090103:09
// 김호준rkaghMy projectAssembly-CSharpCharacterStateDeath.cs

using UnityEngine;

public class CharacterStateDeath : BaseState<CharacterState>
{
    private Character character;
    private int hashIsDie;


    public CharacterStateDeath(GameObject go) : base(go)
    {
        hashIsDie = Animator.StringToHash("isDie");
        character = go.GetComponent<Character>();
    }

    public override void OnEnter(CharacterState stateNum)
    {
        character.animator.SetBool(hashIsDie, true);
    }

    public override void OnUpdate()
    {

    }

    public override void OnFixedUpdate()
    {

    }

    public override void OnExit()
    {
        character.animator.SetBool(hashIsDie, false);
    }
}