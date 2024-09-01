// 2024082320:08
// 2024082320:08
// 김호준rkaghMy projectAssembly-CSharpDataManager.cs


using System;
using System.Collections.Generic;
using UnityEngine;

public interface ILoader<Key, Value>
{
    Dictionary<Key, Value> MakeDict();
}

public class DataManager
{
    public Dictionary<UInt64, string> MonsterName = new();

    public void Init()
    {
        MonsterName = LoadJson<MonsterNameData, UInt64, string>("monster").MakeDict();
    }
    
    Loader LoadJson<Loader, Key, Value>(string path) where Loader : ILoader<Key, Value>
    {
        TextAsset textAsset = Managers.Resource.Load<TextAsset>($"Data/{path}");

        return JsonUtility.FromJson<Loader>(textAsset.text);
    }
}