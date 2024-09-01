// 2024072500:07
// 2024072500:07
// 김호준rkaghMy projectAssembly-CSharpStat.cs


using UnityEngine;

public class Stat : MonoBehaviour
{
    // 멤버 변수
    [SerializeField] protected int _level;
    [SerializeField] protected int _hp;
    [SerializeField] protected int _maxHp;
    [SerializeField] protected int _mp;
    [SerializeField] protected int _maxMp;
    [SerializeField] protected int _attack;
    [SerializeField] protected float _moveSpeed;

    // 멤버 프로퍼티
    public int Level
    {
        get { return _level; }
        set { _level = value; }
    }

    public int Hp
    {
        get { return _hp; }
        set { _hp = value; }
    }

    public int MaxHp
    {
        get { return _maxHp; }
        set { _maxHp = value; }
    }
    
    public int Mp
    {
        get { return _mp; }
        set { _mp = value; }
    }

    public int MaxMp
    {
        get { return _maxMp; }
        set { _maxMp = value; }
    }


    public int Attack
    {
        get { return _attack; }
        set { _attack = value; }
    }

    public float MoveSpeed
    {
        get { return _moveSpeed; }
        set { _moveSpeed = value; }
    }
}