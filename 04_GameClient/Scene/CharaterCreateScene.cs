using System;
using System.Collections;
using System.Collections.Generic;
using Controller.Network;
using Controller.UI;
using UnityEngine;

public class CharaterCreateScene : BaseScene
{
    private GameObject[] models = new GameObject[3];

    private int nowModel = 1;
    private int nowWeapon = 1;

    private readonly string[] weaponPath = { "Axe2H", "Mace2H", "Spear2H" };
    
    private UI_CharacterCreate ui;
    protected override void Init()
    {
        base.Init();
        Managers.Network.Game.Bind((UInt16)Define.PacketType.S_CREATE_CHARACTER, OnRecv);
        SceneType = Define.Scene.CharacterCreate;
        ui = Managers.UI.ShowSceneUI<UI_CharacterCreate>();
        GameObject go = Util.FindChild(gameObject, "ModelPosition");

        for (int i = 0; i < 3; i++)
        {
            models[i] = Managers.Resource.Instantiate($"Character/Type{i + 1}", go.transform);
            //SetWeapon();
            models[i].SetActive(false);
        }
        
        models[nowModel].SetActive(true);
        SetWeapon();
    }

    public void Update()
    {
        if (nowModel != ui.modelNum)
        {
            models[nowModel].SetActive(false);
            nowModel = ui.modelNum;
            models[nowModel].SetActive(true);
            SetWeapon();
        }

        if (nowWeapon != ui.weaponNum)
        {
            nowWeapon = ui.weaponNum;
            SetWeapon();
        }
    }

    void SetWeapon()
    {
        GameObject prop = Util.FindChild(models[nowModel].gameObject, "Prop_R", true);
        if (prop.transform.childCount != 0)
        {
            Managers.Resource.Destroy(prop.transform.GetChild(0).gameObject);
        }
        Managers.Resource.Instantiate($"Weapons/{weaponPath[nowWeapon]}", prop.transform);
    }

    void OnRecv(Packet pkt)
    {
        Managers.Scene.LoadScene(Define.Scene.CharacterSelect);
    }
    
    public override void Clear()
    {
        Managers.Network.Game.Unbind((UInt16)Define.PacketType.S_CREATE_CHARACTER);
    }
}
