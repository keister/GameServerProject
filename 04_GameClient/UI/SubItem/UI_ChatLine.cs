// 2024080421:08
// 2024080421:08
// 김호준rkaghMy projectAssembly-CSharpUI_ChatLine.cs



using TMPro;
using UnityEngine;

public class UI_ChatLine : UI_Base
    {
        private InputReader input;

        private void Awake()
        {
            Init();
        }

        public override void Init()
        {
            
        }


        public void SetText(string txt)
        {
            GetComponent<TMP_Text>().text = txt;
        }

        public void SetColor(Color color)
        {
            GetComponent<TMP_Text>().color = color;
        }
    }
