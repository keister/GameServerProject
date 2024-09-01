using System;
using UnityEngine;
using Controller.Network;
using UnityEngine.SceneManagement;
using System.Collections.Generic;
public class LoginClient : NetClient
{

    public List<string> GameServerIps;
    public List<UInt16> GameServerPorts;
    
    public LoginClient()
    {
        base._encodingKey = 0x11;
        base._packetCode = 0x99;
    }
    

    public override void OnConnect()
    {
        Debug.Log("OnConnect");
    }

    public override void OnDisconnect()
    {
        //throw new NotImplementedException();
    }
}