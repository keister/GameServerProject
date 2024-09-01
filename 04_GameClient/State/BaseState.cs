using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public abstract class BaseState<T> where T : System.Enum
{
    protected GameObject go;
    
    public BaseState(GameObject go)
    {
        this.go = go;
    }
    
    public abstract void OnEnter(T stateNum);
    public abstract void OnUpdate();
    public abstract void OnFixedUpdate();
    public abstract void OnExit();
}
