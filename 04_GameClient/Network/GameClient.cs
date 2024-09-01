// 2024070617:07
// 2024070617:07
// 김호준rkaghMy projectAssembly-CSharpGameClient.cs


public class GameClient : NetClient
{
    public GameClient()
    {
        base._encodingKey = 0x11;
        base._packetCode = 0x99;
    }
    
    public override void OnConnect()
    {
        
    }

    public override void OnDisconnect()
    {
        
    }
}