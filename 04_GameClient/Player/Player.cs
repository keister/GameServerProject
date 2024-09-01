using System;
using Controller.Network;
using Controller.UI;
using UnityEngine;

public class Player : MonoBehaviour
{
    public static Player Instance
    {
        get
        {
            Init();
            return go.GetComponent<Player>();
        }
    }

    private static GameObject go;
    public UInt64 id;
    public UInt64 characterId;
    public Character character;
    public TargetView targetView;
    public InteractionView interactView;

    [SerializeField] public float DashCoolDownTimer;

    static void Init()
    {
        if (go == null)
        {
            go = new GameObject { name = "@Player" };
            Player player = go.AddComponent<Player>();
            
            DontDestroyOnLoad(go.gameObject);
        }
    }


    private void Awake()
    {
        targetView = gameObject.AddComponent<TargetView>();
        interactView = gameObject.AddComponent<InteractionView>();
        PlayerController controller = Util.GetOrAddComponent<PlayerController>(gameObject);
        Managers.Network.Game.Bind((UInt16)Define.PacketType.S_EXP, Handle_S_EXP);
        Managers.Network.Game.Bind((UInt16)Define.PacketType.S_LEVEL_UP, Handle_S_LEVEL_UP);
    }

    private void Update()
    {
        if (character != null)
        {
            transform.position = character.transform.position;
            transform.rotation = character.transform.rotation;
        }
    }

    void Handle_S_EXP(Packet pkt)
    {
        pkt.Read(out int exp);

        character.exp = exp;
    }
    
    void Handle_S_LEVEL_UP(Packet pkt)
    {
        pkt.Read(out int level);

        character.stat.Level = level;
    }
    // Update is called once per frame

}