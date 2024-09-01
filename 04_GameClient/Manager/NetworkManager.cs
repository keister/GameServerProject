using System;
using System.Collections;
using System.Collections.Generic;
using Controller.Network;
using UnityEngine;

public class NetworkManager
{
    private static GameObject go;

    public LoginClient Login
    {
        get
        {
            Init();
            return go.GetComponent<LoginClient>();
        }
    }

    public GameClient Game
    {
        get
        {
            Init();
            return go.GetComponent<GameClient>();
        }
    }

    public ChatClient Chat
    {
        get
        {
            Init();
            return go.GetComponent<ChatClient>();
        }
    }


    static void Init()
    {
        go = GameObject.Find("@Network");
        if (go == null)
        {
            go = new GameObject { name = "@Network" };
            go.AddComponent<LoginClient>();
            go.AddComponent<GameClient>();
            go.AddComponent<ChatClient>();
            GameObject.DontDestroyOnLoad(go);
        }
    }

    public void Pause()
    {
        Login.PausePacket();
        Game.PausePacket();
        Chat.PausePacket();
    }

    public void Resume()
    {
        Login.ResumePacket();
        Game.ResumePacket();
        Chat.ResumePacket();
    }
}