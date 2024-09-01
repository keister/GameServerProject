// 2024081900:08
// 2024081900:08
// 김호준rkaghMy projectAssembly-CSharpPathReciever.cs


using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PathReciever : MonoBehaviour
{


    public void AddPoint(Vector3 point)
    {
        path.Add(point);
    }

    public int Left()
    {
        return path.Count - cur;
    }

    public void Clear()
    {
        cur = 0;
        path.Clear();
    }

    public Vector3 GetCur()
    {
        return path[cur];
    }

    public void Next()
    {
        cur++;
    }
    
    public List<Vector3> path = new();
    public int cur = 0;
}