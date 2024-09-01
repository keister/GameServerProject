// 2024082301:08
// 2024082301:08
// 김호준rkaghMy projectAssembly-CSharpUI_InteractionGuide.cs


using System;
using TMPro;
using UnityEngine;

public class UI_InteractionGuide : UI_Base
{
    enum Texts
    {
        InfoText
    }

    private GameObject go;
    
    public override void Init()
    {
        Bind<TMP_Text>(typeof(Texts));
    }

    public void Awake()
    {
        Init();
    }

    public void SetObject(GameObject go)
    {
        Transform parent = transform.parent;
        GetText((int)Texts.InfoText).text = go.GetComponent<IInteractable>().GetInfoString();
        transform.position = parent.position + Vector3.up * (parent.GetComponent<Collider>().bounds.size.y);
        transform.rotation = Camera.main.transform.rotation;
    }
}