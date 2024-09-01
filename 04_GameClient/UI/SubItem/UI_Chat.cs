// 2024080419:08
// 2024080419:08
// 김호준rkaghMy projectAssembly-CSharpUI_Chat.cs


using System;
using System.Collections.Generic;
using Controller.Network;
using TMPro;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.UI;

public class UI_Chat : UI_Base
{
    enum InputFields
    {
        ChatInput,
    }

    enum Texts
    {
        ChatText,
    }

    enum GameObjects
    {
        Content,
        ChatView,
    }

    private InputReader input;

    private GameObject chatItemPool;
    private Queue<UI_ChatLine> chatItems = new();
    private Queue<UI_ChatLine> curItems = new();
    
    private void Awake()
    {
        Init();
    }

    public override void Init()
    {
        GameObject chatItemPool = new GameObject { name = "@ChatItemPool" };
        chatItemPool.transform.SetParent(gameObject.transform);
        
        for (int i = 0; i < 100; i++)
        {
            UI_ChatLine chatItem = Managers.UI.MakeSubItem<UI_ChatLine>(chatItemPool.transform);
            chatItem.gameObject.SetActive(false);
            chatItems.Enqueue(chatItem);
        }
        
        input = ScriptableObject.CreateInstance<InputReader>();
        
        input.chatStartEvent += OnChatStart;
        input.chatEndEvent += OnChatEnd;
        Bind<TMP_InputField>(typeof(InputFields));
        Bind<GameObject>(typeof(GameObjects));
        Bind<TMP_Text>(typeof(Texts));
        GetInputField((int)InputFields.ChatInput).onSubmit.AddListener(OnSubmit);
    }

    private void Update()
    {

    }

    void OnChatStart()
    {
        GetInputField((int)InputFields.ChatInput).interactable = true;
        GetInputField((int)InputFields.ChatInput).ActivateInputField();
    }

    public UI_ChatLine AddChatLine(string nickname, string str, Byte chatType)
    {
        UI_ChatLine item;


        
        if (curItems.Count == 100)
        {
            item = curItems.Dequeue();
            item.gameObject.transform.SetAsLastSibling();
            curItems.Enqueue(item);
        }
        else
        {
            item = chatItems.Dequeue();
            curItems.Enqueue(item);
            item.gameObject.transform.SetParent(GetObject((int)GameObjects.Content).transform);
        }
        item.gameObject.SetActive(true);

        string prefix = "";
        
        if (chatType == 0)
        {
            prefix = $"{DateTime.Now:[HH:mm]} [일반] {nickname} : ";
            item.SetColor(Color.white);   
        }
        else if (chatType == 1)
        {
            prefix = $"{DateTime.Now:[HH:mm]} {nickname} 님의 귓속말 : ";
            item.SetColor(Color.magenta);
        }
        else if (chatType == 2)
        {
            prefix = $"{DateTime.Now:[HH:mm]} {nickname} 님에게 귓속말 : ";
            item.SetColor(Color.magenta);
        }
        else
        {
            prefix = "";
            item.SetColor(Color.red);
        }
        
        
        item.SetText($"{prefix}{str}");
        GetObject((int)GameObjects.ChatView).GetComponent<ScrollRect>().verticalNormalizedPosition = 0;
        return item;
    }
    
    void OnSubmit(string r)
    {
        if (GetInputField((int)InputFields.ChatInput).text == "")
        {
        }
        else
        {
            string msg = GetInputField((int)InputFields.ChatInput).text.Trim();

            if (msg.StartsWith("/"))
            {
                string targetNickname;
            
                string[] parts = msg.Split(new[] { ' ' }, 3);
                if (parts.Length > 2)
                {
                    if (parts[0] == "/w" || parts[0] == "/ㅈ")
                    {
                        AddChatLine(parts[1], parts[2], 2);
                        Packet pkt = new Packet();
                        pkt.Write((UInt16)Define.PacketType.C_WHISPER);
                        pkt.Write(parts[1]);
                        pkt.Write(parts[2]);
                        Managers.Network.Chat.SendPacket(pkt);
                    }
                }
            }
            else
            {
                AddChatLine(Player.Instance.character.nickname, msg, 0);
                Packet pkt = new Packet();
                pkt.Write((UInt16)Define.PacketType.C_CHAT);
                pkt.Write((Byte)0);
                pkt.Write(msg);
                Managers.Network.Chat.SendPacket(pkt);
            }
        }
        
        
        GetInputField((int)InputFields.ChatInput).text = "";
        GetInputField((int)InputFields.ChatInput).DeactivateInputField();
        GetInputField((int)InputFields.ChatInput).interactable = false;
        
        
    }
    void OnChatEnd()
    {
        // Managers.UI.MakeSubItem<UI_ChatLine>(GetObject((int)GameObjects.Content).transform).SetText(GetInputField((int)InputFields.ChatInput).text.Replace(" ", "\u00A0"));
        // GetObject((int)GameObjects.ChatView).GetComponent<ScrollRect>().normalizedPosition = new(0f, 0f);
        // GetInputField((int)InputFields.ChatInput).text = "";
        // GetInputField((int)InputFields.ChatInput).DeactivateInputField();
        // GetInputField((int)InputFields.ChatInput).interactable = false;
        

    }
}