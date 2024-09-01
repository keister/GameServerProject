// 2024080318:08
// 2024080318:08
// 김호준rkaghMy projectAssembly-CSharpCharacterStateSkill.cs

using UnityEngine;


public class CharacterStateSkill : BaseState<CharacterState>
{
    
    public float timeDuration = 0f;
    private int hashIsAttack;
    private Character character;
    public CharacterStateSkill(GameObject go) : base(go)
    {
        hashIsAttack = Animator.StringToHash("isAttack");
        character = go.GetComponent<Character>();
    }

    public override void OnEnter(CharacterState stateNum)
    {
        character.animator.SetBool(hashIsAttack, true);
        character.targetPos = character.transform.position;
        var effect = Managers.Resource.Instantiate("VFX/swing", go.transform);
        effect.transform.localPosition = new Vector3(0.236f, 0.576f, -0.13f);
        effect.transform.Rotate(-66.533f, -56.243f, -185.717f);
        effect.GetComponent<ParticleSystem>().Play();
    }

    public override void OnUpdate()
    {
        timeDuration += Time.deltaTime;
        if (timeDuration >= 0.8f)
        {
            character.stateMachine.ChangeState(CharacterState.IDLE);
        }
    }

    public override void OnFixedUpdate()
    {
   
    }

    public override void OnExit()
    {
        timeDuration = 0f;
        character.animator.SetBool(hashIsAttack, false);
    }
}