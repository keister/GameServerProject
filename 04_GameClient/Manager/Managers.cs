// 2024070301:07
// 2024070301:07
// 김호준rkaghMy projectAssembly-CSharpManager.cs


using System;
using UnityEngine;

public class Managers : MonoBehaviour
{
    static Managers _instance;  
    static Managers Instance { get { Init(); return _instance; } }
    
    private UIManager _ui = new();
    private ResourceManager _resource = new();
    private NetworkManager _network = new();
    private DataManager _data = new();
    
    public static UIManager UI
    {
        get
        {
            Init();
            return Instance._ui;
        }
    }

    public static DataManager Data
    {
        get
        {
            Init();
            return Instance._data;
        }
    }
    
    public static ResourceManager Resource
    {
        get
        {
            Init();
            return Instance._resource;
        }
    }

    public static NetworkManager Network
    {
        get
        {
            Init();
            return Instance._network;
        }
    }
    
    SceneManagerEx _scene = new SceneManagerEx();
    public static SceneManagerEx Scene { get { return Instance._scene; } }
    
    void Start()
    {
        Init();
    }
    
    static void Init()
    {
        if (_instance == null)
        {
            GameObject go = GameObject.Find("@Managers");
            if (go == null)
            {
                go = new GameObject { name = "@Managers" };
                go.AddComponent<Managers>();
                _instance = go.GetComponent<Managers>();
                _instance._data.Init();
            }
            DontDestroyOnLoad(go);
           
        }
    }

    public static void Clear()
    {
        Scene.Clear();
    }
}