// 2024070401:07
// 2024070401:07
// 김호준rkaghMy projectAssembly-CSharpCharacterSelectScene.cs

using System;
using Controller.Network;
using Controller.UI;
using JetBrains.Annotations;
using UnityEngine;

public class CharacterSelectScene : BaseScene
{
    [CanBeNull] private UI_Character_Slot curSlot;
    private UI_CharacterSelect ui;

    private GameObject modelPosition;
    private GameObject model;
    private GameObject weapon;
    private readonly string[] weaponPath = { "Axe2H", "Mace2H", "Spear2H" };
    protected override void Init()
    {
        base.Init();
        modelPosition = Util.FindChild(gameObject, "ModelPosition");
        SceneType = Define.Scene.CharacterSelect;
        Managers.Network.Game.Bind((ushort)Define.PacketType.S_GET_CHARACTER_LIST, OnRecvCharacterList);
        Managers.Network.Game.Bind((ushort)Define.PacketType.S_GAME_ENTER, Handle_S_GAME_ENTER);
        ui = Managers.UI.ShowSceneUI<UI_CharacterSelect>();
    }

    public override void Clear()
    {
        Managers.Network.Game.Unbind((ushort)Define.PacketType.S_GET_CHARACTER_LIST);
        Managers.Network.Game.Unbind((ushort)Define.PacketType.S_GAME_ENTER);
        Managers.Network.Chat.Unbind((ushort)Define.PacketType.S_CHAT_ENTER);
    }
    public void OnRecvCharacterList(Packet pkt)
    {
        pkt.Read(out UInt16 size);
        Debug.Log($"{size}");
        for (int i = 0; i < size; i++)
        {
            pkt.Read(out UInt64 id)
                .Read(out string nickname)
                .Read(out int level)
                .Read(out int modelId)
                .Read(out int weaponId)
                .Read(out int fieldId);
                
            Debug.Log($"{id}, {nickname}, {level}, {modelId}, {weaponId}");
                
            ui.Slots[i].gameObject.SetActive(true);
            ui.Slots[i].SetInfo(id, i, nickname, level, modelId, weaponId, fieldId);
        }

        if (size != 0)
        {
            ui.curSlot = ui.Slots[0];
            ui.curSlot.btn.Select();
        }
    }
    void Update()
    {
        if (curSlot != ui.curSlot)
        {

            if (curSlot is not null)
            {
                Managers.Resource.Destroy(model);
            }
            
            curSlot = ui.curSlot;
            model = Managers.Resource.Instantiate($"Character/Type{curSlot._modelNum + 1}", modelPosition.transform);
            GameObject prop = Util.FindChild(model.gameObject, "Prop_R", true);
            Managers.Resource.Instantiate($"Weapons/{weaponPath[curSlot._weaponNum]}", prop.transform);
        }
    }
    
    void Handle_S_GAME_ENTER(Packet pkt)
    {
        Player p = Player.Instance;
        float y, x;

        pkt.Read(out p.id)
            .Read(out p.characterId);

        // p.character.stat.Hp = hp;
        // p.character.stat.MoveSpeed = moveSpeed;
        // p.character.stat.Level = level;
        //
        // p.character.modelId = curSlot._modelNum;
        // p.character.weaponId = curSlot._weaponNum;
        //
        // p.transform.position = new Vector3(x, 0f, y);
        

        
        Managers.Scene.LoadScene((Define.Scene)curSlot._fieldId);

    }
}