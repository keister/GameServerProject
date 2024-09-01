// 2024080621:08
// 2024080621:08
// 김호준rkaghMy projectAssembly-CSharpChatManager.cs

using System;
using Controller.Network;
using UnityEngine;

public class ChatManager : MonoBehaviour
{
    private static GameObject go;

    public static ChatManager Instance
    {
        get
        {
            Init();
            return go.GetComponent<ChatManager>();
        }
    }

    private UI_Chat _ui;
    private bool _isEnter = false;

    static void Init()
    {
        if (go == null)
        {
            go = new GameObject { name = "@Chat" };
            var manager = go.AddComponent<ChatManager>();
            manager._ui = Managers.UI.MakeSubItem<UI_Chat>(go.transform);
            Managers.Network.Chat.Bind((ushort)Define.PacketType.S_CHAT, manager.Handle_S_CHAT);
            Managers.Network.Chat.Bind((ushort)Define.PacketType.S_WHISPER, manager.Handle_S_WHISPER);
            Managers.Network.Chat.Bind((ushort)Define.PacketType.S_CHAT_FAIL, manager.Handle_S_CHAT_FAIL);
            Managers.Network.Chat.Bind((ushort)Define.PacketType.S_CHAT_ENTER, manager.Handle_S_CHAT_ENTER);
            DontDestroyOnLoad(go.gameObject);
        }
    }

    public void Enter(UInt64 characterId)
    {
        Packet chatEnterPkt = new Packet();
        chatEnterPkt.Write((UInt16)Define.PacketType.C_CHAT_ENTER);
        chatEnterPkt.Write(characterId);
        Managers.Network.Chat.SendPacket(chatEnterPkt);
    }

    public void SetField(int fieldId)
    {
        Packet chatPkt = new Packet();
        chatPkt.Write((UInt16)Define.PacketType.C_CHAT_MOVE_FIELD);
        chatPkt.Write(fieldId);
        Managers.Network.Chat.SendPacket(chatPkt);
    }

    void Handle_S_CHAT_ENTER(Packet pkt)
    {
        pkt.Read(out bool status);
    }

    void Handle_S_CHAT(Packet pkt)
    {
        pkt.Read(out Byte chatType)
            .Read(out UInt64 playerId)
            .Read(out UInt64 characterId)
            .Read(out string nickname)
            .Read(out string message);

        _ui.AddChatLine(nickname, message, chatType);
    }

    void Handle_S_WHISPER(Packet pkt)
    {
        pkt.Read(out UInt64 playerId)
            .Read(out UInt64 characterId)
            .Read(out string nickname)
            .Read(out string message);

        _ui.AddChatLine(nickname, message, 1);
    }

    void Handle_S_CHAT_FAIL(Packet pkt)
    {
        pkt.Read(out int code);
        switch (code)
        {
            case 0:
                _ui.AddChatLine("", "귓속말 대상이 존재하지 않습니다.", 3);
                break;
            default:
                return;
        }
    }
}