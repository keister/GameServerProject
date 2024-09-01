// 2024080320:08
// 2024080320:08
// 김호준rkaghMy projectAssembly-CSharpUI_ProgressBar_Value.cs


using System;
using System.Threading;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

public class UI_ProgressBar_Value : UI_Base
{
    enum Images
    {
        Fill,
    }

    enum Texts
    {
        Cur,
        Max,
    }
    enum GameObjects
    {
        ProgressBar
    }

    private void Start()
    {
        Init();
    }

    private float cur;
    private float max;
    
    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));
        Bind<TMP_Text>(typeof(Texts));
        Bind<Image>(typeof(Images));
    }

    private void Update()
    {
        GetObject((int)GameObjects.ProgressBar).GetComponent<Slider>().value = cur / max;
    }

    public void SetColor(Color color)
    {
        GetImage((int)Images.Fill).color = color;
    }

    public void SetMax(float val)
    {
        max = val;
        GetText((int)Texts.Max).text = max.ToString();
    }

    public void SetValue(float val)
    {
        cur = val;
        GetText((int)Texts.Max).text = cur.ToString();
    }
}