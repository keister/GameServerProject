// 2024082121:08
// 2024082121:08
// 김호준rkaghMy projectAssembly-CSharpPortal.cs

using System;
using Controller.Network;
using UnityEngine;


public class Portal : IInteractable
{
    [SerializeField] private Define.Scene portalScene;

    private void Awake()
    {
        guide = Managers.UI.MakeWorldSpace<UI_InteractionGuide>(transform);;
        guide.SetObject(gameObject);
        guide.gameObject.SetActive(false);
    }

    public override void Interact()
    {
        Debug.Log("Interact");
        Packet pkt = new Packet();
        
        Debug.Log($"{(byte)portalScene}");
        pkt.Write((UInt16)Define.PacketType.C_MOVE_FIELD);
        pkt.Write((byte)portalScene);
        Managers.Network.Game.SendPacket(pkt);
    }

    public override string GetInfoString()
    {
        return "포탈 이동";
    }
}