namespace Scenes
{
    public class LoginScene : BaseScene
    {
        protected override void Init()
        {
            base.Init();

            SceneType = Define.Scene.Login;

            Managers.UI.ShowSceneUI<UI_Login>();
        }
        public override void Clear()
        {
        }

        void Update()
        {
        
        }
    }
}