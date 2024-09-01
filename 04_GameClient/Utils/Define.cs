// 2024070302:07
// 2024070302:07
// 김호준rkaghMy projectAssembly-CSharpDefine.cs


public class Define
{
    public enum Scene
    {
        GameTown,
        Cemetary,
        Unknown,
        Login,
        ServerSelect,
        CharacterSelect,
        CharacterCreate,
    }


    public enum PacketType
    {
        LOGIN_SERVER_PACKET = 100,
        C_LOGIN,
        S_LOGIN,
        C_GET_SERVER_LIST,
        S_GET_SERVER_LIST,
        
        CHAT_SERVER_PACKET = 200,
        C_CHAT_LOGIN,
        S_CHAT_LOGIN,
        C_CHAT_ENTER,
        S_CHAT_ENTER,
        C_CHAT_LEAVE,
        C_CHAT_MOVE_FIELD,
        C_CHAT,
        S_CHAT,
        C_WHISPER,
        S_WHISPER,
        S_CHAT_FAIL,
        
        
        GAME_SERVER_PACKET = 300,
        C_GAME_LOGIN,
        S_GAME_LOGIN,
        C_GET_CHARACTER_LIST,
        S_GET_CHARACTER_LIST,
        C_CREATE_CHARACTER,
        S_CREATE_CHARACTER,
        C_GAME_ENTER,
        S_GAME_ENTER,
        S_SPAWN_CHARACTER,
        S_DESTROY_CHARACTER,
        C_MOVE,
        S_MOVE,
        C_SKILL,
        S_SKILL,
        S_SPAWN_MONSTER,
        S_DESTROY_OBJECT,
        S_MOVE_OBJECT,
        S_DAMAGE,
        S_EXP,
        C_MOVE_FIELD,
        S_MOVE_FIELD,
        S_MONSTER_ATTACK,
        S_LEVEL_UP,
        S_CHARACTER_DEATH,
        C_CHARACTER_RESPAWN,
        S_CHARACTER_RESPAWN,
    }
    
    public enum UIEvent
    { 
        Click,
        Drag,
    }

}
