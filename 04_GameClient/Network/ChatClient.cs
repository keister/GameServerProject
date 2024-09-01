// 2024080523:08
// 2024080523:08
// 김호준rkaghMy projectAssembly-CSharpChatClient.cs

namespace Controller.Network
{
    public class ChatClient : NetClient
    {
        public ChatClient()
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
}