using System.Collections;
using System.Collections.Generic;
using Controller.UI;
using UnityEngine;

public class ServerSelectScene : BaseScene
{
    // Start is called before the first frame update
    void Start()
    {
        base.Init();

        SceneType = Define.Scene.Login;

        Managers.UI.ShowSceneUI<UI_ServerSelect>();
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public override void Clear()
    {
        
    }
}
