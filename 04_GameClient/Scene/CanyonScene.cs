// 2024071318:07
// 2024071318:07
// 김호준rkaghMy projectAssembly-CSharpGameTownScene.cs

using System;
using System.Collections;
using System.Collections.Generic;
using Controller.Network;
using myproject;
using Unity.VisualScripting;
using UnityEditor;
using UnityEngine;

namespace Scenes
{
    public class CanyonScene : BaseScene
    {
        private Dictionary<UInt64, GameObject> characters = new();
        private Dictionary<UInt64, GameObject> objects = new();
        private Camera camera;
        private UI_Player ui;
        private UI_Die uiDie;
        private InputReader input;
        private UI_Chat chat;
        private GameObject PlayerCharacter;

        protected override void Init()
        {
            base.Init();
            SceneType = Define.Scene.GameTown;
            camera = Camera.main;

            PlayerCharacter = new GameObject { name = $"@Player_Character" };
            PlayerCharacter.transform.SetParent(transform);
            Character character = PlayerCharacter.AddComponent<Character>();
            characters.Add(Player.Instance.id, PlayerCharacter);
            Player.Instance.character = character;

            uiDie = Managers.UI.MakeSubItem<UI_Die>();
            uiDie.Active(false);
            input = ScriptableObject.CreateInstance<InputReader>();
            ChatManager.Instance.SetField(0);
            camera.AddComponent<PlayerCamera>();
            ui = Managers.UI.ShowSceneUI<UI_Player>();
            Managers.Network.Game.Bind((UInt16)Define.PacketType.S_SPAWN_CHARACTER, Handle_S_SPAWN_CHARACTER);
            Managers.Network.Game.Bind((UInt16)Define.PacketType.S_DESTROY_CHARACTER, Handle_S_DESTROY_CHARACTER);
            Managers.Network.Game.Bind((UInt16)Define.PacketType.S_MOVE, Handle_S_MOVE_OTHER);
            Managers.Network.Game.Bind((UInt16)Define.PacketType.S_SKILL, Handle_S_SKILL_OTHER);
            Managers.Network.Game.Bind((UInt16)Define.PacketType.S_SPAWN_MONSTER, Handle_S_SPAWN_MONSTER);
            Managers.Network.Game.Bind((UInt16)Define.PacketType.S_DAMAGE, Handle_S_DAMAGE);
            Managers.Network.Game.Bind((UInt16)Define.PacketType.S_DESTROY_OBJECT, Handle_S_DESTROY_OBJECT);
            Managers.Network.Game.Bind((UInt16)Define.PacketType.S_MOVE_OBJECT, Handle_S_MOVE_OBJECT);
            Managers.Network.Game.Bind((UInt16)Define.PacketType.S_MOVE_FIELD, Handle_S_MOVE_FILED);
            Managers.Network.Game.Bind((UInt16)Define.PacketType.S_MONSTER_ATTACK, Handle_S_MONSTER_ATTACK);
            Managers.Network.Game.Bind((UInt16)Define.PacketType.S_CHARACTER_DEATH, Handle_S_CHARACTER_DEATH);
            Managers.Network.Game.Bind((UInt16)Define.PacketType.S_CHARACTER_RESPAWN, Handle_S_CHARACTER_RESPAWN);
        }

        public override void Clear()
        {
            Managers.Network.Game.Unbind((UInt16)Define.PacketType.S_SPAWN_CHARACTER);
            Managers.Network.Game.Unbind((UInt16)Define.PacketType.S_DESTROY_CHARACTER);
            Managers.Network.Game.Unbind((UInt16)Define.PacketType.S_MOVE);
            Managers.Network.Game.Unbind((UInt16)Define.PacketType.S_SKILL);
            Managers.Network.Game.Unbind((UInt16)Define.PacketType.S_SPAWN_MONSTER);
            Managers.Network.Game.Unbind((UInt16)Define.PacketType.S_DAMAGE);
            Managers.Network.Game.Unbind((UInt16)Define.PacketType.S_DESTROY_OBJECT);
            Managers.Network.Game.Unbind((UInt16)Define.PacketType.S_MOVE_OBJECT);
            Managers.Network.Game.Unbind((UInt16)Define.PacketType.S_MOVE_FIELD);
            Managers.Network.Game.Unbind((UInt16)Define.PacketType.S_MONSTER_ATTACK);
            Managers.Network.Game.Unbind((UInt16)Define.PacketType.S_CHARACTER_DEATH);
            Managers.Network.Game.Unbind((UInt16)Define.PacketType.S_CHARACTER_RESPAWN);
        }

        void Handle_S_CHARACTER_RESPAWN(Packet pkt)
        {
            pkt.Read(out UInt64 playerId)
                .Read(out int hp);

            if (characters.TryGetValue(playerId, out GameObject obj))
            {
                Character c = obj.GetComponent<Character>();
                c.stateMachine.ChangeState(CharacterState.IDLE);

                c.stat.Hp = hp;
            }

            if (playerId == Player.Instance.id)
            {
                input.SetGamePlay();
                uiDie.Active(false);
            }
        }

        void Handle_S_CHARACTER_DEATH(Packet pkt)
        {
            pkt.Read(out UInt64 playerId);

            if (characters.TryGetValue(playerId, out GameObject obj))
            {
                Character c = obj.GetComponent<Character>();
                c.stateMachine.ChangeState(CharacterState.DEATH);
            }

            if (playerId == Player.Instance.id)
            {
                input.SetUI();
                uiDie.EnableButton(true);
                uiDie.Active(true);
            }
        }

        void Handle_S_SPAWN_CHARACTER(Packet pkt)
        {
            GameObject go;
            Character character;

            pkt.Read(out UInt64 playerId);
            if (playerId == Player.Instance.id)
            {
                go = PlayerCharacter;
                character = Player.Instance.character;
            }
            else
            {
                go = new GameObject { name = $"@Character_{playerId}" };
                go.transform.SetParent(transform);
                characters.Add(playerId, go);
                character = go.AddComponent<Character>();
            }

            pkt.Read(out character.nickname)
                .Read(out int level)
                .Read(out float y)
                .Read(out float x)
                .Read(out int maxHp)
                .Read(out int hp)
                .Read(out int maxMp)
                .Read(out int mp)
                .Read(out int moveSpeed)
                .Read(out character.modelId)
                .Read(out character.weaponId)
                .Read(out float yaw);


            character.stat.MaxHp = maxHp;
            character.stat.Hp = hp;
            character.stat.MaxMp = maxMp;
            character.stat.Mp = mp;
            character.stat.MoveSpeed = moveSpeed;
            character.stat.Level = level;



            //if (playerId != Player.Instance.id)
            {
                Managers.UI.MakeWorldSpace<UI_HPBar>(go.transform).SetName(character.nickname);
            }

            var mask = LayerMask.GetMask("Terrain");
            var targetVec = new Vector3(x, 30f, y);
            var ray = new Ray(targetVec, Vector3.down);
            Physics.Raycast(ray, out RaycastHit hit, 100, mask, QueryTriggerInteraction.Ignore);
            character.transform.position = new Vector3(x, hit.point.y, y);
            character.targetPos = character.transform.position;
            character.transform.rotation = Quaternion.Euler(0f, yaw, 0f);
            character.SetModel();
        }

        void Handle_S_DESTROY_CHARACTER(Packet pkt)
        {
            pkt.Read(out UInt64 playerId);

            if (characters.Remove(playerId, out GameObject obj))
            {
                Managers.Resource.Destroy(obj);
            }
        }

        void Handle_S_DESTROY_OBJECT(Packet pkt)
        {
            pkt.Read(out byte type)
                .Read(out UInt64 id);

            Debug.Log($"{type}, {id}");

            if (objects.Remove(id, out GameObject obj))
            {
                Managers.Resource.Destroy(obj);
            }
        }

        // void Handle_S_MOVE(Packet pkt)
        // {
        //     pkt.Read(out UInt64 playerId)
        //         .Read(out float y)
        //         .Read(out float x);
        //     
        //     if (characters.TryGetValue(playerId, out GameObject obj))
        //     {
        //         MovableObject movableObject = obj.GetComponent<MovableObject>();
        //         movableObject.anim.SetBool("isRun", true);
        //         movableObject.SetTargetPosition(new Vector3(x, 0f, y));
        //
        //     }
        // }

        void Handle_S_MOVE_OTHER(Packet pkt)
        {
            pkt.Read(out UInt64 playerId)
                .Read(out UInt16 size);


            if (characters.TryGetValue(playerId, out GameObject obj))
            {
                Character c = obj.GetComponent<Character>();

                c.path.Clear();
                for (int i = 0; i < size; i++)
                {
                    pkt.Read(out float y)
                        .Read(out float x);
                    c.path.AddPoint(new Vector3(x, 0, y));
                }

                //c.targetPos = new Vector3(x, 0f, y);
            }
        }

        void Handle_S_SKILL_OTHER(Packet pkt)
        {
            pkt.Read(out UInt64 playerId)
                .Read(out int skillId)
                .Read(out float targetY)
                .Read(out float targetX)
                .Read(out float posY)
                .Read(out float posX);

            if (characters.TryGetValue(playerId, out GameObject obj))
            {
                Character c = obj.GetComponent<Character>();

                var mask = LayerMask.GetMask("Terrain");
                var targetVec = new Vector3(posX, 30f, posY);
                var ray = new Ray(targetVec, Vector3.down);
                Physics.Raycast(ray, out RaycastHit hit, 100, mask, QueryTriggerInteraction.Ignore);
                c.transform.position = new Vector3(posX, hit.point.y, posY);
                c.transform.transform.LookAt(new Vector3(targetX, hit.point.y, targetY));
                c.path.Clear();
                c.stateMachine.ChangeState(CharacterState.ATTACK);
            }
        }

        void Handle_S_SPAWN_MONSTER(Packet pkt)
        {
            pkt.Read(out UInt64 id);

            pkt.Read(out UInt64 objId)
                .Read(out int maxHp)
                .Read(out int hp)
                .Read(out int speed)
                .Read(out float y)
                .Read(out float x);

            GameObject go = Managers.Resource.Instantiate($"Monster/{objId}", transform);
            objects.Add(id, go);
            Monster mo = go.AddComponent<Monster>();

            go.name = $"slime_{id}";
            mo.id = id;
            go.transform.SetParent(transform);


            mo.stat.Level = 1;
            mo.stat.Attack = 1;
            mo.stat.MaxHp = maxHp;
            mo.stat.Hp = hp;
            mo.stat.MoveSpeed = speed;
            var mask = LayerMask.GetMask("Terrain");
            var targetVec = new Vector3(x, 30f, y);
            var ray = new Ray(targetVec, Vector3.down);
            Physics.Raycast(ray, out RaycastHit hit, 100, mask, QueryTriggerInteraction.Ignore);
            go.transform.position = new Vector3(x, hit.point.y, y);
            mo.target = go.transform.position;
            if (Managers.Data.MonsterName.TryGetValue(objId, out string name))
            {
                mo._UIHpBar.SetName(name);
            }
        }

        void Handle_S_MONSTER_ATTACK(Packet pkt)
        {
            pkt.Read(out UInt64 id)
                .Read(out float y)
                .Read(out float x)
                .Read(out float yaw);

            if (objects.TryGetValue(id, out GameObject obj))
            {
                Monster mo = obj.GetComponent<Monster>();
                var mask = LayerMask.GetMask("Terrain");
                var targetVec = new Vector3(x, 30f, y);
                var ray = new Ray(targetVec, Vector3.down);
                Physics.Raycast(ray, out RaycastHit hit, 100, mask, QueryTriggerInteraction.Ignore);
                mo.transform.position = new Vector3(x, hit.point.y, y);
                mo.transform.rotation = Quaternion.Euler(0f, yaw, 0f);
                mo.path.Clear();
                mo.StateMachine.ChangeState(MonsterState.ATTACK);
            }
        }

        void Handle_S_DAMAGE(Packet pkt)
        {
            pkt.Read(out Byte objectType)
                .Read(out UInt64 id)
                .Read(out int damage);

            if (objectType == 0)
            {
                if (characters.TryGetValue(id, out GameObject obj))
                {
                    Character ch = obj.GetComponent<Character>();
                    ch.stat.Hp = damage;
                    GameObject go = Managers.Resource.Instantiate("VFX/Hit", transform);
                    go.transform.position = ch.transform.position + new Vector3(0f, 0.4f, -0.7f);
                    go.GetComponent<ParticleSystem>().Play();
                }
            }
            else
            {
                if (objects.TryGetValue(id, out GameObject obj))
                {
                    Monster mo = obj.GetComponent<Monster>();
                    mo.stat.Hp = damage;
                    Debug.Log($"{obj.GetComponent<Monster>().stat.Hp}");
                    GameObject go = Managers.Resource.Instantiate("VFX/Hit", transform);
                    go.transform.position = mo.transform.position + new Vector3(0f, 0.4f, -0.7f);
                    go.GetComponent<ParticleSystem>().Play();
                }
            }
        }

        void Handle_S_MOVE_OBJECT(Packet pkt)
        {
            pkt.Read(out byte type)
                .Read(out UInt64 id)
                .Read(out UInt16 size);


            if (objects.TryGetValue(id, out GameObject obj))
            {
                Monster mo = obj.GetComponent<Monster>();

                mo.path.Clear();
                for (int i = 0; i < size; i++)
                {
                    pkt.Read(out float y)
                        .Read(out float x);
                    mo.path.AddPoint(new Vector3(x, 0, y));
                }
            }
        }

        void Handle_S_MOVE_FILED(Packet pkt)
        {
            Debug.Log("HandleMoveField");
            pkt.Read(out byte fieldId);
            Managers.Scene.LoadScene((Define.Scene)fieldId);
        }
    }
}