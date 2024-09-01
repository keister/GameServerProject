// 2024082121:08
// 2024082121:08
// 김호준rkaghMy projectAssembly-CSharpIInteractable.cs


using UnityEngine;

public abstract class IInteractable : MonoBehaviour
{
    public UI_InteractionGuide guide;
    public abstract void Interact();
    public abstract string GetInfoString();
}