// 2024070701:07
// 2024070701:07
// 김호준rkaghMy projectAssembly-CSharpUI_CharacterCreate.cs

using System;
using Controller.Network;
using TMPro;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;

namespace Controller.UI
{
    public class UI_CharacterCreate : UI_Scene
    {
        
        public enum Texts
        {
            NicknameText,
            ModelTypeText,
            WeaponTypeText,
        }

        public enum Buttons
        {
            ModelRightArrow,
            ModelLeftArrow,
            WeaponRightArrow,
            WeaponLeftArrow,
            SelectButton,
        }

        private static readonly string[] ModelTypeString = { "A", "B", "C" };
        private static readonly string[] WeaponTypeString = { "Sword", "Mace", "Spear" };
        
        public int modelNum = 1;
        public int weaponNum = 1;
        private void Awake()
        {
            Init();
        }

        public override void Init()
        {
            base.Init();

            Bind<TMP_Text>(typeof(Texts));
            Bind<Button>(typeof(Buttons));
            GetText((int)Texts.ModelTypeText).text = ModelTypeString[modelNum];
            GetText((int)Texts.WeaponTypeText).text = WeaponTypeString[weaponNum];
            GetButton((int)Buttons.ModelLeftArrow).gameObject.BindEvent(OnModelArrowClicked);
            GetButton((int)Buttons.ModelRightArrow).gameObject.BindEvent(OnModelArrowClicked);
            GetButton((int)Buttons.WeaponLeftArrow).gameObject.BindEvent(OnWeaponArrowClicked);
            GetButton((int)Buttons.WeaponRightArrow).gameObject.BindEvent(OnWeaponArrowClicked);
            GetButton((int)Buttons.SelectButton).gameObject.BindEvent(OnSelectButtonClicked);
        }


        void OnSelectButtonClicked(PointerEventData data)
        {
            Packet pkt = new Packet();
            pkt.Write((UInt16)Define.PacketType.C_CREATE_CHARACTER);
            pkt.Write(GetText((int)Texts.NicknameText).text);
            pkt.Write(modelNum);
            pkt.Write(weaponNum);
            Managers.Network.Game.SendPacket(pkt);
        }

        
        void OnModelArrowClicked(PointerEventData data)
        {
            if (data.selectedObject == GetButton((int)Buttons.ModelLeftArrow).gameObject)
            {
                modelNum--;
                if (modelNum == 0)
                    data.selectedObject.SetActive(false);
                if (modelNum != 2)
                    GetButton((int)Buttons.ModelRightArrow).gameObject.SetActive(true);
            }
            else
            {
                modelNum++;
                if (modelNum == 2)
                    data.selectedObject.SetActive(false);
                if (modelNum != 0)
                    GetButton((int)Buttons.ModelLeftArrow).gameObject.SetActive(true);
            }
            
            GetText((int)Texts.ModelTypeText).text = ModelTypeString[modelNum];
        }
        
        void OnWeaponArrowClicked(PointerEventData data)
        {
            if (data.selectedObject == GetButton((int)Buttons.WeaponLeftArrow).gameObject)
            {
                weaponNum--;
                if (weaponNum == 0)
                    data.selectedObject.SetActive(false);
                if (weaponNum != 2)
                    GetButton((int)Buttons.WeaponRightArrow).gameObject.SetActive(true);
            }
            else
            {
                weaponNum++;
                if (weaponNum == 2)
                    data.selectedObject.SetActive(false);
                if (weaponNum != 0)
                    GetButton((int)Buttons.WeaponLeftArrow).gameObject.SetActive(true);
            }
            
            GetText((int)Texts.WeaponTypeText).text = WeaponTypeString[weaponNum];
        }
        
    }
}