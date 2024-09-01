// 2024070304:07
// 2024070304:07
// 김호준rkaghMy projectAssembly-CSharpUI_ServerSelect.cs

using System;
using System.Collections.Generic;
using System.Text;
using Controller.Network;
using TMPro;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;

namespace Controller.UI
{
    public class UI_ServerSelect : UI_Scene
    {
        private void Start()
        {
            Init();
        }

        enum GameObjects
        {
            Grid_ServerList,
        }

        enum Buttons
        {
            RefreshButton
        }

        enum Texts
        {
            RefreshText,
        }

        public void Awake()
        {
            Managers.Network.Login.Bind((UInt16)Define.PacketType.S_GET_SERVER_LIST, OnRecvServerList);
            Managers.Network.Game.Bind((UInt16)Define.PacketType.S_GAME_LOGIN, Handle_S_GAME_LOGIN);
            Managers.Network.Chat.Bind((UInt16)Define.PacketType.S_CHAT_LOGIN, Handle_S_CHAT_LOGIN);
        }

        
        public UI_Server_Slot[] Slots = new UI_Server_Slot[8];
        public float refreshTimer = 0f;
        public bool isTimer = false;
        public override void Init()
        {
            base.Init();
            
            Bind<GameObject>(typeof(GameObjects));
            Bind<Button>(typeof(Buttons));
            Bind<TMP_Text>(typeof(Texts));
            
            Packet pkt = new Packet();
            GameObject grid = Get<GameObject>((int)GameObjects.Grid_ServerList);
            
            for (int i = 0; i < 8; i++)
            {
                GameObject item = Managers.UI.MakeSubItem<UI_Server_Slot>(grid.transform).gameObject;
                Slots[i] = item.GetOrAddComponent<UI_Server_Slot>();
                Slots[i].Deactive();
                
            }
            
            GetButton((int)Buttons.RefreshButton).gameObject.BindEvent(OnClickRefreshButton);
            pkt.Write((UInt16)Define.PacketType.C_GET_SERVER_LIST);
            Managers.Network.Login.SendPacket(pkt);
        }
        
        public void OnRecvServerList(Packet pkt)
        {
            pkt.Read(out List<UInt16> ports);
            
            for (int i = 0; i < ports.Count; i++)
            {
                Slots[i].Active();
                Slots[i].SetInfo("서버", ports[i]);
            }
        }

        private void Update()
        {
            if (isTimer == true)
            {

                TMP_Text text = GetText((int)Texts.RefreshText);
                refreshTimer -= Time.deltaTime;
                text.text = ((int)refreshTimer).ToString();

                if (refreshTimer <= 0)
                {
                    isTimer = false;
                    text.text = "Refresh";
                    GetButton((int)Buttons.RefreshButton).enabled = true;
                }
            }
        }

        public void OnClickRefreshButton(PointerEventData data)
        {
            GetButton((int)Buttons.RefreshButton).enabled = false;
            refreshTimer = 5f;
            isTimer = true;
            GetText((int)Texts.RefreshText).text = 5.ToString();
            Packet pkt = new Packet();
            pkt.Write((UInt16)Define.PacketType.C_GET_SERVER_LIST);
            Managers.Network.Login.SendPacket(pkt);
        }
        
        public void Handle_S_GAME_LOGIN(Packet pkt)
        {
            pkt.Read(out bool status);

            if (status == true)
            {
                Packet ChatPkt = new Packet();
                ChatPkt.Write((UInt16)Define.PacketType.C_CHAT_LOGIN); 
                ChatPkt.Write((UInt64)PlayerPrefs.GetInt("AccountId"));
                ChatPkt.Write(Encoding.UTF8.GetBytes(PlayerPrefs.GetString("PlayerToken")));
                Managers.Network.Chat.SendPacket(ChatPkt);
            }
            else
            {
                Debug.Log("fail");
            }
        }

        void Handle_S_CHAT_LOGIN(Packet pkt)
        {
            pkt.Read(out bool status);
            
            if (status == true)
            {
                Managers.Scene.LoadScene(Define.Scene.CharacterSelect);
            }
            else
            {
                Debug.Log("fail");
            }
        }
    }
}