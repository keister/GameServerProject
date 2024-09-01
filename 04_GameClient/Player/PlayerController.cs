using System;
using System.Collections;
using System.Collections.Generic;
using System.Net.NetworkInformation;
using System.Timers;
using Controller;
using Controller.Network;
using TMPro;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.InputSystem;

[RequireComponent(typeof(Player))]
public class PlayerController : MonoBehaviour
{
    [SerializeField] public Vector3 direction { get; set; }
    [SerializeField] public InputReader input;


    private void Awake()
    {
        input = ScriptableObject.CreateInstance<InputReader>();
        direction = new Vector3(0, 0, 0);

        input.moveEvent += HandleMove;
        //input.dashEvent += HandleDash;
        input.attackEvent += HandleAttack;
        input.clickEvent += HandleMoveMouse;
        input.interactionEvent += HandleInteration;
        //Managers.Network.Game.Bind((ushort)Define.PacketType.S_MOVE, Handle_S_MOVE);
        //Managers.Network.Game.Bind((ushort)Define.PacketType.S_SKILL, Handle_S_SKILL);
    }

    void Handle_S_MOVE(Packet pkt)
    {
        pkt.Read(out float y)
            .Read(out float x);
    }

    void Handle_S_SKILL(Packet pkt)
    {
        pkt.Read(out int skillId)
            .Read(out float targetY)
            .Read(out float targetX)
            .Read(out float posY)
            .Read(out float posX);

        Player.Instance.transform.position = new Vector3(posX, 0f, posY);
        Player.Instance.transform.LookAt(new Vector3(targetX, 0, targetY));

        //Player.Instance.stateMachine.ChangeState(EPlayerState.ATTACK);
    }

    private void HandleMoveMouse(Vector2 mouse)
    {
        RaycastHit hit;

        var mask = LayerMask.GetMask("Terrain");

        //var pos = Camera.main.ScreenToWorldPoint(mouse);
        if (Player.Instance.character.stateMachine.CurrentStateNum != CharacterState.ATTACK)
        {
            var ray = Camera.main.ScreenPointToRay(mouse);
            if (Physics.Raycast(ray, out hit, 100, mask, QueryTriggerInteraction.Ignore))
            {
                //Debug.Log($"{hit.transform.name}");
                //Player.Instance.character.animator.SetBool("isRun", true);

                Packet pkt = new Packet();

                float x = hit.point.x;
                float y = hit.point.z;
                x = Math.Max(0f, Math.Min(60f, x));
                y = Math.Max(0f, Math.Min(60f, y));
                
                pkt.Write((UInt16)Define.PacketType.C_MOVE);
                pkt.Write(y);
                pkt.Write(x);

                Managers.Network.Game.SendPacket(pkt);
            }
        }
    }

    private void HandleMove(Vector2 dir)
    {
        Player.Instance.character.animator.SetBool("isRun", true);
        direction = new Vector3(dir.x, 0, dir.y);
        Debug.Log($"dir : {direction}");
    }
    

    private void HandleAttack(Vector2 mouse)
    {
        if (EventSystem.current.IsPointerOverGameObject())
        {
            return;
        }
        
        if (Player.Instance.character.stateMachine.CurrentStateNum != CharacterState.ATTACK)
        {
            UInt64 objId;
            if (Player.Instance.targetView.target != null)
            {
                objId = Player.Instance.targetView.target.GetComponent<Monster>().id;
            }
            else
            {
                objId = 0;
            }

            Packet pkt = new Packet();
            pkt.Write((UInt16)Define.PacketType.C_SKILL);
            pkt.Write(objId);
            Managers.Network.Game.SendPacket(pkt);

            //Player.Instance.stateMachine.ChangeState(EPlayerState.ATTACK);
        }
    }

    // private IEnumerator DashCoolDownCoroutine()
    // {
    //     //Debug.Log("Coroutine Start");
    //     DashState dashState = Player.Instance.stateMachine.GetState(EPlayerState.DASH) as DashState;
    //
    //     dashState.dashCooldownTimer = 0;
    //
    //     while (true)
    //     {
    //         if (dashState == null)
    //         {
    //             break;
    //         }
    //
    //         dashState.dashCooldownTimer += Time.deltaTime;
    //         //Debug.Log($"Cool : {dashState.dashCooldownTimer}");
    //         if (dashState.dashCooldownTimer >= DashState.DASH_COOLDOWN)
    //         {
    //             break;
    //         }
    //
    //         yield return null;
    //     }
    // }

    private void HandleInteration()
    {  
        Debug.Log("HandleInteration");
        IInteractable obj = Player.Instance.interactView.target.GetComponent<IInteractable>();
        if (obj != null)
        {
            obj.Interact();
        }
        else
        {
            Debug.Log("Not Found");
        }
    }
}