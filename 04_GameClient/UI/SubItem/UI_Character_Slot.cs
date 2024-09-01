// 2024071201:07
// 2024071201:07
// 김호준rkaghMy projectAssembly-CSharpUI_Character_Slot.cs

using System;
using System.Text;
using Controller.Network;
using TMPro;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;

public class UI_Character_Slot : UI_Base
{
    enum Buttons
    {
        Button,
    }

    enum Texts
    {
        nickname,
        level,
    }

    public UInt64 _id { get; private set; }
    public string _nickname { get; private set; }
    public int _level { get; private set; }
    public int _modelNum { get; private set; }
    public int _weaponNum { get; private set; }

    public int _idx;

    public int _fieldId { get; private set; }
    
    private void Awake()
    {
        Init();
    }

    public Button btn;

    public override void Init()
    {
        Bind<TMP_Text>(typeof(Texts));
        Bind<Button>(typeof(Buttons));
        btn = GetButton((int)Buttons.Button);
    }

    public void SetInfo(UInt64 id, int idx, string nickname, int level, int modelNum, int weaponNum, int fieldId)
    {
        _id = id;
        _idx = idx;
        _nickname = nickname;
        _level = level;
        _modelNum = modelNum;
        _weaponNum = weaponNum;
        _fieldId = fieldId;
        Debug.Log($"{_id}, {_nickname}, {_level}, {_modelNum}, {_weaponNum}");
        GetText((int)Texts.nickname).text = _nickname;
        GetText((int)Texts.level).text = _level.ToString();
    }
}