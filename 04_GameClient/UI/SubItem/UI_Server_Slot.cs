// 2024070304:07
// 2024070304:07
// 김호준rkaghMy projectAssembly-CSharpUI_Server_Slot.cs

using System;
using System.Text;
using Controller.Network;
using TMPro;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;

public class UI_Server_Slot : UI_Base
{
    enum Texts
    {
        ServerSlotText
    }

    enum Buttons
    {
        ServerSlotButton
    }

    private string _ip;
    private UInt16 _port;

    private void Awake()
    {
        Init();
    }

    public override void Init()
    {
        Bind<TMP_Text>(typeof(Texts));
        Bind<Button>(typeof(Buttons));

       // GetText((int)Texts.ServerSlotText).text = $"{_ip}:{_port}";
        //Get<>((int)GameObjects.Text_ServerSlot).GetComponent<TMP_Text>().text = $"{_ip}:{_port}";
        GetButton((int)Buttons.ServerSlotButton).gameObject.BindEvent(OnClicked);
    }

    public void Deactive()
    {
        GetButton((int)Buttons.ServerSlotButton).interactable = false;
    }
    
    public void Active()
    {
        GetButton((int)Buttons.ServerSlotButton).interactable = true;
    }
    
    public void SetInfo(string ip, UInt16 port)
    {
        _ip = ip;
        _port = port;
        Debug.Log($"{_ip}, {_port}");
        // Debug.Log($"{gameObject.activeSelf}");
        //
        // Debug.Log($"{GetText((int)Texts.ServerSlotText)}");
        GetText((int)Texts.ServerSlotText).text = $"{_ip}:{_port}";
    }

    public void OnClicked(PointerEventData data)
    {
        Managers.Network.Login.Disconnect();
        Managers.Network.Game.Connect("localhost", _port);
        Managers.Network.Chat.Connect("localhost", _port + 1);
        
        Packet pkt = new Packet();
        pkt.Write((UInt16)Define.PacketType.C_GAME_LOGIN); 
        pkt.Write((UInt64)PlayerPrefs.GetInt("AccountId"));
        pkt.Write(Encoding.UTF8.GetBytes(PlayerPrefs.GetString("PlayerToken")));
        Managers.Network.Game.SendPacket(pkt);
    }


}