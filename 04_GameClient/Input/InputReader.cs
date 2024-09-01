using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.InputSystem;
using UnityEngine.InputSystem.Interactions;

[CreateAssetMenu(menuName = "InputReader")]
public class InputReader : ScriptableObject, PlayerAction.IPlayerActions, PlayerAction.IUIActions
{
    private PlayerAction _input;

    private void OnEnable()
    {
        if (_input == null)
        {
            _input = new PlayerAction();
            _input.Player.SetCallbacks(this);
            _input.UI.SetCallbacks(this);
            _input.Player.Enable();
            _input.UI.Disable();
        }
    }

    public void SetGamePlay()
    {
        _input.Player.Enable();
        _input.UI.Disable();
    }

    public void SetUI()
    {
        _input.UI.Enable();
        _input.Player.Disable();
    }

    public event Action<Vector2> moveEvent;
    public event Action dashEvent;
    public event Action<Vector2> attackEvent;

    public event Action<Vector2> clickEvent;

    public event Action chatStartEvent;
    public event Action chatEndEvent;

    public event Action interactionEvent;

    public void OnClick(InputAction.CallbackContext context)
    {
        if (context.performed == true)
            clickEvent?.Invoke(Mouse.current.position.ReadValue());
    }

    public void OnChatStart(InputAction.CallbackContext context)
    {
        if (context.performed == true)
        {
            chatStartEvent?.Invoke();
            SetUI();
        }
    }

    public void OnInteraction(InputAction.CallbackContext context)
    {
        if (context.performed == true)
            interactionEvent?.Invoke();
    }

    public void OnMove(InputAction.CallbackContext context)
    {
        moveEvent?.Invoke(context.ReadValue<Vector2>());
    }

    public void OnAttack(InputAction.CallbackContext context)
    {
        if (context.performed)
        {
            attackEvent?.Invoke(Mouse.current.position.ReadValue());
        }
    }

    public void OnDash(InputAction.CallbackContext context)
    {
        if (context.performed)
        {
            dashEvent?.Invoke();
        }
    }

    public void OnChatEnd(InputAction.CallbackContext context)
    {
        if (context.performed == true)
        {
            chatEndEvent?.Invoke();
            SetGamePlay();
        }
    }
}