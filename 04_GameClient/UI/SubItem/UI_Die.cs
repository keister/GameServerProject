// 2024090103:09
// 2024090103:09
// 김호준rkaghMy projectAssembly-CSharpUI_Die.cs


using System;
using Controller.Network;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;

public class UI_Die : UI_Base
{
    public enum Buttons
    {
        RespawnButton,
    }

    private void Awake()
    {
        Init();
    }

    public override void Init()
    {
        Bind<Button>(typeof(Buttons));
        GetButton((int)Buttons.RespawnButton).gameObject.BindEvent(OnButtonClick);
    }

    public void Active(bool active)
    {
        gameObject.SetActive(active);
    }

    public void EnableButton(bool enable)
    {
        GetButton((int)Buttons.RespawnButton).enabled = enable;
    }
    public void OnButtonClick(PointerEventData data)
    {
        Packet pkt = new();
        
        pkt.Write((UInt16)Define.PacketType.C_CHARACTER_RESPAWN);
        
        Managers.Network.Game.SendPacket(pkt);
        EnableButton(false);
    }
}