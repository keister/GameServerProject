using System.Collections;
using System.Collections.Generic;
using Controller.UI;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

public class UI_Player : UI_Scene
{
    private Stat stat;
    
    enum GameObjects
    {
        Exp_Slider,
        HpBar,
        MpBar,
    }

    enum Texts
    {
        CurHp,
        MaxHp,
        CurMp,
        MaxMp,
        level_value,
        exp_percent,
    }

    private UI_ProgressBar_Value hpBar;
    private UI_ProgressBar_Value mpBar;
    
    private void Awake()
    {
        Init();

    }
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        float maxMp = stat.MaxMp;
        float mp = stat.Mp;
        float maxHp = stat.MaxHp;
        float hp = stat.Hp;

        GetObject((int)GameObjects.HpBar).GetComponent<Slider>().value = hp / maxHp;
        GetText((int)Texts.CurHp).text = hp.ToString();
        GetText((int)Texts.MaxHp).text = maxHp.ToString();
        
        GetObject((int)GameObjects.MpBar).GetComponent<Slider>().value = mp / maxMp;
        GetText((int)Texts.CurMp).text = mp.ToString();
        GetText((int)Texts.MaxMp).text = maxMp.ToString();

        int maxExp = 100 * stat.Level;
        int exp = Player.Instance.character.exp;
        GetText((int)Texts.level_value).text = stat.Level.ToString();
        GetText((int)Texts.exp_percent).text = $"{((float)exp * 100f) / maxExp : #0.0#}";
        
        GetObject((int)GameObjects.Exp_Slider).GetComponent<Slider>().value = (float)exp / maxExp;
    }

    public override void Init()
    {
        stat = Player.Instance.character.stat;
        Bind<GameObject>(typeof(GameObjects));
        Bind<TMP_Text>(typeof(Texts));
    }
}
