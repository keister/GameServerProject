using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;

public class UI_EventHandler : MonoBehaviour, IDragHandler, IPointerClickHandler
{
    [SerializeField]
    public Action<PointerEventData> OnClickHandler = null;
    [SerializeField]
    public Action<PointerEventData> OnDragHandler = null;
    public void OnPointerClick(PointerEventData eventData)
    {
        if (OnClickHandler != null)
        {
            if (EventSystem.current.currentSelectedGameObject != null)
            {
                OnClickHandler.Invoke(eventData);
            }
        }
    }
    public void OnDrag(PointerEventData eventData)
    {
        if (OnDragHandler != null)
        {
            OnDragHandler.Invoke(eventData);
        }
            
    }
}
