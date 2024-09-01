// 2024070401:07
// 2024070401:07
// 김호준rkaghMy projectAssembly-CSharpUI_CharacterSelect.cs

using Controller.Network;
using System;
using JetBrains.Annotations;
using Unity.VisualScripting;
using UnityEngine.EventSystems;
using UnityEngine.UI;
using UnityEngine;
namespace Controller.UI
{
    public class UI_CharacterSelect: UI_Scene
    {
        public enum Buttons
        {
            CreateButton,
            SelectButton,
        }

        enum GameObjects
        {
            CharacterList,
        }
       
        private void Awake()
        {
            Init();
        }

        public UI_Character_Slot[] Slots = new UI_Character_Slot[6];
        [CanBeNull] public UI_Character_Slot curSlot;
        public override void Init()
        {
            base.Init();
            
            Bind<Button>(typeof(Buttons));
            Bind<GameObject>(typeof(GameObjects));

            GameObject grid = Get<GameObject>((int)GameObjects.CharacterList);

            for (int i = 0; i < 6; i++)
            {
                GameObject item = Managers.UI.MakeSubItem<UI_Character_Slot>(grid.transform).gameObject;
                Slots[i] = item.GetOrAddComponent<UI_Character_Slot>();
                BindEvent(Slots[i].btn.gameObject, OnSlotClick);
                item.SetActive(false);
            }


            Packet pkt = new Packet();
            pkt.Write((UInt16)Define.PacketType.C_GET_CHARACTER_LIST);
            
            Managers.Network.Game.SendPacket(pkt);
            
            GetButton((int)Buttons.CreateButton).gameObject.BindEvent(OnButtonClick);
            GetButton((int)Buttons.SelectButton).gameObject.BindEvent(OnSelectButtonClick);
        }

        public void Update()
        {
            if (curSlot is null)
            {
                
            }
        }



        public void OnSlotClick(PointerEventData data)
        {
            Debug.Log($"{data}");
            curSlot = data.selectedObject.transform.parent.GetComponent<UI_Character_Slot>();
        }
        public void OnButtonClick(PointerEventData data)
        {
            Managers.Scene.LoadScene(Define.Scene.CharacterCreate);
        }

        public void OnSelectButtonClick(PointerEventData data)
        {


            if (curSlot != null)
            {
                Packet pkt = new Packet();
                pkt.Write((UInt16)Define.PacketType.C_GAME_ENTER);
                pkt.Write(curSlot._id);
                pkt.Write(curSlot._idx);
                ChatManager.Instance.Enter(curSlot._id);
                Managers.Network.Game.SendPacket(pkt);
            }
        }
        
    }
}