using System.Collections.Generic;

namespace Controller
{
    public class StateMachine<T> where T : System.Enum
    {
        public BaseState<T> CurrentState { get; private set; }
        public T CurrentStateNum { get; private set; }
        private Dictionary<T, BaseState<T>> states = new Dictionary<T, BaseState<T>>();

        public StateMachine(T stateNum, BaseState<T> state)
        {
            AddState(stateNum, state);
            CurrentState = state;
            CurrentStateNum = stateNum;
        }
        
        public void AddState(T stateNum, BaseState<T> state)
        {
            if (!states.ContainsKey(stateNum))
            {
                states.Add(stateNum, state);
            }
        }

        public BaseState<T> GetState(T stateNum)
        {
            if (states.TryGetValue(stateNum, out BaseState<T> state))
            {
                return state;
            }

            return null;
        }

        public void DeleteState(T stateNum)
        {
            states.Remove(stateNum);
        }

        public void ChangeState(T stateNum)
        {
            CurrentState?.OnExit();
            T prevState = CurrentStateNum;
            
            if (states.TryGetValue(stateNum, out BaseState<T> newState))
            {
                CurrentState = newState;
                CurrentStateNum = stateNum;
            }
            CurrentState?.OnEnter(prevState);
        }

        public void UpdateState()
        {
            CurrentState?.OnUpdate();
        }

        public void FixedUpdateState()
        {
            CurrentState?.OnFixedUpdate();
        }
    }
}